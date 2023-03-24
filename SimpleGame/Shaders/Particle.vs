#version 330

layout(location = 0)in vec3 a_Position;

layout(location = 1)in vec3 a_Vel;
layout(location = 2)in vec4 a_Color;
layout(location = 3)in float a_EmitTime;
layout(location = 4)in float a_LifeTime;
out vec4 outColor;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel= vec3(1.0,0.0,0.0);
const float c_resetTime=2.f;
const float c_PI=3.14952;




void main()
{
	vec4 newPos=vec4(0,0,0,1);
	float t = fract(u_Time);

	newPos.x = a_Position.x + c_Vel.x * t;
	newPos.y = a_Position.y + sin(t*c_PI*2.0);

	gl_Position =newPos;
}




/*void main()
{
	vec4 newPosition=vec4(0,0,0,1);

	float t = u_Time - a_EmitTime;

	if(t<0.0)
	{
	}
	else
	{
	float newT = a_LifeTime*fract(t/a_LifeTime);
	newPosition.xyz = a_Position
		+a_Vel*newT
		+0.5 * u_Accel*newT*newT; 
		
		newPosition.w=1;
	}
	gl_Position = newPosition;

	outColor=a_Color;
}*/
