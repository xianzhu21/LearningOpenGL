#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string &directory,
                             bool gamma = false);

class Model {
 public:
  Model(const std::string &path) { loadModel(path); }
  void Draw(Shader shader);

 private:
  // Model Data
  std::vector<Mesh> meshes;
  std::string directory;
  std::vector<Texture> textures_loaded;

  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string type_name);
};

#endif  // !MODEL_H
