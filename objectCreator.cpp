
#include "objectCreator.h"
#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <loader/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "textureUtil.h"

movement_state_t input_transitions[NUM_INPUTS][7] = {

    // Left press 0
    {
        WALK_L,  // STAND
        WALK_L,  // WALK_L
        WALK_L,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Left release 1
    {
        STAND,   // STAND
        STAND,   // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Right press 2
    {
        WALK_R,  // STAND
        WALK_R,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Right release 3
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        STAND,   // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Up press 4
    {
        JUMP_UP, // STAND
        JUMP_L,  // WALK_L
        JUMP_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Up release 5
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Down press 6
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    },

    // Down release 7
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R   // JUMP_R
    }};

Quad::Quad()
    : _position{glm::vec2(0.0f)}, _velocity{glm::vec2(0.0f)}
{
    float vertices[] = {
        // positions      // texture coords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right

        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f // bottom left
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
        0,                 // Attribute position
        3,                 // Vector Elements (1,2,3,4)
        GL_FLOAT,          // Vector Type
        GL_FALSE,          // Nomarlize
        5 * sizeof(float), // Stride
        (void *)0);        // Offset
    glEnableVertexAttribArray(0);

    // Texture
    glVertexAttribPointer(
        1,                            // Attribute position
        2,                            // Vector Elements (1,2,3,4)
        GL_FLOAT,                     // Vector Type
        GL_FALSE,                     // Nomarlize
        5 * sizeof(float),            // Stride
        (void *)(3 * sizeof(float))); // Offset
    glEnableVertexAttribArray(1);

    // Disable VAO
    glBindVertexArray(0);
}

void Quad::draw()
{
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Quad::setPosition(glm::vec2 pos)
{
    _position = pos;
}

void Quad::setVelocity(glm::vec2 vel)
{
    _velocity = vel;
}

glm::vec2 Quad::getPosition()
{
    return _position;
}

glm::vec2 Quad::getVelocity()
{
    return _velocity;
}

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
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
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
            0,                 // Attribute position
            3,                 // Vector Elements (1,2,3,4)
            GL_FLOAT,          // Vector Type
            GL_FALSE,          // Nomarlize
            5 * sizeof(float), // Stride
            (void *)0);        // Offset

        glEnableVertexAttribArray(0);

        // Textures
        glVertexAttribPointer(
            1,                            // Attribute position
            2,                            // Vector Elements (1,2,3,4)
            GL_FLOAT,                     // Vector Type
            GL_FALSE,                     // Nomarlize
            5 * sizeof(float),            // Stride
            (void *)(3 * sizeof(float))); // Offset
        glEnableVertexAttribArray(1);

        // Disable VAO
        glBindVertexArray(0);
    }
}

void Model::draw()
{
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, _model_vertices.size());
    glBindVertexArray(0);
}

/* To do
- make macro for vertex data layout since it is pretty much the same for most objects:
- pos, tex, normal ...

*/

/* === New definitions === */

// Mesh base class

Mesh::Mesh()
    : Mesh(QUAD)
{
}

Mesh::Mesh(mesh_primitive_t mp)
{
    switch (mp)
    {
    case QUAD:
        // todo: create debug output
        printf("Created Quad!\n");
        build_quad();
        set_vertex_attrib_config();
        break;
    case BOX:
        // todo
        break;
    case SPHERE:
        // todo
        break;
    case CAPSULE:
        // todo
        break;
    default:
        // print "invalid obj", create default quad
        break;
    }
}

Mesh::Mesh(const std::string path_to_obj)
{
    int err = load_model(path_to_obj);
    if (!err)
    {
        set_vertex_attrib_config();
    }
    else
    {
        build_quad();
        set_vertex_attrib_config();
        printf("Error loading model mesh - created unit quad instead!\n");
    }
}

void Mesh::build_quad()
{
    float vertices[] = {
        // positions        // texture coords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right

        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f // bottom left
    };

    int num_elements = sizeof(vertices) / (sizeof(shapes::vertex));
    shapes::vertex v{};
    for (int i = 0; i < num_elements; i++)
    {
        // points
        v.x = vertices[5 * i + 0];
        v.y = vertices[5 * i + 1];
        v.z = vertices[5 * i + 2];
        // texture
        v.u = vertices[5 * i + 3];
        v.v = vertices[5 * i + 4];

        _model_vertices.push_back(v);
    }
}

int Mesh::load_model(const std::string path_to_obj)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path_to_obj.c_str()))
    {
        printf("Error loading model mesh. Failed to initialize model!\n");
        return -1;
    }
    else
    {
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
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
        printf("Model mesh loaded!\n");
        return 0;
    }
}

