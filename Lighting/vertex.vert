#version 420 core
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aNormal;
layout (location = 5) in mat4 aInstanceModel;

out vec4 vertexColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform float offsetX;
uniform float offsetY;
uniform float offsetZ;

uniform mat4 transform;
uniform mat4 model;

uniform bool useInstance;
uniform vec3 offsets[20];

out VS_OUT {
  vec2 texCoord;
  vec3 Normal;
  vec3 FragPos;
  vec3 ClipNormal;
} vs_out;

layout (std140, binding = 0) uniform Matrices
{
  mat4 view;
  mat4 projection;
};

void main()
{
  // gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, aPos.z, 1.0);
  // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  vec3 position = aPos;
  if (useInstance) {
    gl_Position = projection * view * aInstanceModel * transform * vec4(position, 1.0);
  } else {
    gl_Position = projection * view * model * transform * vec4(position, 1.0);
  }
  vertexColor = vec4(gl_Position.xy, 0.5, 1.0);
  TexCoord = aTexCoord;
  Normal = mat3(transpose(inverse(model))) * aNormal;
  FragPos = vec3(model * vec4(aPos, 1.0));
  vs_out.texCoord = TexCoord;
  vs_out.Normal = Normal;
  vs_out.FragPos = FragPos;

  mat3 normalMatrix = mat3(transpose(inverse(view * model)));
  vs_out.ClipNormal = normalize(vec3(projection * vec4(normalMatrix * aNormal, 0.0)));
}