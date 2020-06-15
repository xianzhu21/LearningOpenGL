#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Camera.h"
#include "constants.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <map>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void renderWindow(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

constexpr int NUM_POINT_LIGHTS = 1;

float mixOpacity = 0.1;
float radian = 0.0;

float ambient_mult = 0.1;
float diffuse_mult = 0.5;
float specular_mult = 1.0;

float speed = 0.0f;  // Time between current frame and last frame
float last_frame = 0.0f;  // Time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
bool firstMouse = true;
float last_x = DISPLAY_WIDTH / 2.0f;
float last_y = DISPLAY_HEIGHT / 2.0f;

float divider = DISPLAY_WIDTH / 2.0f;

int samples = 4;

int main() {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, samples);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // for macOS
#endif

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "LearnOpenGL", NULL, NULL);
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

  // glad: load all OpenGL function pointersd
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  renderWindow(window);

  glfwTerminate();

  return 0;
}

void renderWindow(GLFWwindow *window) {
  Shader objShader(vertPath, objectFragPath);
  Shader lightShader(vertPath, lightFragPath);
  Shader screenShader("./framebuffer.vert", "./framebuffer.frag");
  Shader cubemapShader("./cubemap.vert", "./cubemap.frag");
  //Shader geomShader("./geometry.vert", "./geometry.frag", "./geometry.geom");
  Shader normalShader("./vertex.vert", "./geometry.frag", "./geometry.geom");

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
  unsigned int uboMatrices;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &uboMatrices);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);

  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);

  // cube
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

  // texture
  unsigned int texture0 = createTexture("./container.jpg", GL_CLAMP_TO_BORDER);
  unsigned int texture1 = createTexture("./awesomeface.png", GL_REPEAT);
  unsigned int diffuseMap = createTexture("./container2.png", GL_REPEAT);
  unsigned int specularMap =
      createTexture("./container2_specular.png", GL_REPEAT);
  unsigned int coloredSpecularMap =
      createTexture("./lighting_maps_specular_color.png", GL_REPEAT);
  unsigned int emissionMap =
      createTexture("./container_emission.jpg", GL_REPEAT);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  // normal
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(5 * sizeof(float)));

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // instance VBO
  unsigned int instanceVBO;
  glGenBuffers(1, &instanceVBO);
  const unsigned int amount = 1000;
  glm::mat4 models[amount];
  srand(glfwGetTime());
  float radius = 5.0f;
  float offset = 0.8f;
  int index = 0;
  for (unsigned int i = 0; i < amount; ++i) {
    glm::mat4 model = glm::mat4(1.0f);
    float angle = (float)i / (float)amount * 360.0f;
    float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float y = displacement * 0.4f;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    model = glm::translate(model, glm::vec3(x, y, z));

    float scale = (rand() % 20) / 100.0f + 0.05f;
    model = glm::scale(model, glm::vec3(scale));

    float rotAngle = (rand() % 360);
    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

    models[i] = model;
  }
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * amount, &models[0],
               GL_STATIC_DRAW);
  size_t vec4Size = sizeof(glm::vec4);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * (vec4Size),
                        (void *)0);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * (vec4Size),
                        (void *)(1 * vec4Size));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * (vec4Size),
                        (void *)(2 * vec4Size));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * (vec4Size),
                        (void *)(3 * vec4Size));
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);
  glVertexAttribDivisor(7, 1);
  glVertexAttribDivisor(8, 1);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't
  // unbind VAOs (nor VBOs) when it's not directly necessary
  glBindVertexArray(0);

  // plane VAO and texture
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(5 * sizeof(float)));
  glBindVertexArray(0);
  unsigned int floorTexture = createTexture("./awesomeface.png", GL_REPEAT);

  // transparent VAO
  unsigned int transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // unsigned int grassTex = createTexture("./grass.png", GL_REPEAT);
  unsigned int grassTex =
      createTexture("./blending_transparent_window.png", GL_REPEAT);

  // screen VAO
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);

  // cubemap VAO
  unsigned int cubemapVAO, cubemapVBO;
  glGenVertexArrays(1, &cubemapVAO);
  glGenBuffers(1, &cubemapVBO);
  glBindVertexArray(cubemapVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindVertexArray(0);
  unsigned int cubemapTexture = loadCubemap(faces);

  // geometry VAO
  unsigned int geomVAO, geomVBO;
  glGenVertexArrays(1, &geomVAO);
  glGenBuffers(1, &geomVBO);
  glBindVertexArray(geomVAO);
  glBindBuffer(GL_ARRAY_BUFFER, geomVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);


  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

  double red = 0.1f;
  double green = 0.1f;
  double blue = 0.1f;

  screenShader.use();
  screenShader.setInt("screenTexture", 0);

  // framebuffer configuration
  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  // create a color attachment texture
  unsigned int textureColorbuffer;
  glGenTextures(1, &textureColorbuffer);
  //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0,
  //             GL_RGB, GL_UNSIGNED_BYTE, NULL);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glBindTexture(GL_TEXTURE_2D, 0);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
  //                       textureColorbuffer, 0);
  // multi-sample
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorbuffer);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, DISPLAY_WIDTH,
                          DISPLAY_HEIGHT, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D_MULTISAMPLE, textureColorbuffer, 0);

  // create a renderbuffer object
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, DISPLAY_WIDTH,
  //                      DISPLAY_HEIGHT);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                   GL_DEPTH24_STENCIL8, DISPLAY_WIDTH,
                                   DISPLAY_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // POST-PROCESSIONG FRAMEBUFFER
  unsigned int intermediateFBO;
  glGenFramebuffers(1, &intermediateFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
  unsigned int screenTexture;
  glGenTextures(1, &screenTexture);
  glBindTexture(GL_TEXTURE_2D, screenTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0,
               GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         screenTexture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!"
         << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    speed = currentFrame - last_frame;
    last_frame = currentFrame;

    processInput(window);

    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);
    // make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();

    // transformations
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(0.7, 0.7, 0.7));
    //trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
    //trans =|
    //    glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    //unsigned int transformLoc =
    //    glGetUniformLocation(shader.getID(), "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom),
                                  DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // render cubemap
    glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    cubemapShader.use();
    glm::mat4 cubemapView = glm::mat4(glm::mat3(view));
    cubemapShader.setMat4("view", cubemapView);
    cubemapShader.setMat4("projection", projection);
    glBindVertexArray(cubemapVAO);

    cubemapShader.setInt("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    //glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // render object
    objShader.use();
    objShader.setMat4("transform", trans);
    objShader.setMat4("model", model);

    objShader.setVec3("camPos", camera.Position);

    objShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    objShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    objShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    objShader.setFloat("material.shininess", 32.0f);
    
    glm::vec3 lightColor = glm::vec3(1.0f);
    //lightColor.x = sin(glfwGetTime() * 2.0f) + 0.2f;
    //lightColor.y = sin(glfwGetTime() * 0.7f) + 0.2f;
    //lightColor.z = sin(glfwGetTime() * 1.3f) + 0.2f;

    glm::vec3 diffuseColor = lightColor * diffuse_mult;
    glm::vec3 ambientColor = lightColor * ambient_mult;
    glm::vec3 specularColor = lightColor * specular_mult;

    // directional light
    objShader.setVec3("dirLight.direction", 0.2f, 1.0f, 0.3f);
    objShader.setVec3("dirLight.ambient", ambientColor);
    objShader.setVec3("dirLight.diffuse", diffuseColor);
    objShader.setVec3("dirLight.specular", specularColor);
    
    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
      string lightName = "pointLights[" + to_string(i) + "]";
      objShader.setVec3(lightName + ".position", pointLightPositions[i]);
      
      objShader.setFloat(lightName + ".constant", attenuations[i].x);
      objShader.setFloat(lightName + ".linear", attenuations[i].y);
      objShader.setFloat(lightName + ".quadratic", attenuations[i].z);

      objShader.setVec3(lightName + ".ambient", ambientColor);
      objShader.setVec3(lightName + ".diffuse", diffuseColor);
      objShader.setVec3(lightName + ".specular", specularColor);
    }

    // flash light
    objShader.setVec3("spotLight.position", camera.Position);
    objShader.setVec3("spotLight.direction", camera.Front);
    objShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(8.5f)));
    objShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));
    objShader.setVec3("spotLight.ambient", ambientColor);
    objShader.setVec3("spotLight.diffuse", diffuseColor);
    objShader.setVec3("spotLight.specular", specularColor);

    objShader.setInt("material.diffuse", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    objShader.setInt("material.specular", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    objShader.setInt("material.emission", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);

    //objShader.setInt("texGrass", 3);
    //glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_2D, grassTex);

    //objShader.setFloat("mixOpacity", mixOpacity);

    //objShader.setFloat("offsetX", sin(time) / 2.0f);
    //objShader.setFloat("offsetY", cos(time) / 2.0f);
    //objShader.setFloat("offsetZ", tan(time) / 2.0f);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture0);
    //objShader.setInt("texture0", 0);

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, texture1);
    // objShader.setInt("texture1", 1);

    objShader.setBool("useGeom", true);
    objShader.setFloat("time", time);

    // instance
    //for (unsigned int i = 0; i < 40; ++i) {
    //  objShader.setVec3(("offsets[" + std::to_string(i) + "]"), translations[i]);
    //}
    bool useInstance = false;
    objShader.setBool("useInstance", useInstance);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);  // default
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glBindVertexArray(VAO);
    glEnable(GL_MULTISAMPLE);
    if (useInstance) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);
    } else {
      for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++) {
        // render pass, draw objects as normal, writing to the stencil buffer
        glm::mat4 each_model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        each_model = glm::rotate(each_model, glm::radians(angle),
                                 glm::vec3(1.0f, 0.3f, 0.5f));
        objShader.setMat4("model", each_model);
        // objShader.setMat4("model", models[i]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
    glBindVertexArray(0);
    glDisable(GL_CULL_FACE);

    objShader.setBool("useGeom", false);
    objShader.setBool("useInstance", false);
    trans = glm::mat4(1.0);
    objShader.setMat4("transform", trans);

    // normal
    //normalShader.use();
    //glBindVertexArray(VAO);
    //for (unsigned int i = 0; i < 10; i++) {
    //  // render pass, draw objects as normal, writing to the stencil buffer
    //  glm::mat4 each_model = glm::translate(model, cubePositions[i]);
    //  float angle = 20.0f * i;
    //  each_model = glm::rotate(each_model, glm::radians(angle),
    //                           glm::vec3(1.0f, 0.3f, 0.5f));
    //  normalShader.setMat4("model", each_model);
    //  glDrawArrays(GL_TRIANGLES, 0, 36);
    //}

    // floor
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0x00);
    objShader.use();
    glBindVertexArray(planeVAO);
    objShader.setBool("floor", true);
    objShader.setInt("floorTex", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
    objShader.setMat4("model", model);
    objShader.setFloat("near", 1.0f);
    objShader.setFloat("far", 100.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    objShader.setBool("floor", false);
    glStencilMask(0xFF);

    // transparent grass
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0x00);
    objShader.use();
    glBindVertexArray(transparentVAO);
    objShader.setBool("grass", true);
    objShader.setInt("grassTex", 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, grassTex);
    objShader.setMat4("transform", trans);

    map<float, glm::vec3> sorted;
    for (int i = 0; i < vegetation.size(); ++i) {
      float distance = glm::length(camera.Position - vegetation[i]);
      sorted[distance] = vegetation[i];
    }
    for (map<float, glm::vec3>::reverse_iterator it = sorted.rbegin();
         it != sorted.rend(); ++it) {
      glm::mat4 each_model = glm::mat4(1.0f);
      each_model = glm::translate(each_model, it->second);
      objShader.setMat4("model", each_model);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
    objShader.setBool("grass", false);
    glStencilMask(0xFF);


    // border
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilMask(0x00);
    ////glDisable(GL_DEPTH_TEST);
    //lightShader.use();
    //float scale = 1.1;
    //glBindVertexArray(VAO);
    //lightShader.setMat4("transform", trans);
    //lightShader.setMat4("model", model);
    //lightShader.setMat4("view", view);
    //lightShader.setMat4("projection", projection);
    //lightShader.setVec3("lightColor", lightColor);
    //for (unsigned int i = 0; i < 10; i++) {
    //  glm::mat4 each_model = glm::translate(model, cubePositions[i]);
    //  float angle = 20.0f * i;
    //  each_model = glm::rotate(each_model, glm::radians(angle),
    //                                glm::vec3(1.0f, 0.3f, 0.5f));
    //  each_model = glm::scale(each_model, glm::vec3(scale, scale, scale));
    //  lightShader.setMat4("model", each_model);
    //  glDrawArrays(GL_TRIANGLES, 0, 36);
    //} 
    //glBindVertexArray(0);
    //glStencilMask(0xFF);
    //glEnable(GL_DEPTH_TEST);


    //lightShader.use();
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, light_pos);
    //model = glm::scale(model, glm::vec3(0.2f));
    //lightShader.setMat4("transform", trans);
    //lightShader.setMat4("model", model);
    //lightShader.setMat4("view", view);
    //lightShader.setMat4("projection", projection);
    //glm::vec3 lightCubeColor = lightColor * glm::vec3(0.8f);
    //lightShader.setVec3("lightColor", lightCubeColor);
    //glBindVertexArray(VAO);
    //for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
    //  model = glm::translate(model, pointLightPositions[i]);
    //  lightShader.setMat4("model", model);
    //  glDrawArrays(GL_TRIANGLES, 0, 36);
    //}
    //glBindVertexArray(0);

    // geometry
    //geomShader.use();
    //glBindVertexArray(geomVAO);
    //glDrawArrays(GL_POINTS, 0, 4);

    // now bind back to default framebuffer and draw a quad plane with the
    // attached framebuffer color texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, DISPLAY_WIDTH,
                      DISPLAY_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    screenShader.use();
    screenShader.setFloat("divider", divider);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  cout << "framebuffer_size_callback" << endl;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mixOpacity += 0.001;
    if (mixOpacity > 1.0) {
      mixOpacity = 1.0;
    }
    radian += 0.1;
    if (radian > 180) {
      radian = 180;
    }
  } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mixOpacity -= 0.001;
    if (mixOpacity < 0.0f) {
      mixOpacity = 0.0f;
    }
    radian -= 0.1;
    if (radian < -180) {
      radian = -180;
    }
  }

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

  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) ambient_mult += speed;
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) ambient_mult -= speed;
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) diffuse_mult += speed;
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) diffuse_mult -= speed;
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) specular_mult += speed;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) specular_mult -= speed;

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) divider += 1;
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) divider -= 1;
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
