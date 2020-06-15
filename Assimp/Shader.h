#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
 private:
  unsigned int ID;
  std::string vShaderPath, fShaderPath;

  unsigned int createShader(int type, std::string codePath);
  void init();

 public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  void use();
  unsigned int getID();
  unsigned int createTexture(std::string filename, int format, int param);
  // utility uniform
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4 &value) const;
  void setVec3(const std::string &name, float v1, float v2, float v3) const;
  void setVec3(const std::string &name, glm::vec3 &value) const;
};
#endif  // !SHADER_H
