#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void renderWindow(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void set_light_color(Shader shader);

glm::vec3 attenuations[] = {
    glm::vec3(1.0f, 0.14f, 0.07f),    // 32
    glm::vec3(1.0f, 0.09f, 0.032f),   // 50
    glm::vec3(1.0f, 0.07f, 0.017f),   // 65
    glm::vec3(1.0f, 0.045f, 0.0075f)  // 100
};

float cube_vertices[] = {
    // position         
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
                        
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
                        
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
                        
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
                        
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
                        
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

constexpr float DISPLAY_WIDTH = 1200.0f;
constexpr float DISPLAY_HEIGHT = 900.0f;

float delta_time = 0.0f;  // Time between current frame and last frame
float last_frame = 0.0f;  // Time of last frame

float ambient_mult = 0.1;
float diffuse_mult = 0.5;
float specular_mult = 1.0;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 light_pos(0.0f, 2.0f, 3.0f);
bool firstMouse = true;
float last_x = DISPLAY_WIDTH / 2.0f;
float last_y = DISPLAY_HEIGHT / 2.0f;

int main() {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  // for macOS
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                        "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  Shader shader("./model.vert", "./model.frag");
  Shader light_shader("./model.vert", "./light.frag");

  // load models
  //Model my_model("./nanosuit/nanosuit.obj");
  //Model my_model("./m01_model/fbx/car.fbx");
  //Model my_model("./moto/motobike.obj");
  //Model my_model("./crysis-nano-suit-2/scene.fbx");
  //Model my_model("./dae/Handgun_dae.dae");
  //Model my_model("./Audi_R8/Models/Audi_R8.dae");
  Model my_model("./car/car1.obj");
  camera.Position = glm::vec3(0.0, 30.0, 60.0);
  camera.Pitch = -20.0f;
  camera.updateCameraVectors();


  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
               GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;

    processInput(window);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setVec3("camPos", camera.Position);

    set_light_color(shader);

    my_model.Draw(shader);

    glm::mat4 trans = glm::mat4(0.3f);
    glm::vec3 light_color = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, light_pos);
    model = glm::scale(model, glm::vec3(0.1f));
    light_shader.use();
    light_shader.setMat4("transform", trans);
    light_shader.setMat4("model", model);
    light_shader.setMat4("view", view);
    light_shader.setMat4("projection", projection);
    light_shader.setVec3("lightColor", light_color);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void set_light_color(Shader shader) {
  glm::vec3 light_color = glm::vec3(1.0f);
  glm::vec3 diffuse_dolor = light_color * diffuse_mult;
  glm::vec3 ambient_color = light_color * ambient_mult;
  glm::vec3 specular_color = light_color * specular_mult;
  // directional light
  shader.setVec3("dirLight.direction", 0.2f, 1.0f, 0.3f);
  shader.setVec3("dirLight.ambient", ambient_color);
  shader.setVec3("dirLight.diffuse", diffuse_dolor);
  shader.setVec3("dirLight.specular", specular_color);

  // point lights
  string lightName = "pointLight";
  shader.setVec3(lightName + ".position", light_pos);
  shader.setFloat(lightName + ".constant", attenuations[2].x);
  shader.setFloat(lightName + ".linear", attenuations[2].y);
  shader.setFloat(lightName + ".quadratic", attenuations[2].z);
  shader.setVec3(lightName + ".ambient", ambient_color);
  shader.setVec3(lightName + ".diffuse", diffuse_dolor);
  shader.setVec3(lightName + ".specular", specular_color);

  // flash light
  shader.setVec3("spotLight.position", camera.Position);
  shader.setVec3("spotLight.direction", camera.Front);
  shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(8.5f)));
  shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));
  shader.setVec3("spotLight.ambient", ambient_color);
  shader.setVec3("spotLight.diffuse", diffuse_dolor);
  shader.setVec3("spotLight.specular", specular_color);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  cout << "framebuffer_size_callback" << endl;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  float speed = delta_time * 2;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, speed);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, speed);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, speed);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, speed);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, speed);
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, speed);

  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) light_pos.z -= speed;
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) light_pos.z += speed;
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) light_pos.x -= speed;
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) light_pos.x += speed;
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) light_pos.y += speed;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) light_pos.y -= speed;

  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) ambient_mult += delta_time;
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) ambient_mult -= delta_time;
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) diffuse_mult += delta_time;
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) diffuse_mult -= delta_time;
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) specular_mult += delta_time;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) specular_mult -= delta_time;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    last_x = xpos;
    last_y = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - last_x;
  float yoffset = last_y - ypos;
  last_x = xpos;
  last_y = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(yoffset);
}
