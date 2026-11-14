
#include <TetoEngine/Private/Renderer/Renderer.hpp>

#include <TetoEngine/Application/Application.hpp>
#include <TetoEngine/Private/Renderer/MaterialTemplateSTD430.hpp>

using namespace TetoEngine;

InstanceRenderer::InstanceRenderer(Application * app)
	: app_ {app}
	, textureAtlas_ {app, 2048, 2048, 16}
{
	init();
}

void InstanceRenderer::init()
{
	geometryBuffer_.create();
	indexSSBO_ .create(GL_DYNAMIC_DRAW, 0, 10000000);
	matrixSSBO_.create(GL_DYNAMIC_DRAW, 1, 10000000);

	auto viewport = app_->viewport();
	perspective_ = glm::perspective(glm::radians (45.0f), viewport.AspectRatio(), 0.1f, 100.0f);
	ortho_ = glm::ortho(0.0f, static_cast<float>(viewport.width_), 0.0f, static_cast<float>(viewport.height_), -10.0f, 10.0f);
}

void InstanceRenderer::sync()
{
	geometryBuffer_.upload();
	textureAtlas_  .generate();
	fontAtlas_     .generate();
}

inline TRID getProgramHash(ResourceView<ShaderSource> vertex, ResourceView<ShaderSource> fragment)
{
	return generate_ordered_set_hash({vertex.handle(), fragment.handle()});
}

