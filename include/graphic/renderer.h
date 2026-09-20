#ifndef _INCLUDE_GRAPHIC_RENDERER_H
#define _INCLUDE_GRAPHIC_RENDERER_H

#include "primitives/triangle.hpp"
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"

class Renderer {
    // Initial sizes of window
    const unsigned int SCR_WIDTH_ = 1200;
    const unsigned int SCR_HEIGHT_ = 1000;

    // Center of x,y
    float last_x_ = SCR_WIDTH_ / 2.0f;
    float last_y_ = SCR_HEIGHT_ / 2.0f;
    bool first_mouse_ = true;

    // timing between frames
    float delta_time_ = 0.0f;
    float last_frame_ = 0.0f;

    // IDs of data buffers
    unsigned int VBO_ = 0, VAO_ = 0;

    // Window
    GLFWwindow *window_ = nullptr;

    // camera
    Camera camera_;
    // lighting
    glm::vec3 light_pos_;
    // shaders
    Shader lighting_shader_;

    // Data
    std::vector<float> vert_tri_blue;
    std::vector<float> vert_tri_red;

  public:
    Renderer(const char *vertexPath, const char *fragmentPath)
        : camera_(glm::vec3(0.0f, 0.0f, 3.0f)), light_pos_(1.2f, 1.0f, 2.0f) {
      // glfw: initialize and configure
      // ------------------------------
      glfwInit();
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
      // glfw window creation
      // --------------------
      window_ = glfwCreateWindow(SCR_WIDTH_, SCR_HEIGHT_, "Triangles3D", NULL, NULL);
      if (window_ == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
      }
      glfwMakeContextCurrent(window_);
      glfwSetWindowUserPointer(window_, reinterpret_cast<void *>(this));
      glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
      glfwSetCursorPosCallback(window_, mouseCallback);
      glfwSetScrollCallback(window_, scrollCallback);

      // tell GLFW to capture our mouse
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      // glad: load all OpenGL function pointers
      // ---------------------------------------
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
      }

      // configure global opengl state
      // -----------------------------
      glEnable(GL_DEPTH_TEST);

