#version 450 core

layout (location = 0) in vec3               vertexPos_VS;	  	// Position in attribute location 0
layout (location = 1) in vec2               texCoord_VS;
layout (location = 2) in vec3               vertexNormal_VS;

layout (location = 3) uniform mat4          modelMatrix;
layout (location = 4) uniform mat4          viewMatrix;
layout (location = 5) uniform mat4          projMatrix;

uniform vec3			lightPos;

uniform vec3 camPos;

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

	vec4 m_pos = modelMatrix * vec4(vertexPos_VS, 1.0);
	vec3 n = vec3(mat3(modelMatrix) * vertexNormal_VS);
	n = normalize(n);

	vec3 light = vec3(lightPos.xyz);
	vec3 lv = light - vec3(m_pos.xyz);
	float d = length(lv);
	lv = normalize(lv);
	
	outControlPoint.intensity = clamp(dot(n,lv), 0.0, 1.0);

    vec3 v = normalize(m_pos.xyz-camPos);
    outControlPoint.rv = reflect(v,n);

    gl_Position   = (projMatrix * viewMatrix * modelMatrix * vec4(vertexPos_VS, 1.0));
}