#version 430 core

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate

// The end result of this shader
out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;
uniform float sinTime;

const int MAX_TEXTURES=2;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    bool activeTextures[MAX_TEXTURES];
    sampler2D textures[MAX_TEXTURES];
};

// Definition of a light source structure
struct LightSource
{
  bool enabled;
  vec4 position;
  vec4 diffuse;
  vec4 specular;
};

const int MaxNumberOfLights = 10;

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

// Some hard coded default ambient lighting
vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);

// The front surface material
uniform Material material;

uniform sampler2D u_depthTexture;
in vec4 _fragPosLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDirection)
{
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;
    float closestDepth = texture(u_depthTexture, projectedCoords.xy).r;
    float currentDepth = projectedCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    //Percentage Closer Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_depthTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float percentageCloserFiltering = texture(u_depthTexture, projectedCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > percentageCloserFiltering ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

void main()
{
  vec3 normalDirection = normalize(normal);
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
  vec3 lightDirection;
  float attenuation;

  // initialize total lighting with ambient lighting
  vec3 totalLighting = vec3(scene_ambient) * vec3(material.ambient);


  // for all light sources
  for (int index = 0; index < numberOfLights; index++) 
  {
    LightSource light = lights[index];
    if (0.0 == light.position.w) // directional light?
    {
      attenuation = 1.0; // no attenuation
      lightDirection = normalize(vec3(light.position));
    }
    else // point light or spotlight (or other kind of light) 
    {
      vec3 positionToLightSource = vec3(light.position - position);
      float distance = length(positionToLightSource);
      lightDirection = normalize(positionToLightSource);
      attenuation = 1.0;
    }

    vec3 diffuseReflection = attenuation
      * vec3(light.diffuse) * vec3(material.diffuse)
      * max(0.0, dot(normalDirection, lightDirection));

    vec3 specularReflection;
    if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
    {
      specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
    }
    else // light source on the right side
    {
      specularReflection = attenuation * vec3(light.specular) * vec3(material.specular)
        * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), max(0.00001,material.shininess));
    }

      float shadow = ShadowCalculation(_fragPosLightSpace, lightDirection);

      totalLighting = totalLighting + (1.0 - shadow) * (diffuseReflection + specularReflection);
  }

  vec4 diffuseTex = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 mixedTextureColor = vec4(1.0, 0.0, 0.0, 1.0);

  // How we could check for a diffuse texture map
  if (material.activeTextures[0])
  {
    diffuseTex = texture2D(material.textures[0], texCoord);
    mixedTextureColor = diffuseTex;
  }

  if(material.activeTextures[1])
  {
    vec4 diffuseTex2 = texture2D(material.textures[1], texCoord);
    if(!(diffuseTex2.r == 0  && diffuseTex2.g == 0 && diffuseTex2.b == 0))
      mixedTextureColor = mix(diffuseTex, diffuseTex2, 0.5);
  }

  if(material.activeTextures[0] || material.activeTextures[1])
  {
      totalLighting = totalLighting * mixedTextureColor.rgb;
  }

  color = vec4(totalLighting, mixedTextureColor.a);
}
