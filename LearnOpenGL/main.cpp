#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Camera.h"

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
int createShaderProgram(string vertPath, string fragPath);
bool readFileToString(const string &filepath, string *out);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

unsigned int createShader(int type, string sourcePath);

float vertices[] = {
   // position 1        // position 2       // colors          // texture
   -0.5f, -0.5f, 0.0f,  0.0f, -0.3f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,  0.6f, -0.3f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    0.0f,  0.5f, 0.0f,  0.3f,  0.3f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f
};

float rectangle_vts[] = {
  // positions         // texture
   1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
   1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
  -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
};
unsigned int indices[] = {
  0, 1, 3,  // first triangle
  1, 2, 3   // second triangle
};

float texCoords[] = {
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.5f, 1.0f
};

float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

const string &vertPath = "./vertex.vert";
const string &objectFragPath = "./object.frag";
const string &fragYellowPath = "./frag_yellow.frag";

constexpr float DISPLAY_WIDTH = 1000.0f;
constexpr float DISPLAY_HEIGHT = 1000.0f;

float mixOpacity = 0.1;
float radian = 0.0;

float speed = 0.0f;  // Time between current frame and last frame
float last_frame = 0.0f;  // Time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float last_x = DISPLAY_WIDTH / 2.0f;
float last_y = DISPLAY_HEIGHT / 2.0f;

int main() {
  cout << "LearnOpenGL" << endl;
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

  Camera camera;
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

  renderWindow(window);

  glfwTerminate();

  return 0;
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

void renderWindow(GLFWwindow *window) {
  double red = 0.2f;
  double green = 0.3f;
  double blue = 0.3f;

  Shader shader(vertPath, objectFragPath);
  shader.init();

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  //glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vts), rectangle_vts,
  //             GL_STATIC_DRAW);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //             GL_STATIC_DRAW);
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
  //                      (void *)0);

  // cube
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

  /*
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // position
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)0);
  // color
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  */

  // texture
  unsigned int texture0 =
      shader.createTexture("./container.jpg", GL_RGB, GL_CLAMP_TO_BORDER);
  unsigned int texture1 =
      shader.createTexture("./awesomeface.png", GL_RGBA, GL_REPEAT);
  glEnableVertexAttribArray(3);
  // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
  //                      (void *)(9 * sizeof(float)));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  // light
  unsigned int lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't
  // unbind VAOs (nor VBOs) when it's not directly necessary
  glBindVertexArray(0);
  
  //unsigned int VAO1;
  //glGenVertexArrays(1, &VAO1);
  //glBindVertexArray(VAO1);

  //glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
  //                      (void *)(3 * sizeof(float)));

  // glBindVertexArray(0);

  // int shaderProgram = createShaderProgram(vertexShaderPath,
  // fragmentShaderPath);
  int yellowShader = createShaderProgram(vertPath, fragYellowPath);

  shader.use();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    speed = currentFrame - last_frame;
    last_frame = currentFrame;

    processInput(window);

    glClearColor(red, green, blue, 1.0f);
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw triangle
    float time = glfwGetTime();
    //float green = sin(time) / 2.0f;
    //int gColorLocation = glGetUniformLocation(shader.getID(), "gColor");
    //glUniform4f(gColorLocation, 0.0f, green, 0.0f, 1.0f);

    // transformations
    glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    shader.setMat4("transform", trans);
    //trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
    //trans =
    //    glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    //unsigned int transformLoc =
    //    glGetUniformLocation(shader.getID(), "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f),
    //                    glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    //view = glm::rotate(view, glm::radians(radian), glm::vec3(1.0f, 0.0f, 0.0f));
    //const float radius = 10.0f;
    //float camX = sin(glfwGetTime()) * radius;
    //float camZ = cos(glfwGetTime()) * radius;
    //view = glm::lookAt(glm::vec3(camX, 10.0, camZ),  // cam position
    //                   glm::vec3(0.0, 0.0, 0.0),     // target
    //                   glm::vec3(0.0, 1.0, 0.0));    // up vector
    view =
        glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom),
                                  DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setFloat("offsetX", sin(time) / 2.0f);
    shader.setFloat("offsetY", cos(time) / 2.0f);
    shader.setFloat("offsetZ", tan(time) / 2.0f);

    //std::cout << "mixOpacity=" << mixOpacity << std::endl;
    shader.setFloat("mixOpacity", mixOpacity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    shader.setInt("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    shader.setInt("texture1", 1);

    glBindVertexArray(VAO);

    // draw triangle
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    //glUseProgram(yellowShader);
    //glBindVertexArray(VAO1);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // draw rectangle
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);

    // rotate


    // draw cube
    // draw cubes
    for (unsigned int i = 0; i < 10; ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      if (i == 0 || i % 3 == 0) {
        angle += glfwGetTime() * 30.0f;
      }
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
      shader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // second rectangle
    //glBindVertexArray(VAO);
    //glm::mat4 trans2 = glm::mat4(1.0f);
    ////trans =
    ////    glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    //float length = sin(glfwGetTime());
    //trans2 = glm::translate(trans2, glm::vec3(-length, length, 0.0f));
    //trans2 = glm::scale(trans2, glm::vec3(0.3, 0.3, 0.3));
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwPollEvents();
    glfwSwapBuffers(window);

    // this_thread::sleep_for(chrono::seconds(1));
    // red += 0.05f;
    // green += 0.05f;
    // blue += 0.05f;
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}


int createShaderProgram(string vertPath, string fragPath) {
  unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertPath);
  unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragPath);

  cout << "vertexShader=" << vertexShader
       << ", fragmentShader=" << fragmentShader << endl;

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int linked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
  if (!linked) {
    char infoLog[512];
     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
     cout << "Linked error." << endl;
     return -1;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

unsigned int createShader(int type, string sourcePath) {
  unsigned int shader;
  shader = glCreateShader(type);

  string source;
  if (!readFileToString(sourcePath, &source)) {
    cout << "Get shader source from " << sourcePath << "error." << endl;
    return -1;
  }
  const char *cstr = source.c_str();

  glShaderSource(shader, 1, &cstr, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    return -1;
  }

  return shader;
}

bool readFileToString(const string &filepath, string *out) {
  if (ifstream is{filepath, ios::binary | ios::ate}) {
    auto size = is.tellg();
    string str(size, '\0');
    is.seekg(0);
    if (is.read(&str[0], size)) {
      *out = str;
      return true;
    }
  }
  cout << "file error" << endl;
  return false;
}
