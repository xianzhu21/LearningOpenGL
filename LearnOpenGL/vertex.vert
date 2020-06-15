#version 330 core
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;

uniform float offsetX;
uniform float offsetY;
uniform float offsetZ;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  // gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, aPos.z, 1.0);
  // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  gl_Position = projection * view * model * transform * vec4(aPos, 1.0);
  vertexColor = vec4(gl_Position.xy, 0.5, 1.0);
  TexCoord = aTexCoord;
}