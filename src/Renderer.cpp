#include <algorithm>

#include <Core/ID.hpp>
#include <Material/MaterialInstance.hpp>
#include <OpenGL/DrawCommand.hpp>
#include <Renderer/Renderer.hpp>


using namespace TLOT;

void SetupTechnique(Technique & technique)
{
	if (technique.enableDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(technique.depthTestFunction);
	}
	else
		glDisable(GL_DEPTH_TEST);

	if (technique.enableBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(technique.blendFunctionSource, technique.blendFunctionDestination);
	}
	else
		glDisable(GL_BLEND);

	glFrontFace(technique.frontFace);
}

Renderer::Renderer (std::shared_ptr<VertexTemplate> vertexTemplate, size_t windowWidth, size_t windowHeight)
{
	m_geometry  .Create(vertexTemplate);
	m_matrixSSBO.Create(GL_DYNAMIC_DRAW, 1, 10000000);
	m_indexSSBO .Create(GL_DYNAMIC_DRAW, 0, 10000000);

	m_persp = glm::perspective (glm::radians (45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
	m_ortho = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
}

TextureQuad Renderer::Quad(ResourceHandle handle)
{
	m_atlas.Feed(handle);
	TextureQuad const & textureQuad = m_atlas.Quad (handle);
	TextureQuad scaled;

	 // half pixel correction
	float xOffset = 0.25;
	float yOffset = 0.25;

	if (textureQuad.w == 1)
		xOffset = 0.0;

	if (textureQuad.h == 1)
		yOffset = 0.0;

	scaled.x = (textureQuad.x + xOffset) / m_atlas.Width  ();
	scaled.w = textureQuad.w / m_atlas.Width  ();
	scaled.y = (textureQuad.y + yOffset) / m_atlas.Height ();
	scaled.h = textureQuad.h / m_atlas.Height ();
	scaled.depth = static_cast<float> (textureQuad.depth);

	return scaled;
}

ID64_t Renderer::CreateProgram(ResourceHandle vertexSource, ResourceHandle fragmentSource)
{
	ShaderProgram shader;

	if (vertexSource != InvalidResource)
	{
		auto vertex = AssetManager::GetShaderSource(vertexSource);
		if (vertex.HasValue())
			shader.CompileVertex(vertex.Get().source);
		else return 0;
	}
	else
		return 0;

	if (fragmentSource != InvalidResource)
	{
		auto fragment = AssetManager::GetShaderSource(fragmentSource);
		if (fragment.HasValue())
			shader.CompileFragment(fragment.Get().source);
		else return 0;
	}
	else
		return 0;

	shader.Link ();

	ID64_t handle = GenerateOrderedSetHash({vertexSource, fragmentSource});
	
	m_programs.emplace(handle, std::move(shader));

	return handle; // should it returns the opengl handle instead ?
}

void Renderer::RegisterTechnique(Technique technique)
{
	auto materialTemplate = AssetManager::GetMaterialTemplate(technique.material);

	if (!materialTemplate.HasValue())
		return;

	m_techniques.emplace_back(technique);
	m_materialSSBO[technique.material].Create(GL_DYNAMIC_DRAW, 2, 10000000);
	m_drawCommandDIBO[technique.material].Create(GL_DYNAMIC_DRAW, 10000000);
}

SceneObject Renderer::Instanciate(ResourceHandle geometryID, MaterialInstance material, Transform transform)
{
	auto geometry = AssetManager::GetGeometry(geometryID);
	SceneObject object;

	if(!geometry)
	{
		object.geometryID = InvalidResource;
	}

	m_geometry.RegisterGeometry(geometryID, geometry.Get());

	object.geometryID = geometryID;
	object.materialID = material.GetTemplateHandle();

	object.indexCount = m_geometry.GetIndexCount(geometryID);
	object.firstIndex = m_geometry.GetFirstIndex(geometryID);
	object.baseVertex = m_geometry.GetFirstVertex(geometryID);

	object.matrixIndex   = m_matrixLocation  .Create();
	object.materialIndex = m_materialLocation.Create(material.GetTemplateHandle());

	glm::mat4 matrix = transform.GetModelMatrix();
	m_matrixSSBO.Upload(matrix, object.matrixIndex * sizeof(glm::mat4));
	m_materialSSBO[object.materialID].Upload(material.Data(), object.materialIndex * material.Size());

	return object;
}

void Renderer::KillInstance(SceneObject instance)
{
	if (instance.geometryID == InvalidResource)
		return;

	m_matrixLocation  .Destroy(instance.matrixIndex);
	m_materialLocation.Destroy(instance.materialID, instance.matrixIndex);
}

void Renderer::Update(SceneObject instance, MaterialInstance material, Transform transform)
{
	if (instance.geometryID == InvalidResource)
		return;
	
	glm::mat4 matrix = transform.GetModelMatrix();
	m_matrixSSBO.Upload(matrix, instance.matrixIndex * sizeof(glm::mat4));
	m_materialSSBO[instance.materialID].Upload(material.Data(), instance.materialIndex * material.Size());
}

void Renderer::Update(SceneObject instance, MaterialInstance material)
{
	if (instance.geometryID == InvalidResource)
		return;

	m_materialSSBO[instance.materialID].Upload(material.Data(), instance.materialIndex * material.Size());
}

void Renderer::Update(SceneObject instance, Transform transform)
{
	if (instance.geometryID == InvalidResource)
		return;
	
	glm::mat4 matrix = transform.GetModelMatrix();
	m_matrixSSBO.Upload(matrix, instance.matrixIndex * sizeof(glm::mat4));
}


void Renderer::Render(SceneObject instance)
{
	if (instance.geometryID == InvalidResource)
		return;

	m_toRender[instance.materialID].emplace_back(instance);
}

void Renderer::Render()
{
	Sync();

	// should be creating different commands for different materials
	auto renderData = BuildCommands(m_toRender);
	m_indexSSBO.Upload(renderData.instanceDataSSBO, 0);

	m_geometry.Bind();
	m_matrixSSBO.Bind();
	//m_indexBuffer .Bind();

	// for each technique apply program and drawcall
	for (auto & technique : m_techniques)
	{
		std::vector<DrawCommand> & commands = renderData.commands[technique.material];
		m_drawCommandDIBO[technique.material].Upload(commands, 0);
		m_drawCommandDIBO[technique.material].Bind();

		ShaderProgram & program = m_programs.at(technique.program);
		m_materialSSBO[technique.material].Bind();
		program.Use();

		if (technique.mode == ProjectionMode::Perspective)
			program.UploadMatrix4("uProjection", m_persp);

		else if (technique.mode == ProjectionMode::Orthographic)
			program.UploadMatrix4("uProjection", m_ortho);

		if (technique.activeCamera == (uint32_t)-1)
			program.UploadMatrix4("uView", glm::mat4{1.0}); // TODO: allow multiple cameras

		SetupTechnique(technique);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_atlas.Get ());
		program.Upload1i("uAtlas", 0); // technically useless as showed per previous experiment (opengl seems to autobind uAtlas to the active texture 0)

		glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, commands.size(), 0);

		program.Cancel();
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);  // same for dibo

	m_toRender.clear();
}

