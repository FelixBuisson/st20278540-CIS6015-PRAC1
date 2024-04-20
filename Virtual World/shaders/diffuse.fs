#version 450 core

layout(binding = 0) uniform sampler2D ColTexture[4];


in ControlPoint {
  float    intensity;
  vec2    texCoord;
  vec3 worldPos; 
  vec3 rv; 
  vec3 normal;
} inVertex;

uniform vec3 lightCol;
uniform vec3 lightAmb;

out vec4 FragColor; // Color that will be used for the fragment

/////////////////////////////////////////// ///////////////////////
// main()
//////////////////////////////////////////////////////////////////
void main()
{
    
    vec4 colour = (vec4(lightAmb,1.0) + inVertex.intensity * vec4(lightCol,1.0)) * texture(ColTexture[0], inVertex.texCoord.xy);

    FragColor = vec4(colour.xyz, 1.0);
}