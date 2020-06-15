#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  vShaderPath = vertexPath;
  fShaderPath = fragmentPath;

  std::cout << vShaderPath << fShaderPath << std::endl;
}

void Shader::init() {
  unsigned int vShader = createShader(GL_VERTEX_SHADER, vShaderPath);
  unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fShaderPath);

  ID = glCreateProgram();
  glAttachShader(ID, vShader);
  glAttachShader(ID, fShader);
  glLinkProgram(ID);

  int suc;
  char infoLog[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &suc);
  if (!suc) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKED_FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

void Shader::use() { glUseProgram(ID); }

unsigned int Shader::getID() { return ID; }

void Shader::setBool(const std::string &name, bool value) const {
  setInt(name, (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


unsigned int Shader::createShader(int type, std::string codePath) {
  std::string codeStr;
  std::ifstream shaderFile;
  // ensure ifstream objects can throw exceptions
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    shaderFile.open(codePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    codeStr = shaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
  const char *code = codeStr.c_str();

  unsigned int shader;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    return -1;
  }

  return shader;
}

unsigned int Shader::createTexture(std::string filename, int format, int param) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if (param == GL_CLAMP_TO_BORDER) {
    float borderColor[] = { 0.0f, 0.6f, 0.3f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  }
  // load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture;
}
