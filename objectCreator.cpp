
#include "objectCreator.h"
#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <loader/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Quad::Quad()
    : _position{glm::vec2(0.0f)} , _velocity{glm::vec2(0.0f)}
{
    float vertices[] = {
        // positions      // texture coords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,   // top left
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // top right

         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f    // bottom left
    };

    // Bookmark
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO memory
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Memory layout

    // Position
    glVertexAttribPointer(
        0,                  // Attribute position
        3,                  // Vector Elements (1,2,3,4)
        GL_FLOAT,           // Vector Type
        GL_FALSE,           // Nomarlize
        5 * sizeof(float),  // Stride
        (void*)0);          // Offset
    glEnableVertexAttribArray(0);

    // Texture
    glVertexAttribPointer(
        1,                            // Attribute position
        2,                            // Vector Elements (1,2,3,4)
        GL_FLOAT,                     // Vector Type
        GL_FALSE,                     // Nomarlize
        5 * sizeof(float),            // Stride
        (void*)(3 * sizeof(float)));  // Offset
    glEnableVertexAttribArray(1);

    // Disable VAO
    glBindVertexArray(0);
}
    
void Quad::draw()
{   
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Quad::setPosition(glm::vec2 pos){_position = pos;}
void Quad::setVelocity(glm::vec2 vel){_velocity = vel;}
glm::vec2 Quad::getPosition(){return _position;}
glm::vec2 Quad::getVelocity(){return _velocity;}

// =====================================================================================

Model::Model(const std::string path) 
    : _model_path(path)
{
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _model_path.c_str())) 
        {
            printf("Error loading model mesh. Failed to initialize model!\n");
        }
        else
        {
            for (const auto& shape : shapes) 
            {
                for (const auto& index : shape.mesh.indices) 
                {
                    shapes::vertex vertex{};
                    vertex.x = attrib.vertices[3 * index.vertex_index + 0];
                    vertex.y = attrib.vertices[3 * index.vertex_index + 1];
                    vertex.z = attrib.vertices[3 * index.vertex_index + 2];

                    // to do : add normals and try phong shading

                    vertex.u = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * index.texcoord_index + 1];
                
                    _model_vertices.push_back(vertex);
                }
            }

            // Bookmark
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // VBO memory
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            size_t array_size = sizeof(_model_vertices[0]) * _model_vertices.size();
            glBufferData(GL_ARRAY_BUFFER, array_size, _model_vertices.data(), GL_STATIC_DRAW);

            // Position
            glVertexAttribPointer(
                0,                  // Attribute position
                3,                  // Vector Elements (1,2,3,4)
                GL_FLOAT,           // Vector Type
                GL_FALSE,           // Nomarlize
                5 * sizeof(float),  // Stride
                (void*)0);          // Offset
            glEnableVertexAttribArray(0);

            // Textures
            glVertexAttribPointer(
                1,                            // Attribute position
                2,                            // Vector Elements (1,2,3,4)
                GL_FLOAT,                     // Vector Type
                GL_FALSE,                     // Nomarlize
                5 * sizeof(float),            // Stride
                (void*)(3 * sizeof(float)));  // Offset
            glEnableVertexAttribArray(1);

            // Disable VAO
            glBindVertexArray(0);
        }
}

void Model::draw()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, _model_vertices.size());
    glBindVertexArray(0);
}

/* To do
- make macro for vertex data layout since it is pretty much the same for most objects:
- pos, tex, normal ...

*/