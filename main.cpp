#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #define TINYOBJLOADER_IMPLEMENTATION
// #include <loader/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

#include "shader.h"
#include "textureUtil.h"
#include "objectCreator.h"

// GLFW Util Functions
void frame_buffer_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void input_character_manager(int button, int action);
void poll_buttons(GLFWwindow *window);

int window_width = 900;
int window_height = 900;

button_action_t button_action_state = LEFTR;
button_action_t button_walk_state = LEFTR;

int main()
{
    // ----------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Platformer", NULL, NULL);

    if (!window)
    {
        std::cout << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }
    else
    {
        std::cout << "Created window!\n";
        glfwMakeContextCurrent(window);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load Opengl function pointers!\n";
        glfwTerminate();
        return -1;
    }
    else
    {
        std::cout << "Loaded Opengl functions!\n";
    }

    glViewport(0, 0, window_width, window_height);

    // -------------------------- Set callbacks for input --------------------------------

    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
    glfwSetKeyCallback(window, key_callback);

    // -----------------------------------------------------------------------------------
    // Basic geometries
    Quad background_quad;

    // -----------------------------------------------------------------------------------
    // Shader creations
    Shader quad_shader("_vertex.vs", "_fragment.fs");
    Shader background_shader("_vertex_background.vs", "_fragment_background.fs");

    // -----------------------------------------------------------------------------------
    // Basic textures
    Texture2D background("textures/background/grass_landscape.png");

    // ------------------------------ Walk Animation -------------------------------------
    Texture2D idle_sprite("assets_gary_walk_cycle/idle.png");
    Texture2D l1_sprite("assets_gary_walk_cycle/left1_.png");
    Texture2D l2_sprite("assets_gary_walk_cycle/left2_.png");
    Texture2D r1_sprite("assets_gary_walk_cycle/right1_.png");
    Texture2D r2_sprite("assets_gary_walk_cycle/right2_.png");
    Texture2D jump_sprite("assets_gary_moves/jump.png");
    // todo: fall

    // Walk
    std::vector<Texture2D *> walk_textures;
    walk_textures.push_back(&idle_sprite);
    walk_textures.push_back(&l1_sprite);
    walk_textures.push_back(&l2_sprite);
    walk_textures.push_back(&r1_sprite);
    walk_textures.push_back(&r2_sprite);

    // --------------------------------- Character Object --------------------------------
    Character character;

    // Background
    glm::mat4 base = glm::mat4(1.0f);
    // -----------------------------------------------------------------------------------

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    // ------------------------------- Loop Init -----------------------------------------
    double t1 = glfwGetTime();
    double t2;
    double dt;

    // glEnable(GL_DEPTH_TEST);

    // ---------------------------- Render/Game Loop -------------------------------------
    while (!glfwWindowShouldClose(window))
    {

        // delta time
        t2 = glfwGetTime();
        dt = t2 - t1;
        t1 = t2;

        // printf("dt: %f\n", dt);

        poll_buttons(window);

        /* ===  Clear screen === */
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        /* === Background === */
        background_shader.activate();
        background_shader.setMatrix("model", glm::value_ptr(base));

        // Set sampler unit
        background_shader.setInt("tex", 0);

        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background.getTextureID());

        background_quad.draw();

        /* === Character === */

        character.updateMovementState(button_action_state, dt);
        character.draw(quad_shader, walk_textures, jump_sprite, idle_sprite);

        /* === Displat all === */

        glfwSwapBuffers(window);
        glfwPollEvents();
        // glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}

// ------------------------------- End -------------------------------------

// polling
void poll_buttons(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// callbacks
void frame_buffer_callback(GLFWwindow *window, int width, int height)
{
    window_width = width;
    window_height = height;
    glViewport(0, 0, window_width, window_height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    input_character_manager(key, action);
}

void input_character_manager(int button, int action)
{
    switch (button)
    {
    case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS)
        {
            button_action_state = LEFTP;
            button_walk_state = LEFTP;
        }
        if (action == GLFW_RELEASE)
        {
            button_action_state = LEFTR;
            button_walk_state = LEFTR;
        }
        break;
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS)
        {
            button_action_state = RIGHTP;
            button_walk_state = RIGHTP;
        }
        if (action == GLFW_RELEASE)
        {
            button_action_state = RIGHTR;
            button_walk_state = RIGHTR;
        }
        break;
    case GLFW_KEY_UP:
        if (action == GLFW_PRESS)
        {
            button_action_state = UPP;
        }
        if (action == GLFW_RELEASE)
        {
            button_action_state = UPR;
        }
        break;
    case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS)
        {
            button_action_state = DOWNP;
        }
        if (action == GLFW_RELEASE)
        {
            button_action_state = DOWNR;
        }
        break;
    }

    // std::cout << "Button-Action: " << button_action_state << "\n";
}
