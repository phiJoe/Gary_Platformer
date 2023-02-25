
#include "objectCreator.h"
#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <loader/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "textureUtil.h"

movement_state_t input_transitions[NUM_INPUTS][8] = {

    // Left press 0
    {
        WALK_L,  // STAND
        WALK_L,  // WALK_L
        WALK_L,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Left release 1
    {
        STAND,   // STAND
        STAND,   // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Right press 2
    {
        WALK_R,  // STAND
        WALK_R,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Right release 3
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        STAND,   // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Up press 4
    {
        JUMP_UP, // STAND
        JUMP_L,  // WALK_L
        JUMP_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Up release 5
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Down press 6
    {
        DUCK,    // STAND
        DUCK,    // WALK_L
        DUCK,    // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        DUCK     // DUCK
    },

    // Down release 7
    {
        STAND,   // STAND
        WALK_L,  // WALK_L
        WALK_R,  // WALK_R
        JUMP_UP, // JUMP_UP
        FALL,    // FALL
        JUMP_L,  // JUMP_L,
        JUMP_R,  // JUMP_R
        STAND    // DUCK
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

// todo: configure component-choice (pos, tex, normals, bi-normals etc)
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

void Actor::setScale(glm::vec3 scale)
{
    _scale_mat = glm::scale(glm::mat4(1.0f), scale);
}

void Actor::setAcceleration(glm::vec3 acceleration)
{
    _acceleration = acceleration;
}

void Actor::setName(std::string name)
{
    _name = name;
}

/* === Character class definitions === */

Character::Character()
    : _curr_move_state(STAND), _prev_move_state(STAND), _walk_phase_index(0),
      _frame_timer(0.0f), _current_walk_button_action(OFF),
      _walk_L_velocity(glm::vec3(-0.2f, 0.0f, 0.0f)), _walk_R_velocity(glm::vec3(0.2f, 0.0f, 0.0f)), _jump_velocity(glm::vec3(0.0f, 2.5f, 0.0f))
{
    Actor::setName("Character_Actor");
    Actor::setAcceleration(glm::vec3(0.0f, -9.81f / 2, 0.0f));
    Actor::setScale(glm::vec3(0.05f, 0.05f, 0.05f));

    std::cout << "Created: " << _name << "\n";
}

void Character::updateMovementState(button_action_t button_action, double dt)
{

    // button_action_t current_walk_button_action;

    if (_current_walk_button_action != button_action)
    {
        switch (button_action)
        {
        case LEFTP:
            _current_walk_button_action = LEFTP;
            break;
        case LEFTR:
            _current_walk_button_action = LEFTR;
            break;
        case RIGHTP:
            _current_walk_button_action = RIGHTP;
            break;
        case RIGHTR:
            _current_walk_button_action = RIGHTR;
            break;
        }
    }

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
            _vel = _vel + glm::vec3(_acceleration.x * dt, _acceleration.y * dt, _acceleration.z * dt);

            if (_vel.y < 0.0f)
            {
                _curr_move_state = FALL;
            }
            break;
        case FALL:
            _pos = _pos + glm::vec3(_vel.x * dt, _vel.y * dt, _vel.z * dt);
            _vel = _vel + glm::vec3(_acceleration.x * dt, _acceleration.y * dt, _acceleration.z * dt);

            if (_pos.y < 0.0f) // initial position
            {
                _pos.y = 0.0f;

                if (_current_walk_button_action == LEFTP)
                {
                    _curr_move_state = WALK_L;
                    _vel = _walk_L_velocity;
                }
                else if (_current_walk_button_action == RIGHTP)
                {
                    _curr_move_state = WALK_R;
                    _vel = _walk_R_velocity;
                }
                else
                {
                    _curr_move_state = STAND;
                    _vel = glm::vec3(0.0f, 0.0f, 0.0f);
                }
            }
            break;
        case DUCK:
            _vel = glm::vec3(0.0f, 0.0f, 0.0f);
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
            _frame_timer = 0.0f;
            _walk_phase_index = 0;
            _vel = _walk_L_velocity;
            break;
        case WALK_R:
            _frame_timer = 0.0f;
            _walk_phase_index = 0;
            _vel = _walk_R_velocity;
            break;
        case JUMP_UP:
            _vel = _jump_velocity;
            break;
        case JUMP_L:
            _vel = _jump_velocity + _walk_L_velocity;
            break;
        case JUMP_R:
            _vel = _jump_velocity + _walk_R_velocity;
            break;
        case DUCK:
            _vel = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
}

// note: use STAND sprite for jump/fall for now
void Character::draw(Shader &shader, const std::vector<Texture2D *> &walk_textures,
                     const Texture2D &jump_texture,
                     const Texture2D &fall_texture,
                     const Texture2D &duck_texture)
{
    shader.activate();

    // set transforms based on integrated position and the quad scale
    glm::mat4 model_m = glm::translate(glm::mat4(1.0f), _pos) * _scale_mat;
    shader.setMatrix("model", glm::value_ptr(model_m));

    activateAnimationTexture(shader, walk_textures, jump_texture, fall_texture, duck_texture);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Debug
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, _model_vertices.size());
    glBindVertexArray(0);
}

void Character::activateAnimationTexture(Shader &shader, const std::vector<Texture2D *> &walk_textures,
                                         const Texture2D &jump_texture, const Texture2D &fall_texture,
                                         const Texture2D &duck_texture)
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
    case DUCK:
        glBindTexture(GL_TEXTURE_2D, duck_texture.getTextureID());
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

// set last button state for every object in queue
void set_button_action()
{
    // set current button state
}

// per frame
// only one button state can get through per callback
void movement_state_resolver(button_action_t input_button, int action)
{
    // old_state = current_state
    // current_state = input_button

    /*
    button states:

    int up_state
    int down_state
    int net_state_vertical
    int old_net_state_vertical;

    int r_state
    int l_state
    int net_state_horizontal
    int old_net_state_horizontal

    bool space_pulse
    int space_state
    int old_space_state

    switch(input)
        case up:
            if(press)
                up_state = 1;
            if(released)
                up_state = 0;
        case down:
            if(press)
                down_state = -1;
            if(released)
                down_state = 0;
        case R:
            if(press)
                r_state = 1;
            if(released)
                r_state = 0;
        case L:
            if(press)
                l_state = -1;
            if(released)
                l_state = 0;
        case space:
            if(press)
                space_state = 1;
            if(released)
                space_state = 0;

        // Duck
        net_state_vertical = up_state + down_state;

        // Walk
        net_state_horizontal = l_state + r_state;

        // Jump
        if(space_state != old_space_state && space_state == 1)
            space_pulse = true
        else
            space_pulse = false
        old_space_state = space_state // save for next frame

        // State resolving with input
        switch(current_movement_state)
        {
            case: STAND
                if(space_pulse == true)
                    // jump_up
                if(net_vertical_state != 0)
                    if(net_vertical_state == 1)
                        // nothing
                    else
                        // duck
                else
                    // stand
                if(net_horizontal_state != 0)
                    if(net_horizontal_state == 1)
                        // walk_r
                    else
                        // walk_l
                else
                    // stand
            case: JUMP
                // nothing, no control
            case: DUCK
                if(net_vertical_state == 0)
                    // stand
            case: WALK_
            case: WALK_R
        }
    */
}
