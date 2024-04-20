#version 450 core

layout(binding = 0) uniform sampler2D ColTexture[4];


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
    
    vec4 colour = texture(ColTexture[0], inVertex.texCoord.xy);

    float toonIntensity = 0.25 * floor(inVertex.intensity / 0.25);

    FragColor = vec4(vec3(colour.xyz * toonIntensity), 1.0);
}