#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include <Transform.hpp>

#include <Core/Logger.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace TLOT
{
	class DebugRenderer
	{
		struct DebugVertex
		{
			float x, y;
		};

		struct DebugMesh
		{
			enum class Type
			{
				FilledRect = 0,
				Rect       = 1
			};

			size_t vertexCount;
			glm::mat4 matrix;
			glm::vec3 color;
			uint32_t vbo;

			Type type;
		};

	public:
		// --- LOGIQUE SINGLETON ---
        
        // Accès à l'instance unique
        static DebugRenderer& Get()
        {
            static DebugRenderer instance;
            return instance;
        }

        // On supprime la copie et l'assignation
        DebugRenderer(const DebugRenderer&) = delete;
        void operator=(const DebugRenderer&) = delete;

        // Initialisation (à appeler une fois après la création de la fenêtre OpenGL)
        void Init(float width, float height)
        {
            projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 100.0f);
            if (program == 0) {
                CreateDebugShader();
                glGenVertexArrays(1, &vao);
				glBindVertexArray (vao);

				rectVertices = {
					{-1.0f, -1.0f},
					{ 1.0f, -1.0f},
					{ 1.0f,  1.0f},
					{-1.0f, -1.0f},
					{ 1.0f,  1.0f},
					{-1.0f,  1.0f}
				};

				glGenBuffers (1, &rect_vbo);
				glBindBuffer (GL_ARRAY_BUFFER, rect_vbo);
				glBufferData (GL_ARRAY_BUFFER, rectVertices.size () * sizeof (DebugVertex), rectVertices.data (), GL_DYNAMIC_DRAW);
				
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

				glBindVertexArray (0);
			}
        }

		uint64_t CreateRect (glm::mat4 model, glm::vec3 color, bool filled=false)
		{
			auto id = nextMeshID++;

			DebugMesh::Type type = filled ? DebugMesh::Type::FilledRect : DebugMesh::Type::Rect;

			meshes[id] = DebugMesh {rectVertices.size (), model, color, rect_vbo, type};

			return id;
		};

		uint64_t CreateRect (glm::vec4 rect, glm::vec3 color, bool filled=false)
		{
			auto id = nextMeshID++;

			glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
            model = glm::scale(model, glm::vec3(rect.z / 2, rect.w / 2, 1.0f));
			model = glm::translate (model, glm::vec3 {1.0, 1.0, 0});

			DebugMesh::Type type = filled ? DebugMesh::Type::FilledRect : DebugMesh::Type::Rect;

			meshes[id] = DebugMesh {rectVertices.size (), model, color, rect_vbo, type};

			return id;
		};

		void DestroyItem (uint32_t id)
		{
			auto pos = meshes.find (id);

			if (pos == meshes.end ()) return;

			meshes.erase (id);
		}

		void Render ()
		{
			if (!active)
			{
				meshes.clear ();
				return;
			}

			glBindVertexArray (vao);

			for (auto & [meshID, mesh] : meshes)
			{
				glUseProgram (program);

				glUniformMatrix4fv (glGetUniformLocation (program, "model"), 1, GL_FALSE, &mesh.matrix[0][0]);
				glUniformMatrix4fv (glGetUniformLocation (program, "projection"), 1, GL_FALSE, &projection[0][0]);
				glUniform3fv (glGetUniformLocation (program, "color"), 1, &mesh.color[0]);
				glUniform1f (glGetUniformLocation (program, "type"), (float) mesh.type);

				glBindBuffer (GL_ARRAY_BUFFER, mesh.vbo);

				glDrawArrays (GL_TRIANGLES, 0, mesh.vertexCount);

				glBindBuffer (GL_ARRAY_BUFFER, 0);
			}

			meshes.clear ();

			glBindVertexArray(0);
		}

		void SetActive (bool flag)
		{
			active = flag;
		}

	private:
		DebugRenderer() : vao(0), program(0) {}

		void CreateDebugShader ()
		{
			int status;
			char infoLog[512];

			std::string vertex = \
			"#version 460 core\n" \
			"layout (location = 0) in vec2 aPos;\n" \
			"\n" \
			"uniform mat4 model;\n" \
			"uniform mat4 projection;\n" \
			"\n" \
			"out vec2 LocalPos; // Pour savoir où on est dans le rectangle\n" \
			"\n" \
			"void main() {\n" \
			"	LocalPos = aPos; \n" \
			"	gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n" \
			"}";

			const char* vertex_code = vertex.c_str ();
			unsigned int vertexShaderObject = glCreateShader (GL_VERTEX_SHADER);
			glShaderSource (vertexShaderObject, 1, &vertex_code, NULL);
			glCompileShader (vertexShaderObject);
			glGetShaderiv (vertexShaderObject, GL_COMPILE_STATUS, &status);
			if (!status) {
				glGetShaderInfoLog(vertexShaderObject, 512, NULL, infoLog);
				TLOT::Logger::log (TLOT::LogLevel::Error, "{}", infoLog);
				return;
			}


			std::string fragment = \
			"#version 460 core\n" \
			"out vec4 FragColor;\n" \
			"\n" \
			"in vec2 LocalPos;\n" \
			"\n" \
			"uniform vec3 color;\n" \
			"uniform float type; // 0 = Filled, 1 = Outline\n" \
			"\n" \
			"void main() {\n" \
			"	if (type > 0.5) { // Mode Rect (Outline)\n" \
			"		vec2 unitPerPixel = fwidth(LocalPos);\n" \
			"		vec2 thickness = unitPerPixel * 7.5;\n" \
			"		float edgeX = smoothstep(1.0 - thickness.x, 1.0, abs(LocalPos.x));\n" \
			"		float edgeY = smoothstep(1.0 - thickness.y, 1.0, abs(LocalPos.y));\n" \
			"		if (max(edgeX, edgeY) < 0.5) {\n" \
			"			discard;\n" \
			"		}\n" \
			"	}\n" \
			"	FragColor = vec4(color, 1.0);\n" \
			"}";

			const char* fragment_code = fragment.c_str ();
			unsigned int fragmentShaderObject = glCreateShader (GL_FRAGMENT_SHADER);
			glShaderSource (fragmentShaderObject, 1, &fragment_code, NULL);
			glCompileShader (fragmentShaderObject);
			glGetShaderiv (fragmentShaderObject, GL_COMPILE_STATUS, &status);
			if (!status) {
				glGetShaderInfoLog(fragmentShaderObject, 512, NULL, infoLog);
				TLOT::Logger::log (TLOT::LogLevel::Error, "{}", infoLog);
				return;
			}

			program = glCreateProgram ();

			glAttachShader (program, vertexShaderObject);
			glAttachShader (program, fragmentShaderObject);

			glLinkProgram (program);

			glGetProgramiv(program, GL_LINK_STATUS, &status);
			if(!status) {
				glGetProgramInfoLog(program, 512, NULL, infoLog);
				TLOT::Logger::log (TLOT::LogLevel::Error, "{}", infoLog);
				return;
			}
		}

		unsigned int vao;
		unsigned int rect_vbo;
		unsigned int program;

		std::vector<DebugVertex> rectVertices;

		glm::mat4 projection;

		std::map<uint64_t, DebugMesh> meshes;

		uint64_t nextMeshID = 0;

		bool active = false;
	};
}