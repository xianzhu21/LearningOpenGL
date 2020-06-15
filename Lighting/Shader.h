#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#endif  // !SHADER_H

class Shader {
 private:
  unsigned int ID;
  std::string vShaderPath, fShaderPath, gShaderPath;
  unsigned int createShader(int type, std::string codePath);

 public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath,
         const std::string &geometryPath = "");

  void init();
  void use();
  unsigned int getID();
  // utility uniform
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4 &value) const;
  void setVec3(const std::string &name, float v1, float v2, float v3) const;
  void setVec3(const std::string &name, glm::vec3 &value) const;
  void setUbo(const std::string &name, int value) const;
};
unsigned int createTexture(std::string filename, int param);
unsigned int loadCubemap(std::vector<std::string> faces);
