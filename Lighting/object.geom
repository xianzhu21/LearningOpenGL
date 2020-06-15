#version 420 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
  vec2 texCoord;
  vec3 Normal;
  vec3 FragPos;
  vec3 ClipNormal;
} gs_in[];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform float time;
uniform bool useGeom = false;

const float MAGNITUDE = 0.4;

vec4 explode(vec4 position, vec3 normal) {
  float magnitude = 2.0;
  vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
  return position + vec4(direction, 0.0);
}

vec3 GetNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

void GenerateLine(int index) {
  gl_Position = gl_in[index].gl_Position;
  EmitVertex();
  gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].ClipNormal, 0.0) * MAGNITUDE;
  EmitVertex();
  EndPrimitive();
}

void main() {
//  vec3 normal = GetNormal();
//
//  for (int i = 0; i < 3; ++i) {
//    if (useGeom) {
//      gl_Position = explode(gl_in[i].gl_Position, normal);
//    } else {
//      gl_Position = gl_in[i].gl_Position;
//    }
//    TexCoord = gs_in[i].texCoord;
//    Normal = gs_in[i].Normal;
//    FragPos = gs_in[i].FragPos;
//    EmitVertex();
//  }
//  EndPrimitive();

  GenerateLine(0);
  GenerateLine(1);
  GenerateLine(2);
}
