#include "Model.h"

void Model::Draw(Shader shader) {
  for (unsigned int i = 0; i < meshes.size(); ++i) {
    meshes[i].Draw(shader);
  }
}

void Model::loadModel(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  std::cout << "directory=" << directory << std::endl;

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
    Vertex vertex;
    vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                mesh->mVertices[i].z);
    vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                              mesh->mNormals[i].z);
    if (mesh->mTextureCoords[0]) {          
      vertex.tex_coord =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      vertex.tex_coord = glm::vec2(0.0f, 0.0f);
    }
    //vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y,
    //                           mesh->mTangents[i].z);
    //vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
    //                             mesh->mBitangents[i].z);
    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuse_maps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
    std::vector<Texture> specular_maps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, TEXTURE_SPECULAR);
    std::vector<Texture> normal_maps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL);
    std::vector<Texture> height_maps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, TEXTURE_HEIGHT);
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
    textures.insert(textures.end(), height_maps.begin(), height_maps.end());
  }

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string type_name) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), directory);
      texture.type = type_name;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture);
    }
  }
  return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory,
                             bool gamma) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
