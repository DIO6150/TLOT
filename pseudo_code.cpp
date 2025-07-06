
#include <memory>
#include <unordered_map>
#include <vector>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/matrix.hpp"
#include "vendor/glm/gtx/transform.hpp"
#include "vendor/glad/include/glad/glad.h"
#include "src/include/utils.hpp"

using namespace std;
using namespace glm;

#define MAX_INSTANCE_COUNT 65536

struct DrawCommandIndirect {
    unsigned int index_count;
    unsigned int instance_count;
    unsigned int base_index;
    int          base_vertex;
    unsigned int base_instance;
};

struct InstanceData {
    mat4        matrix;
    vec3        color;
    vec4        atlasCoords; // x, y, w, h -> texCoords = (w, h) * texCoords + (x, y)
};

class Vertex {
    vec3 position;
    vec2 uv;
    vec3 normals;
};

class Mesh {
    private:
    vector<Vertex>   vertices;
    vector<uint32_t> indices;

    friend GameObject;
    friend Batch;
    friend Scene;
};

class Scene;

class GameObject {
    private:
    bool should_render;
    
    Mesh *mesh;
    InstanceData data;

    Scene *m_parent;

    friend Scene;

    GameObject (Mesh *mesh, InstanceData data, Scene *parent) :
        should_render (true),
        mesh (mesh), 
        data (data),
        m_parent (parent)
    {
        
    }

    public:
    void Translate (glm::vec3 translation) {
        data.matrix = glm::translate (data.matrix, translation);
        m_parent->should_resend_ssbo = true;
    }
};

struct LayoutElement {
    unsigned int elem_type;
    int elem_count;
};

class Layout {
    private:
    int m_size;
    int m_stride;
    std::vector<LayoutElement> m_elements;

    friend Batch;

    public:
    Layout (std::vector<LayoutElement> elements) {
        int stride = 0;
        for (auto& e : elements) {
            stride += e.elem_count * getGLTypeSize (e.elem_type);
        }
    }

    private:
    void Apply () {
        int i = 0;
        intptr_t offset = 0;
        for (auto& e : m_elements) {
            glVertexAttribPointer (i, e.elem_count, e.elem_type, GL_FALSE, m_stride, (void *) offset);
            glEnableVertexAttribArray (i++);

            offset += e.elem_count * getGLTypeSize (e.elem_type);
        }
    }

    void Apply ();
};

class Batch {
    std::vector<Mesh *> meshes;

    uint32_t m_vao;
    uint32_t m_vbo;
    uint32_t m_ebo;
    uint32_t m_dibo;
    uint32_t m_instance_ssbo;
    
    uint32_t m_diffuseMap;
    uint32_t m_ambientMap;
    uint32_t m_normalsMap;
    uint32_t m_specularMap;
    uint32_t m_albedoMap;

    // Shader m_shader;
    Layout *m_layout;

    public:
    Batch (Layout *&&layout, vector<Mesh *> &meshes) {
        m_layout = std::move (layout);
        layout = nullptr;
        
        glGenVertexArrays(1, &m_vao);
        
        glGenBuffers (1, &m_vbo);
        glGenBuffers (1, &m_ebo);
        glGenBuffers (1, &m_dibo);
        
        glGenBuffers (1, &m_instance_ssbo);

        size_t vbo_size;
        size_t ebo_size;
        size_t dibo_size;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for (auto &mesh : meshes) {
            vertices.insert (vertices.end (), mesh->vertices.begin (), mesh->vertices.end ());
            indices .insert (indices .end (), mesh->indices .begin (), mesh->indices .end ());
        }

        vbo_size    = vertices.size ();
        ebo_size    = indices .size ();
        dibo_size   = meshes  .size () * sizeof (DrawCommandIndirect);
        
        Bind ();
        
        glNamedBufferData (m_vbo,   vbo_size,  vertices.data (), GL_STATIC_DRAW);
        glNamedBufferData (m_ebo,   ebo_size,  indices .data (), GL_STATIC_DRAW);
        glNamedBufferData (m_dibo,  dibo_size, NULL,             GL_DYNAMIC_DRAW);
        
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, m_instance_ssbo);
        glNamedBufferData (m_instance_ssbo, sizeof (InstanceData) * MAX_INSTANCE_COUNT, NULL, GL_DYNAMIC_DRAW);


