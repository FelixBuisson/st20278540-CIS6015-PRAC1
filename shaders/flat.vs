#version 450 core

layout (location = 0) in vec3               vertexPos_VS;	  	// Position in attribute location 0
layout (location = 1) in vec2               texCoord_VS;
layout (location = 2) in vec3               vertexNormal_VS;

layout (location = 3) uniform mat4          modelMatrix;
layout (location = 4) uniform mat4          viewMatrix;
layout (location = 5) uniform mat4          projMatrix;

out ControlPoint {
  float    intensity;
  vec2    texCoord;
  vec3 worldPos; 
  vec3 rv; 
  vec3 normal;
} outControlPoint;

//////////////////////////////////////////////////////////////////
// main()
//////////////////////////////////////////////////////////////////
void main()
{    
    outControlPoint.texCoord    = texCoord_VS;

    gl_Position   = (projMatrix * viewMatrix * modelMatrix * vec4(vertexPos_VS, 1.0));
}