#version 330

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec4 a_Color;
out vec4 outColor;
void main()
{
	vec4 newPosition;
	newPosition.xyz = a_Position; 
	newPosition.w=1;
	gl_Position = newPosition;

	outColor=a_Color;
}
