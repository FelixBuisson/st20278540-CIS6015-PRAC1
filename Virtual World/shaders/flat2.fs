#version 450 core

layout(binding = 0) uniform sampler2D ColTexture;
layout(binding = 1) uniform sampler2D Col2Texture;


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
    vec4 colour2 = texture(Col2Texture, inVertex.texCoord.xy);
    vec4 average = 0.5f*(colour+colour2);

    FragColor = vec4(average.xyz, 1.0);
}