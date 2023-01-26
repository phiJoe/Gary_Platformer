#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define TINYOBJLOADER_IMPLEMENTATION
#include <loader/tiny_obj_loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#include "shader.h"
#include "textureUtil.h"
#include "objectCreator.h"

// GLFW Util Functions
void frame_buffer_callback(GLFWwindow* window, int width, int height);
void poll_buttons(GLFWwindow* window);

int window_width = 900;
int window_height = 900;

enum WALK_PHASE {idle, left1, left2, right1, right2};

int main() {
    // ----------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Platformer", NULL, NULL);

    if (!window) {
        std::cout << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    } else {
        std::cout << "Created window!\n";
        glfwMakeContextCurrent(window);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load Opengl function pointers!\n";
        glfwTerminate();
        return -1;
    } else {
        std::cout << "Loaded Opengl functions!\n";
    }

    glViewport(0, 0, window_width, window_height);

    // ---------------------------------------- Set callbacks for input ----------------
    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);

    // ---------------------------------------------------------------------------------

    Quad background_quad;
    Quad character_quad;
    character_quad.setVelocity(glm::vec2(-0.05f, 0.0f));

    // 3D Model
    std::string model_path = "example_mesh/viking_model.obj";
    Model viking_room(model_path);

    // -----------------------------------------------------------------------------------
    // Shader creations
    Shader quad_shader("_vertex.vs", "_fragment.fs");
    Shader background_shader("_vertex_background.vs", "_fragment_background.fs");
    Shader model_shader("_vertex_model.vs", "_fragment_model.fs");

    // -----------------------------------------------------------------------------------
    // Textures
    Texture2D quad_texture("textures/gary.png");
    Texture2D background("textures/background/grass_landscape.png");
    Texture2D stone_tiles("textures/background/rock_tiles.png");
    Texture2D model_texture("textures/viking_room.png");

    // ------------------------------ Walk Animation -------------------------------------
    double walk_interval_slow = 0.1;
    double walk_timer = 0.0;
    int walk_counter = 0;
    bool invert = false;
    
    Texture2D idle_sprite("walk_cycle/idle.png");
    Texture2D l1_sprite("walk_cycle/left1_.png");
    Texture2D l2_sprite("walk_cycle/left2_.png");
    Texture2D r1_sprite("walk_cycle/right1_.png");
    Texture2D r2_sprite("walk_cycle/right2_.png");

    WALK_PHASE walk_sequence[2][8] = {{idle, left1, left2, left1, idle, right1, right2, right1},
                                      {idle, right1, right2, right1, idle, left1, left2, left1}};

    // ------------------------------------------------------------------------------------
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    // Quad Transform
    glm::mat4 base = glm::mat4(1.0f);
    glm::mat4 scale = glm::scale(base, glm::vec3(0.05f, 0.05f, 0.05f));

    // Character Transform
    glm::mat4 model_m;
   
    // Model transform
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 translate = glm::translate(base, glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotate_x = glm::rotate(base, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 rotate_y = glm::rotate(base, glm::radians(-140.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 model = translate * rotate_y * rotate_x;

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glm::vec2 velocity;
    glm::vec2 position;

    // ------------------------------- Loop Init -----------------------------------------
    double t1 = glfwGetTime();
    double t2;
    double dt;

    //glEnable(GL_DEPTH_TEST);

    // ---------------------------- Render/Game Loop -------------------------------------
    while (!glfwWindowShouldClose(window)) {

        // delta time
        t2 = glfwGetTime();
        dt = t2 - t1;
        t1 = t2;

        //printf("dt: %f\n", dt);

        poll_buttons(window);

        /* ===  Clear screen === */
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        /* === Background === */
        background_shader.activate();
        background_shader.setMatrix("model", glm::value_ptr(base));

        // Set sampler unit
        background_shader.setInt("tex", 0);

        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background.getTextureID());

        // Draw
        background_quad.draw();
        
        /* === Character === */

        // Physics update
        position = glm::vec2(character_quad.getPosition().x + character_quad.getVelocity().x * dt, 0);
        character_quad.setPosition(position);

        quad_shader.activate();

        model_m = glm::translate(base, glm::vec3(position.x, -0.6f, 0.0f)) * scale;
        quad_shader.setMatrix("model", glm::value_ptr(model_m));
        quad_shader.setBool("invert", invert);

        // Set sampler unit
        quad_shader.setInt("tex", 0);

        // Walk state
        if(walk_timer > walk_interval_slow)
        {   
            walk_timer = 0.0;
            walk_counter++;
            if(walk_counter > 7)
            {
                walk_counter = 0;
                invert = !invert;
                character_quad.setVelocity(glm::vec2(character_quad.getVelocity().x * -1.0, 0.0f));
            }
        }
        else
        {
            walk_timer += dt;
        }

        switch(walk_sequence[!invert][walk_counter])
        {   
            case idle:
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, idle_sprite.getTextureID());
                break;
            case left1:
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, l1_sprite.getTextureID());
                break;
            case left2:
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, l2_sprite.getTextureID());
                break;
            case right1:
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, r1_sprite.getTextureID());
                break;
            case right2:
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, r2_sprite.getTextureID());
                break;
        }
        // Draw quad
        character_quad.draw();

        /* === Model === */

        // model_shader.activate();
        // model_shader.setMatrix("model", glm::value_ptr(model));
        // model_shader.setMatrix("view", glm::value_ptr(view));
        // model_shader.setMatrix("projection", glm::value_ptr(projection));

        // // Set sampler unit
        // model_shader.setInt("tex", 0);

        // // Associate texture unit
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, model_texture.getTextureID());

        // // Draw model
        // viking_room.draw();

        /* === Displat all === */

        glfwSwapBuffers(window);
        glfwPollEvents();
        //glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}

void frame_buffer_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, window_width, window_height);
}

void poll_buttons(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