      // Configure shader
      lighting_shader_.loadByPath(vertexPath, fragmentPath);
    }

    template <typename T> void appendTriangle(std::vector<float> &out, const Triangle<T> &tri) {
      const auto &a = tri.a_;
      const auto &b = tri.b_;
      const auto &c = tri.c_;
      const auto &n = tri.plane_.n_;

      out.push_back(a.x_);
      out.push_back(a.y_);
      out.push_back(a.z_);
      out.push_back(n.x_);
      out.push_back(n.y_);
      out.push_back(n.z_);

      out.push_back(b.x_);
      out.push_back(b.y_);
      out.push_back(b.z_);
      out.push_back(n.x_);
      out.push_back(n.y_);
      out.push_back(n.z_);

      out.push_back(c.x_);
      out.push_back(c.y_);
      out.push_back(c.z_);
      out.push_back(n.x_);
      out.push_back(n.y_);
      out.push_back(n.z_);
    }

    template <typename T>
    void fillTrianglesBuffer(const std::vector<Triangle<T>> &triangles, const std::vector<size_t> &indexes) {
      auto it = indexes.begin();
      auto end = indexes.end();
      for (size_t i = 0; i < triangles.size(); i++) {
        auto &cur_tri = triangles[i];
        if (it != end && *it == i) {
          appendTriangle(vert_tri_red, cur_tri);
          ++it;
        } else {
          appendTriangle(vert_tri_blue, cur_tri);
        }
      }
      vert_tri_blue.insert(vert_tri_blue.end(), vert_tri_red.begin(), vert_tri_red.end());

      // first, configure the 's VAO (and VBO)
      glGenVertexArrays(1, &VAO_);
      glGenBuffers(1, &VBO_);

      glBindBuffer(GL_ARRAY_BUFFER, VBO_);
      glBufferData(GL_ARRAY_BUFFER, vert_tri_blue.size() * sizeof(float), vert_tri_blue.data(), GL_STATIC_DRAW);

      glBindVertexArray(VAO_);

      // position attribute
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
      glEnableVertexAttribArray(0);
      // normal attribute
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
    }

    void renderWindow() {
      // render loop
      while (!glfwWindowShouldClose(window_)) {
        // per-frame time logic
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time_ = current_frame - last_frame_;
        last_frame_ = current_frame;

        // input
        processInput(window_);

        // render
        glClearColor(0.882f, 0.824f, 0.733f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lighting_shader_.use();
        lighting_shader_.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        lighting_shader_.setVec3("viewPos", camera_.Position);

        glm::vec3 sunColor(1.0f, 0.96f, 0.90f);

        // light properties
        lighting_shader_.setVec3("light.ambient", sunColor * 1.0f);
        lighting_shader_.setVec3("light.diffuse", sunColor * 0.3f);
        lighting_shader_.setVec3("light.specular", glm::vec3(1.0f));

        // http://devernay.free.fr/cours/opengl/materials.html
        // material properties

        glm::vec3 baseColor(0.565f, 0.698f, 0.847f); // blue
        lighting_shader_.setVec3("material.ambient", baseColor * 1.0f);
        lighting_shader_.setVec3("material.diffuse", baseColor * 1.0f);
        lighting_shader_.setVec3("material.specular", 0.25f, 0.25f, 0.25f);
        lighting_shader_.setFloat("material.shininess", 32);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)SCR_WIDTH_ / (float)SCR_HEIGHT_, 0.1f, 100.0f);
        glm::mat4 view = camera_.GetViewMatrix();
        lighting_shader_.setMat4("projection", projection);
        lighting_shader_.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lighting_shader_.setMat4("model", model);

        // render the blue triangles
        glBindVertexArray(VAO_);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<size_t>((vert_tri_blue.size() - vert_tri_red.size()) / 6));

        baseColor = glm::vec3(0.690f, 0.439f, 0.302f); // red
        lighting_shader_.setVec3("material.ambient", baseColor * 1.0f);
        lighting_shader_.setVec3("material.diffuse", baseColor * 1.0f);
        lighting_shader_.setVec3("material.specular", 0.25f, 0.25f, 0.25f);
        lighting_shader_.setFloat("material.shininess", 32);

        // render the red triangles
        glDrawArrays(GL_TRIANGLES, static_cast<size_t>((vert_tri_blue.size() - vert_tri_red.size()) / 6),
                     static_cast<size_t>(vert_tri_red.size() / 6));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window_);
        glfwPollEvents();
      }
    }

  private:
    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window) {
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_.ProcessKeyboard(FORWARD, delta_time_);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_.ProcessKeyboard(BACKWARD, delta_time_);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_.ProcessKeyboard(LEFT, delta_time_);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_.ProcessKeyboard(RIGHT, delta_time_);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
      if (Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
        renderer->framebuffer_size_callback(width, height);
      }
    }

    void framebuffer_size_callback(int width, int height) {
      // make sure the viewport matches the new window dimensions; note that width and
      // height will be significantly larger than specified on retina displays.
      glViewport(0, 0, width, height);
    }

    static void mouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
      if (Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
        renderer->mouse_callback(xposIn, yposIn);
      }
    }

    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void mouse_callback(double xposIn, double yposIn) {
      float xpos = static_cast<float>(xposIn);
      float ypos = static_cast<float>(yposIn);
      if (first_mouse_) {
        last_x_ = xpos;
        last_y_ = ypos;
        first_mouse_ = false;
      }

      float xoffset = xpos - last_x_;
      float yoffset = last_y_ - ypos; // reversed since y-coordinates go from bottom to top

      last_x_ = xpos;
      last_y_ = ypos;

      camera_.ProcessMouseMovement(xoffset, yoffset);
    }

    static void scrollCallback(GLFWwindow *window, double xposIn, double yposIn) {
      (void)xposIn;

      if (Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
        renderer->scroll_callback(yposIn);
      }
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(double yoffset) { camera_.ProcessMouseScroll(static_cast<float>(yoffset)); }
};

#endif