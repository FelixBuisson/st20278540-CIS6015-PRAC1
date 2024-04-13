#version 450 core

layout(binding = 0) uniform sampler2D ColTexture;


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

    FragColor = vec4(colour.xyz, 1.0);
}