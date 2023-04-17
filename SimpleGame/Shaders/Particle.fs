#version 330

layout(location=0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_UV;

void main()
{
   FragColor = vec4(v_UV,0,1);
}