void Mesh::set_vertex_attrib_config()
{
    // Bookmark
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // VBO memory
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    size_t array_size = sizeof(_model_vertices[0]) * _model_vertices.size();
    glBufferData(GL_ARRAY_BUFFER, array_size, _model_vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(
        0,                 // Attribute position
        3,                 // Vector Elements (1,2,3,4)
        GL_FLOAT,          // Vector Type
        GL_FALSE,          // Nomarlize
        5 * sizeof(float), // Stride
        (void *)0);        // Offset
    glEnableVertexAttribArray(0);

    // Textures
    glVertexAttribPointer(
        1,                            // Attribute position
        2,                            // Vector Elements (1,2,3,4)
        GL_FLOAT,                     // Vector Type
        GL_FALSE,                     // Nomarlize
        5 * sizeof(float),            // Stride
        (void *)(3 * sizeof(float))); // Offset
    glEnableVertexAttribArray(1);

    // Disable VAO
    glBindVertexArray(0);
}

/* === Actor class definitions === */

Actor::Actor()
    : _pos(0.0f), _vel(0.0f), Mesh::Mesh(QUAD)
{
}

/* === Character class definitions === */

Character::Character()
    : _curr_move_state(STAND), _prev_move_state(STAND), _walk_phase_index(0), _frame_timer(0.0f)
{
}

void Character::updateMovementState(button_action_t button_action, double dt)
{
    movement_state_t new_move_state = get_state_transition(button_action); // consult table

    // same state
    if (new_move_state == _curr_move_state)
    {
        // std::cout << " new_move_state == _curr_move_state \n";
        switch (_curr_move_state)
        {
        case STAND:
            _vel = glm::vec3(0.0f, 0.0f, 0.0f);
            break;
        case WALK_L:
        case WALK_R:
            _frame_timer += dt;
            if (_frame_timer > _walk_phase_interval)
            {
                _frame_timer = 0.0f;
                _walk_phase_index++;
                if (_walk_phase_index > _num_walk_phases)
                {
                    _walk_phase_index = 0;
                }
            }
            // Physics
            _pos = _pos + glm::vec3(_vel.x * dt, _vel.y * dt, _vel.z * dt);
            break;
        case JUMP_UP:
        case JUMP_L:
        case JUMP_R:
            _pos = _pos + glm::vec3(_vel.x * dt, _vel.y * dt, _vel.z * dt);
            _vel = _vel + glm::vec3(_gravity.x * dt, _gravity.y * dt, _gravity.z * dt);

            if (_vel.y < 0.0f)
            {
                _curr_move_state = FALL;
            }

            break;
        case FALL:
            _pos = _pos + glm::vec3(_vel.x * dt, _vel.y * dt, _vel.z * dt);
            _vel = _vel + glm::vec3(_gravity.x * dt, _gravity.y * dt, _gravity.z * dt);

            if (_pos.y < 0.0f) // initial position
            {
                _pos.y = 0.0f;
                _vel = glm::vec3(0.0f, 0.0f, 0.0f);
                _curr_move_state = STAND;
            }

            break;
        }
    }
    // new, initial state state
    else
    {
        _curr_move_state = new_move_state;
        switch (new_move_state)
        {
        case STAND:
            _vel = glm::vec3(0.0f, 0.0f, 0.0f);
            break;
        case WALK_L:
            _walk_phase_index = 0;
            _vel = glm::vec3(-0.2f, 0.0f, 0.0f);
            break;
        case WALK_R:
            _frame_timer = 0.0f;
            _walk_phase_index = 0;
            _vel = glm::vec3(0.2f, 0.0f, 0.0f);
            break;
        case JUMP_UP:
            _vel = glm::vec3(0.0f, 2.5f, 0.0f);
            break;
        // Not supported yet:
        case JUMP_L:
            _vel = glm::vec3(-0.2f, 2.5f, 0.0f);
            break;
        case JUMP_R:
            _vel = glm::vec3(0.2f, 2.5f, 0.0f);
            break;
        }
    }
}

// note: use STAND sprite for jump/fall for now
void Character::draw(Shader &shader, const std::vector<Texture2D *> &walk_textures, const Texture2D &jump_texture, const Texture2D &fall_texture)
{
    shader.activate();

    // set transforms based on integrated position and the quad scale
    glm::mat4 model_m = glm::translate(glm::mat4(1.0f), _pos) * _scale_mat;
    shader.setMatrix("model", glm::value_ptr(model_m));

    activateAnimationTexture(shader, walk_textures, jump_texture, fall_texture);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Debug
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, _model_vertices.size());
    glBindVertexArray(0);
}

void Character::activateAnimationTexture(Shader &shader, const std::vector<Texture2D *> &walk_textures, const Texture2D &jump_texture, const Texture2D &fall_texture)
{
    glActiveTexture(GL_TEXTURE0);
    switch (_curr_move_state)
    {
    case STAND:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[idle]).getTextureID());
        break;
    case WALK_L:
        setWalkPhaseTexture(left, walk_textures);
        shader.setBool("invert", false);
        break;
    case WALK_R:
        setWalkPhaseTexture(right, walk_textures);
        shader.setBool("invert", true);
        break;
    case JUMP_UP:
        glBindTexture(GL_TEXTURE_2D, jump_texture.getTextureID());
        break;
    case FALL:
        glBindTexture(GL_TEXTURE_2D, fall_texture.getTextureID());
        break;
    case JUMP_L:
        glBindTexture(GL_TEXTURE_2D, jump_texture.getTextureID());
        break;
    case JUMP_R:
        glBindTexture(GL_TEXTURE_2D, jump_texture.getTextureID());
        break;
    }
}

void Character::setWalkPhaseTexture(walk_dir_t dir, const std::vector<Texture2D *> &walk_textures)
{
    switch (_walk_sequence[dir][_walk_phase_index])
    {
    case idle:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[idle]).getTextureID());
        break;
    case left1:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[left1]).getTextureID());
        break;
    case left2:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[left2]).getTextureID());
        break;
    case right1:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[right1]).getTextureID());
        break;
    case right2:
        glBindTexture(GL_TEXTURE_2D, (*walk_textures[right2]).getTextureID());
        break;
    }
}

movement_state_t Character::get_state_transition(button_action_t button_action)
{
    return input_transitions[button_action][_curr_move_state];
}
