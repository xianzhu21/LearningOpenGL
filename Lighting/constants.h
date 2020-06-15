#pragma once

#include "GLFW/glfw3.h"

#include <string>
#include <vector>

using namespace std;

float cube_vertices[] = {
   // Back face                         // normal
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    0.0f,  0.0f, -1.0f,
   // Front face  
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    0.0f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    0.0f,  0.0f, 1.0f,
   // Left face  
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
   // Right face  
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    1.0f,  0.0f,  0.0f,
   // Bottom face  
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
   // Top face  
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,    0.0f,  1.0f,  0.0f
};

float planeVertices[] = {
  // positions          // texture   // normal
   5.0f, -0.5f,  5.0f,  1.5f, 0.0f,  0.0f, 1.0f, 0.0f,
  -5.0f, -0.5f, -5.0f,  0.0f, 1.5f,  0.0f, 1.0f, 0.0f,
  -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

   5.0f, -0.5f,  5.0f,  1.5f, 0.0f,  0.0f, 1.0f, 0.0f,
   5.0f, -0.5f, -5.0f,  1.5f, 1.5f,  0.0f, 1.0f, 0.0f,
  -5.0f, -0.5f, -5.0f,  0.0f, 1.5f,  0.0f, 1.0f, 0.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -5.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -2.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

glm::vec3 attenuations[] = {
  glm::vec3(1.0f,	0.14f,  0.07f),   // 32	
  glm::vec3(1.0f,	0.09f,  0.032f),  // 50	
  glm::vec3(1.0f,	0.07f,  0.017f),  // 65	
  glm::vec3(1.0f,	0.045f,	0.0075f)  // 100	
};

glm::vec3 pointLightPositions[] = {
  glm::vec3( 0.7f,  0.2f,  2.0f),
  glm::vec3( 2.3f, -3.3f, -4.0f),
  glm::vec3(-4.0f,  2.0f, -12.0f),
  glm::vec3( 0.0f,  0.0f, -3.0f)
};

float transparentVertices[] = {
  // positions         // texture Coords
  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
  0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
  1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

// transparent vegetation locations
vector<glm::vec3> vegetation 
{
  glm::vec3(-1.5f, 0.0f, -0.48f),
  glm::vec3( 1.5f, 0.0f, 0.51f),
  glm::vec3( 0.0f, 0.0f, 0.7f),
  glm::vec3(-0.3f, 0.0f, -2.3f),
  glm::vec3 (0.5f, 0.0f, -0.6f)
};

// vertex attributes for a quad that fills the entire screen in Normalized
// Device Coordinates.
float quadVertices[] = {
  // positions   // texCoords
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f, 0.0f,

  -1.0f,  1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f
};

vector<string> faces
{
  "./right.jpg",
  "./left.jpg",
  "./top.jpg",
  "./bottom.jpg",
  "./front.jpg",
  "./back.jpg"
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

float points[] = {
    -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  // top-left
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // top-right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f,  // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f   // bottom-left
};

const string &vertPath = "./vertex.vert";
const string &objectFragPath = "./object.frag";
const string &lightFragPath = "./light.frag";

constexpr float DISPLAY_WIDTH = 1200.0f;
constexpr float DISPLAY_HEIGHT = 900.0f;