#version 420 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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

#define NUM_POINT_LIGHTS 4

uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 camPos;
uniform float near;
uniform float far;
uniform sampler2D grassTex;
uniform sampler2D floorTex;
uniform bool grass;
uniform bool floor;
uniform samplerCube skybox;

Lighting CalcLighting(vec3 lightDir, vec3 normal, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);

void main()
{
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(camPos - FragPos);

  vec3 result = CalcDirLight(dirLight, norm, viewDir);
  //for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
  //  result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
  //}
  //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

  vec4 grassColor = texture(grassTex, TexCoord);
  vec4 floorColor = texture(floorTex, TexCoord);

  float ratio = 1.00 / 1.52;  // air/glass
  vec3 I = normalize(FragPos - camPos);
  vec3 R;
  if (!gl_FrontFacing) {
    norm = -norm;
  }
  R = reflect(I, norm);
  //R = refract(I, norm, ratio);
  if (floor) {
    //FragColor = floorColor;
    FragColor = texture(skybox, R);
  } else if (grass) {
      if (grassColor.a > 0.1)
        FragColor = grassColor;
  } else {
    //FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    //FragColor = texture(skybox, R);
    FragColor = vec4(result, 1.0);
  }

  //FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0);
}

float LinearizeDepth(float depth) {
  // back to NDC
  float z = depth * 2.0 - 1.0;
  return (2.0 * near * far) / (far + near -z * (far - near));
}

Lighting CalcLighting(Lighting lLighting, vec3 lightDir, vec3 normal, vec3 viewDir) {
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 ambient = lLighting.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = lLighting.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = lLighting.specular * spec * vec3(texture(material.specular, TexCoord));
  return Lighting(ambient, diffuse, specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  return (lighting.ambient + lighting.diffuse + lighting.specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * light.linear
                            + light.quadratic * (distance * distance));
  return (lighting.ambient + lighting.diffuse + lighting.specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  Lighting lighting = CalcLighting(Lighting(light.ambient, light.diffuse, light.specular),
                                   lightDir, normal, viewDir);
  
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  return lighting.ambient + lighting.diffuse * intensity + lighting.specular * intensity;
}
