#version 330

layout(location=0) out vec4 FragColor;
in vec4 v_Color;
in vec4 outColor;
void main()
{
	FragColor = v_Color;
}
