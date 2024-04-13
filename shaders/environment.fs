#version 450 core

layout(binding = 0) uniform sampler2D ColTexture;
layout(binding = 4) uniform samplerCube SkyboxTexture;


in ControlPoint {
  float    intensity;
  vec2    texCoord;
  vec3 worldPos; 
  vec3 rv; 
  vec3 normal;
} inVertex;

out vec4 FragColor; // Color that will be used for the fragment


/////////////////////////////////////////// ///////////////////////
// main()
//////////////////////////////////////////////////////////////////
void main()
{
    
    vec4 colour = texture(ColTexture, inVertex.texCoord.xy);
    vec4 colourSky = texture(SkyboxTexture, inVertex.rv);
    
    vec4 colourUse = mix(inVertex.intensity*colour, colourSky,0.5);

    FragColor = vec4(colourUse.xyz, 1.0);
}