#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;

uniform vec4 gColor;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float mixOpacity;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
  vec2 yTexCoord = vec2(-TexCoord.x, TexCoord.y);
  FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), mixOpacity);
  // FragColor = texture(texture0, yTexCoord);0
  // FragColor = gColor;
}