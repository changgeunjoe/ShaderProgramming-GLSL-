#version 330

in vec3 outColor;

layout(location=0) out vec4 FragColor;
uniform vec4 u_Color;

void main()
{
	FragColor = vec4(outColor, u_Color.a);
}
