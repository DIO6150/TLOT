#include <scene.hpp>

#include <details/batch.hpp>
#include <details/draw_command.hpp>
#include <game_object.hpp>
#include <mesh.hpp>
#include <layout.hpp>

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

Engine::Scene::Scene () {
	m_batches.reserve (8);
}

EngineDetail::Batch *Engine::Scene::CreateBatch (Shader* shader_base) {
	m_batches.emplace_back (new Layout { {{GL_FLOAT, 3}, {GL_FLOAT, 2}, {GL_FLOAT, 3}} }, shader_base);

	return (&m_batches.back ());
}

Engine::GameObject *Engine::Scene::CreateObject (Mesh *mesh, EngineDetail::InstanceData data, Shader *shader) {
	EngineDetail::Batch *batch;
	GameObject *object;

	batch = GetBatch (shader);

	if (!batch) {
		batch = CreateBatch (shader);
	}

	m_loaded_objects.push_back (GameObject {mesh, data, this, batch});
	object = &m_loaded_objects.back ();	

	batch->UploadGameObject (object);

	return (object);
}

EngineDetail::Batch *Engine::Scene::GetBatch (Shader *shader) {
	auto pos = std::find_if (m_batches.begin (), m_batches.end (), [shader](EngineDetail::Batch &batch) -> bool {
		return (batch.GetShader () == shader);
	});

	if (pos == m_batches.end ()) {
		// TODO: error logging
		return (nullptr);
	}

	return (&(*pos));
}

void Engine::Scene::Render ()  {
	// TODO : add frame limitations (some form of async image generation ? to not bottleneck the app)
	for (auto &batch : m_batches) {
		batch.Bind ();
		
		batch.m_shader->Use ();

		batch.UploadDrawCommands ();
		if (batch.m_should_resend_ssbo) { batch.UploadInstanceDataSSBO (); }
		
		glMultiDrawElementsIndirect (
			GL_TRIANGLES,
			GL_UNSIGNED_INT,
			nullptr,
			batch.m_visible_mesh_count,
			0
		);
		
		batch.Unbind ();

		batch.m_should_resend_ssbo = false;
	}
}