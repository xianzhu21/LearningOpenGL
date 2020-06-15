#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
  sampler2D diffuse0;
  sampler2D specular0;
  sampler2D emission;
  float shininess;
};

struct Lighting {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;
  float constant;
  float linear;
  float quadratic;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 camPos;

Lighting CalcLighting(Lighting lighting, vec3 lightDir, vec3 normal, vec3 viewDir);
vec3 CalcDirLighting(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLighting(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLighting(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(camPos - FragPos);

  vec3 outColor = CalcDirLighting(dirLight, norm, viewDir);
  outColor += CalcPointLighting(pointLight, norm, FragPos, viewDir);
  //outColor += CalcSpotLighting(spotLight, norm, FragPos, viewDir);

  FragColor = vec4(outColor, 1.0);
  //FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}

Lighting CalcLighting(Lighting lighting, vec3 lightDir, vec3 normal, vec3 viewDir) {
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 ambient = lighting.ambient * vec3(texture(material.diffuse0, TexCoord));
  vec3 diffuse = lighting.diffuse * diff * vec3(texture(material.diffuse0, TexCoord));
  vec3 specular = lighting.specular * spec * vec3(texture(material.specular0, TexCoord));
  return Lighting(ambient, diffuse, specular);
}

vec3 CalcDirLighting(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  return (lighting.ambient + lighting.diffuse + lighting.specular);
}

vec3 CalcPointLighting(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * light.linear
                            + light.quadratic * (distance * distance));
  return (lighting.ambient + lighting.diffuse + lighting.specular) * attenuation;
}

vec3 CalcSpotLighting(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  return lighting.ambient + lighting.diffuse * intensity + lighting.specular * intensity;
}
