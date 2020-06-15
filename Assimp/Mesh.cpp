#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  setupMesh();
}

void Mesh::setupMesh() {
  std::cout << vertices.size() << "----" << indices.size() << std::endl;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coord));
  //// vertex tangent
  //glEnableVertexAttribArray(3);
  //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                      (void*)offsetof(Vertex, tangent));
  //// vertex bitangent
  //glEnableVertexAttribArray(4);
  //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                      (void*)offsetof(Vertex, bitangent));

  glBindVertexArray(0);
}

void Mesh::Draw(Shader shader) {
  unsigned int diffuse_num = 0;
  unsigned int specular_num = 0;
  unsigned int normal_num = 0;
  unsigned int height_num = 0;

  for (unsigned int i = 0; i < textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;
    if (name == TEXTURE_DIFFUSE) {
      number = std::to_string(diffuse_num++);
    } else if (name == TEXTURE_SPECULAR) {
      number = std::to_string(specular_num++);
    } else if (name == TEXTURE_NORMAL) {
      number = std::to_string(normal_num++);
    } else if (name == TEXTURE_HEIGHT) {
      number = std::to_string(height_num++);
    }

    shader.setInt(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  shader.setFloat("material.shininess", 32.0f);

  //std::cout << "VAO=" << VAO << std::endl;
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}
