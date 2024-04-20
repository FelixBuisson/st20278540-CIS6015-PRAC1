#version 450 core

uniform vec3 useCol;

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
    FragColor = vec4(useCol,1.0);
}