FrameRenderData InstanceRenderer::buildCommands(std::map<ID64, std::vector<TetoInstance>> const & objects)
{
	FrameRenderData renderData;
	renderData.instanceDataSSBO.reserve(objects.size());

	uint32_t baseInstance = 0;
	for (auto & [material, sceneObjects] : objects)
	{
		renderData.commands[material].reserve(sceneObjects.size()); // Capacité max théorique

		// 1) Culling
		// TODO : implement frustum culling
		std::vector<const TetoInstance*> visibleObjects;
		visibleObjects.reserve(sceneObjects.size());

		for (const auto& obj : sceneObjects)
		{
			if (obj.visible_) 
			{
				visibleObjects.push_back(&obj);
			}
		}

		if (visibleObjects.empty()) continue;

		// 2) Sort by geometry
		std::sort(visibleObjects.begin(), visibleObjects.end(), 
			[] (const TetoInstance* a, const TetoInstance* b)
			{
				return a->geometryID < b->geometryID;
			}
		);

		// 3) Command Creation and SSBO filling
		ID64 currentGeometryID = ~0;
		
		for (const TetoInstance* obj : visibleObjects)
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

void InstanceRenderer::createMaterialBuffer(ID64 handle)
{
	if (registeredMaterial_.contains(handle)) return;

	materialSSBO_     .at(handle).create(GL_DYNAMIC_DRAW, 2, 10000000);
	drawCommandBuffer_.at(handle).create(GL_DYNAMIC_DRAW, 10000000);

	registeredMaterial_.emplace(handle);
}

void InstanceRenderer::registerTechnique(Technique technique)
{
	techniqueList_.push_back(technique);

	createMaterialBuffer(technique.material);
}

TRID InstanceRenderer::registerTexture(ID64 handle)
{
	return TRID();
}

TRID InstanceRenderer::createFont(ID64 handle)
{
	return TRID();
}

TRID InstanceRenderer::generateProgram(ID64 vertexHandle, ID64 fragmentHandle)
{
	auto vertexView   = app_->getShaderSource(vertexHandle);
	auto fragmentView = app_->getShaderSource(fragmentHandle);

	if (!(vertexView && fragmentView))
	{
		return InvalidID;
	}

	TRID handle = getProgramHash(vertexView, fragmentView);

	if (programList_.contains(handle))
	{
		return handle;
	}

	ShaderProgram program;
	program.compileVertex  (vertexView  .get()->source_);
	program.compileFragment(fragmentView.get()->source_);
	program.link ();
	
	programList_.emplace(handle, std::move(program));

	return handle;
}

std::optional<Material> InstanceRenderer::generateMaterial(ID64 handle)
{
	auto materialView = app_->getMaterialLayout(handle);
	if (!materialView) return std::nullopt;
	
	std::polymorphic<MaterialTemplate> instanceMaterialTemplate { std::in_place_type<MaterialTemplateSTD430> };

	if (materialTemplates_.contains(handle))
	{
		std::polymorphic<MaterialTemplate> contained { materialTemplates_.at(handle) };
		instanceMaterialTemplate.swap(contained);
	}
	else
	{
		for (auto uniform : materialView->instanceProperties)
		{
			instanceMaterialTemplate->addProperty(uniform.first, uniform.second);
		}
		
		materialTemplates_.emplace(handle, instanceMaterialTemplate);
		createMaterialBuffer(handle);
	}

	auto vertexHandle   = app_->pullResource(materialView->shader.first);
	auto fragmentHandle = app_->pullResource(materialView->shader.second);

	if (vertexHandle == InvalidID || fragmentHandle == InvalidID) return std::nullopt;

	auto programHandle = generateProgram(vertexHandle, fragmentHandle);
	auto program = programList_.at(programHandle);
	
	return Material { instanceMaterialTemplate, program };
}

TetoInstance InstanceRenderer::instanciate(ID64 handle)
{
	auto mesh = app_->getMesh(handle);
	if (!mesh) return TetoInstance { .geometryID = InvalidID };

	auto geometry = app_->getGeometry      (mesh->geometry_);
	auto material = app_->getMaterialLayout(mesh->material_);
	if(!geometry || !material) return TetoInstance { .geometryID = InvalidID };
	
	createMaterialBuffer(material.handle());
	geometryBuffer_.registerGeometry(geometry.handle(), geometry.get());
	
	TetoInstance object;
	object.geometryID    = geometry.handle();
	object.materialID    = material.handle();
	object.indexCount    = geometryBuffer_.getIndexCount (geometry.handle());
	object.firstIndex    = geometryBuffer_.getFirstIndex (geometry.handle());
	object.baseVertex    = geometryBuffer_.getFirstVertex(geometry.handle());
	object.matrixIndex   = matrixIndexBuffer_  .get();
	object.materialIndex = materialIndexBuffer_.get(material.handle());

	return object;
}

TetoInstance InstanceRenderer::instanciate(ID64 handle, glm::mat4 model, Material material)
{
	auto object = instanciate(handle);

	matrixSSBO_.upload(model, object.matrixIndex * sizeof(glm::mat4));
	materialSSBO_
		.at(object.materialID)
		.upload(material.rawProperties(), object.materialIndex * material.propertiesSize());

	return object;
}

void InstanceRenderer::updateInstance(TetoInstance instance, glm::mat4 model, Material material)
{
	if (instance.geometryID == InvalidID)
		return;

	matrixSSBO_.upload(model, instance.matrixIndex * sizeof(glm::mat4));
	materialSSBO_[instance.materialID].upload(material.rawProperties(), instance.materialIndex * material.propertiesSize());
}

void InstanceRenderer::updateInstance(TetoInstance instance, glm::mat4 model)
{
	if (instance.geometryID == InvalidID) return;

	matrixSSBO_.upload(model, instance.matrixIndex * sizeof(glm::mat4));
}

void InstanceRenderer::updateInstance(TetoInstance instance, Material material)
{
	if (instance.geometryID == InvalidID) return;

	materialSSBO_[instance.materialID].upload(material.rawProperties(), instance.materialIndex * material.propertiesSize());
}

void InstanceRenderer::drawInstance(TetoInstance instance)
{
	if (instance.geometryID == InvalidID) return;
	if (!instance.visible_) return;
	
	renderList_[instance.materialID].emplace_back(instance);
}

void InstanceRenderer::killInstance(TetoInstance & instance)
{
	if (instance.geometryID == InvalidID)
		return;

	matrixIndexBuffer_  .destroy(instance.matrixIndex);
	materialIndexBuffer_.destroy(instance.materialID, instance.materialIndex);

	instance.geometryID = InvalidID;
}

GLenum tatgle(Technique::Arg arg)
{
	switch (arg)
	{
	case Technique::Arg::DEPTH_LESS:                return GL_LESS;
	case Technique::Arg::BLEND_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
	case Technique::Arg::BLEND_SRC_ALPHA:           return GL_SRC_ALPHA;
	case Technique::Arg::FACE_CCW:                  return GL_CCW;
	case Technique::Arg::FACE_CW:                   return GL_CW;
	default: return 0;
	}
}

void setup_technique(Technique & technique)
{
	if (technique.enableDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(tatgle(technique.depthTestFunction));
		
	}
	else
		glDisable(GL_DEPTH_TEST);

	if (technique.enableBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(tatgle(technique.blendFunctionSource), tatgle(technique.blendFunctionDestination));
	}
	else
		glDisable(GL_BLEND);

	if (technique.enableDepthMask)
	{
		glDepthMask(GL_TRUE);
	}
	else
		glDepthMask(GL_FALSE);

	glFrontFace(tatgle(technique.frontFace));
}

void InstanceRenderer::render()
{
	sync();

	// should be creating different commands for different materials
	auto renderData = buildCommands(renderList_);
	indexSSBO_.upload(renderData.instanceDataSSBO, 0);

	geometryBuffer_.bind();
	matrixSSBO_.bind();
	//m_indexBuffer .bind();

	// for each technique apply program and drawcall
	for (auto & technique : techniqueList_)
	{
		std::vector<DrawCommand> & commands = renderData.commands[technique.material];
		drawCommandBuffer_[technique.material].upload(commands, 0);
		drawCommandBuffer_[technique.material].bind();

		if (!programList_.contains(technique.program))
			continue;

		ShaderProgram & program = programList_.at(technique.program);
		materialSSBO_[technique.material].bind();
		program.use();

		if (technique.mode == ProjectionMode::Perspective)
			program.uploadMatrix4("uProjection", perspective_);

		else if (technique.mode == ProjectionMode::Orthographic)
			program.uploadMatrix4("uProjection", ortho_);

		program.uploadMatrix4("uView", glm::mat4{1.0}); // TODO: allow multiple cameras

		setup_technique(technique);
		
		glActiveTexture(GL_TEXTURE0);

		// we dont want to do that :/
		// probably a special method to render text instead
		if (technique.useFontAtlas)
			glBindTexture(GL_TEXTURE_2D_ARRAY, fontAtlas_.get());
		else
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureAtlas_.handle());

		program.upload1i("uAtlas", 0); // technically useless as showed per previous experiment (opengl seems to autobind uAtlas to the active texture 0)

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, commands.size(), 0);

		program.cancel();
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	renderList_.clear();
}