        m_layout->Apply ();

        Unbind ();
    }

    void Bind () {
        glBindVertexArray (m_vao);
        glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_dibo);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
    }

    void Unbind () {
        glBindVertexArray (0);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
        glBindTexture (GL_TEXTURE_2D, 0);
    }

    const uint32_t GetDiffuse () const {
        return (m_diffuseMap);
    }

    const uint32_t GetAmbient () const {
        return (m_ambientMap);
    }
};

class Scene {
    vector<Mesh> loaded_mesh;
    vector<GameObject> loaded_objects;
    unordered_map<Mesh *, uint32_t> instance_counter;
    uint32_t visible_mesh_count;
    vector<Batch> batches;
    bool should_resend_ssbo;

    friend GameObject;
    
    public:
    Scene () : visible_mesh_count (0), should_resend_ssbo (true) {

    }

    GameObject &CreateObject (Mesh *mesh, InstanceData data) {
        if (instance_counter.find (mesh) == instance_counter.end ()) {
            instance_counter.insert ({mesh, 1});
            ++visible_mesh_count;
        }

        loaded_objects.push_back (GameObject {mesh, data, this});

        return (loaded_objects.back ());
    }

    private:
    void SetInvisible (GameObject &obj) {
        obj.should_render = false;
        --instance_counter[obj.mesh];
        --visible_mesh_count;
        should_resend_ssbo = true;
    }
    
    void SetVisible (GameObject &obj) {
        obj.should_render = true;
        ++instance_counter[obj.mesh];
        ++visible_mesh_count;
        should_resend_ssbo = true;
    }

    void UploadDrawCommands () const {
        size_t  index_offset;
        size_t  vertex_offset;
        size_t  instance_offset;
        int64_t draw_commands_offset;

        DrawCommandIndirect command;

        vertex_offset   = 0;
        index_offset    = 0;
        instance_offset = 0;

        for (const auto &[mesh, instance_count] : instance_counter) {
            command.index_count     = mesh->indices.size ();
            command.instance_count  = instance_count;
            command.base_index      = index_offset;
            command.base_vertex     = vertex_offset;
            command.base_instance   = instance_offset;

            index_offset    += mesh->indices .size ();
            vertex_offset   += mesh->vertices.size ();
            instance_offset += instance_count;

            glBufferSubData (
                GL_DRAW_INDIRECT_BUFFER, 
                draw_commands_offset,
                sizeof (DrawCommandIndirect),
                (void *) &command
            );

            draw_commands_offset += sizeof (DrawCommandIndirect);
        }
    }

    void UploadInstanceDataSSBO ()
    {
        size_t offset;
        offset = 0;        
        
        for (auto obj : loaded_objects) {
            if (!obj.should_render) continue;
            
            glBufferSubData (GL_SHADER_STORAGE_BUFFER, offset, sizeof (InstanceData), (void*) &obj.data);
            offset += sizeof (InstanceData);
        }
    }
    
    public:
    void Render () {
        if (true) { // TODO : add frame limitations (some form of async image generation ? to not bottleneck the app)
            for (auto &batch : batches) {
                batch.Bind ();

                UploadDrawCommands ();
                if (should_resend_ssbo)
                    UploadInstanceDataSSBO ();

                glActiveTexture (GL_TEXTURE0);
                glBindTexture (GL_TEXTURE_2D_ARRAY, batch.GetDiffuse ());

                glActiveTexture (GL_TEXTURE1);
                glBindTexture (GL_TEXTURE_2D_ARRAY, batch.GetAmbient ());

                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, visible_mesh_count, 0);

                batch.Unbind ();
            }
        }

        should_resend_ssbo = false;
    }
};