void Renderer::Sync()
{
	m_geometry.Upload();
	m_atlas.Generate();
}

Renderer::FrameRenderData Renderer::BuildCommands(std::map<ResourceHandle, std::vector<SceneObject>> const & objects)
{
	FrameRenderData renderData;
	renderData.instanceDataSSBO.reserve(objects.size());

	uint32_t baseInstance = 0;
	for (auto & [material, sceneObjects] : objects)
	{
		renderData.commands[material].reserve(sceneObjects.size()); // Capacité max théorique

		// 1) Culling
		std::vector<const SceneObject*> visibleObjects;
		visibleObjects.reserve(sceneObjects.size());

		for (const auto& obj : sceneObjects)
		{
			// TODO : implement frustum culling
			// TODO : implement visibility on only certain techniques per materials ?
			if (obj.isVisible) 
			{
				visibleObjects.push_back(&obj);
			}
		}

		if (visibleObjects.empty()) continue;

		// 2) Sort by geometry
		std::sort(visibleObjects.begin(), visibleObjects.end(), 
			[](const SceneObject* a, const SceneObject* b) {
				return a->geometryID < b->geometryID;
			});

		// 3) Command Creation and SSBO filling
		ResourceHandle currentGeometryID = ~0;
		
		for (const SceneObject* obj : visibleObjects)
		{
			if (obj->geometryID != currentGeometryID)
			{
				currentGeometryID = obj->geometryID;

				DrawCommand cmd{};
				cmd.indexCount    = obj->indexCount;
				cmd.instanceCount = 0;
				cmd.firstIndex    = obj->firstIndex;
				cmd.baseVertex    = obj->baseVertex;
				cmd.baseInstance  = baseInstance;

				renderData.commands[material].push_back(cmd);
			}

			renderData.instanceDataSSBO.push_back({
				obj->matrixIndex,
				obj->materialIndex
			});

			renderData.commands[material].back().instanceCount++;
			baseInstance++;
		}
	}

	return renderData;
}
