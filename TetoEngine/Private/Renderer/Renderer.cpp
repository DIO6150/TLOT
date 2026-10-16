
#include <TetoEngine/Private/Renderer/Renderer.hpp>

#include <TetoEngine/Application/Application.hpp>
#include <TetoEngine/Private/Renderer/MaterialTemplateSTD430.hpp>

using namespace TetoEngine;

Renderer::Renderer(Application * app)
	: app_ {app}
	, textureAtlas_ {app, 2048, 2048, 16}
{
	init();
}

void Renderer::init()
{
	geometryBuffer_.create();
	indexSSBO_ .create(GL_DYNAMIC_DRAW, 0, 10000000);
	matrixSSBO_.create(GL_DYNAMIC_DRAW, 1, 10000000);

	auto viewport = app_->viewport();
	perspective_ = glm::perspective(glm::radians (45.0f), viewport.AspectRatio(), 0.1f, 100.0f);
	ortho_ = glm::ortho(0.0f, static_cast<float>(viewport.width_), 0.0f, static_cast<float>(viewport.height_), -10.0f, 10.0f);
}

void Renderer::sync()
{
	geometryBuffer_.upload();
	textureAtlas_  .generate();
	fontAtlas_     .generate();
}

TRID Renderer::getProgramHash(ResourceView<ShaderSource> vertex, ResourceView<ShaderSource> fragment)
{
	return generate_ordered_set_hash({vertex.handle(), fragment.handle()});
}

FrameRenderData Renderer::buildCommands(std::map<ID64, std::vector<TIRID>> const & objects)
{
	FrameRenderData renderData;
	renderData.instanceDataSSBO.reserve(objects.size());

	uint32_t baseInstance = 0;
	for (auto & [material, sceneObjects] : objects)
	{
		renderData.commands[material].reserve(sceneObjects.size()); // Capacité max théorique

		// 1) Culling
		// TODO : implement frustum culling
		std::vector<const TIRID*> visibleObjects;
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
			[] (const TIRID* a, const TIRID* b)
			{
				return a->geometryID < b->geometryID;
			}
		);

		// 3) Command Creation and SSBO filling
		ID64 currentGeometryID = ~0;
		
		for (const TIRID* obj : visibleObjects)
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

void Renderer::createMaterialBuffer(ID64 handle)
{
	if (registeredMaterial_.contains(handle)) return;

	materialSSBO_     .at(handle).create(GL_DYNAMIC_DRAW, 2, 10000000);
	drawCommandBuffer_.at(handle).create(GL_DYNAMIC_DRAW, 10000000);

	registeredMaterial_.emplace(handle);
}

void Renderer::registerTechnique(Technique technique)
{
	techniqueList_.push_back(technique);

	createMaterialBuffer(technique.material);
}

TRID Renderer::registerTexture(ID64 handle)
{
	return TRID();
}

TRID Renderer::createFont(ID64 handle)
{
	return TRID();
}

TRID Renderer::createProgram(ID64 vertexHandle, ID64 fragmentHandle)
{
	auto vertexView   = app_->getShaderSource(vertexHandle);
	auto fragmentView = app_->getShaderSource(fragmentHandle);

	if (!(vertexView && fragmentView))
	{
		return InvalidID;
	}

	ShaderProgram program;
	program.compileVertex  (vertexView  .get()->source_);
	program.compileFragment(fragmentView.get()->source_);
	program.link ();
	
	TRID handle = getProgramHash(vertexView, fragmentView);
	programList_.emplace(handle, std::move(program));

	return handle;
}

Material Renderer::generateMaterial(ID64 handle)
{
	if (materialTemplates_.contains(handle))
	{
		return Material { materialTemplates_.at(handle), handle };
	}

	auto materialView = app_->getMaterialLayout(handle);
	if (!materialView) return Material {};
	
	auto std430 = std::make_shared<MaterialTemplateSTD430>();
	for (auto uniform : materialView->uniforms_)
	{
		std430->addUniform(uniform.first, uniform.second);
	}

	materialTemplates_.emplace(handle, std430);
	createMaterialBuffer(handle);

	return Material { std430, handle };
}

TIRID Renderer::instanciate(ID64 handle)
{
	auto mesh = app_->getMesh(handle);
	if (!mesh) return TIRID { .geometryID = InvalidID };

	auto geometry = app_->getGeometry      (mesh->geometry_);
	auto material = app_->getMaterialLayout(mesh->material_);
	if(!geometry || !material) return TIRID { .geometryID = InvalidID };
	
	createMaterialBuffer(material.handle());
	geometryBuffer_.registerGeometry(geometry.handle(), geometry.get());
	
	TIRID object;
	object.geometryID    = geometry.handle();
	object.materialID    = material.handle();
	object.indexCount    = geometryBuffer_.getIndexCount (geometry.handle());
	object.firstIndex    = geometryBuffer_.getFirstIndex (geometry.handle());
	object.baseVertex    = geometryBuffer_.getFirstVertex(geometry.handle());
	object.matrixIndex   = matrixIndexBuffer_  .get();
	object.materialIndex = materialIndexBuffer_.get(material.handle());

	return object;
}

TIRID Renderer::instanciate(ID64 handle, glm::mat4 model, Material material)
{
	auto object = instanciate(handle);

	matrixSSBO_.upload(model, object.matrixIndex * sizeof(glm::mat4));
	materialSSBO_
		.at(object.materialID)
		.upload(material.raw(), object.materialIndex * material.size());

	return object;
}

void Renderer::updateInstance(TIRID instance, glm::mat4 model, Material material)
{
	if (instance.geometryID == InvalidID)
		return;

	matrixSSBO_.upload(model, instance.matrixIndex * sizeof(glm::mat4));
	materialSSBO_[instance.materialID].upload(material.raw(), instance.materialIndex * material.size());
}

void Renderer::updateInstance(TIRID instance, glm::mat4 model)
{
	if (instance.geometryID == InvalidID) return;

	matrixSSBO_.upload(model, instance.matrixIndex * sizeof(glm::mat4));
}

void Renderer::updateInstance(TIRID instance, Material material)
{
	if (instance.geometryID == InvalidID) return;

	materialSSBO_[instance.materialID].upload(material.raw(), instance.materialIndex * material.size());
}

void Renderer::drawInstance(TIRID instance)
{
	if (instance.geometryID == InvalidID) return;
	if (!instance.visible_) return;
	
	renderList_[instance.materialID].emplace_back(instance);
}

void Renderer::killInstance(TIRID & instance)
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

void Renderer::render()
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