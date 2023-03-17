#version 330

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec3 a_Color;
//in vec2 a_Texcoord;
out vec3 outColor;
uniform vec4 u_Trans;

uniform float u_Scale;
void main()
{
	vec4 newPosition;

	float time =u_Scale;
	newPosition.xy = a_Position.xy*u_Trans.w*u_Scale + u_Trans.xy;
	newPosition.z = 0;
	newPosition.w= 1;
	gl_Position = newPosition;

	outColor=a_Color;
}
