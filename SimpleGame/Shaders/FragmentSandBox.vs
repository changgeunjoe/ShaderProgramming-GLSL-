#version 330

in vec3 a_Position;
in vec2 a_Texcoord;



varying vec2 v_Texcoord;
varying vec2 v_Enemypos;

void main()
{
	
	gl_Position = vec4(a_Position, 1.0f);
	v_Texcoord = a_Texcoord;

}
