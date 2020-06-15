#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
  //glm::vec3 tangent;
  //glm::vec3 bitangent;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

const std::string TEXTURE_DIFFUSE("diffuse");
const std::string TEXTURE_SPECULAR("specular");
const std::string TEXTURE_NORMAL("normal");
const std::string TEXTURE_HEIGHT("height");

class Mesh {
 public:
  // Mesh Data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures);
  void Draw(Shader shader);

 private:
  // Render data
  unsigned int VAO, VBO, EBO;

  void setupMesh();
};
#endif  // MESH_H
