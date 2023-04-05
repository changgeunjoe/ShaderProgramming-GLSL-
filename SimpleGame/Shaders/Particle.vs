#version 330

layout(location = 0)in vec3 a_Position;

layout(location = 1)in vec3 a_Vel;
layout(location = 2)in vec4 a_Color;
layout(location = 3)in float a_EmitTime;
layout(location = 4)in float a_LifeTime;
layout(location = 5)in float a_PeriodTime;
layout(location = 6)in float a_AmpTime;
layout(location = 7)in float a_ValueTime;
out vec4 outColor;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel= vec3(-1.0,1.0,0.0);
const float c_resetTime=2.f;
const float c_PI=3.14952;
const float c_Amp=0.5;
const float c_Period=2.0;

vec4 GraphSin()
{
	vec4 newPos=vec4(0,0,0,1);
	float t = u_Time - a_EmitTime;

	if(t<0.0)
	{
	}
	else
	{
		float newT = a_LifeTime*fract(t/a_LifeTime);
		float paraX = sin(a_ValueTime*2*c_PI);
		float paraY = cos(a_ValueTime*2*c_PI);
		newPos.x = a_Position.x + c_Vel.x * newT+paraX;
		newPos.y = a_Position.y + c_Vel.y * newT+paraY;

		vec2 nVel= vec2(-c_Vel.y,c_Vel.x);
		nVel=normalize(nVel);

		newPos.xy=newPos.xy+
			nVel*
			newT*a_AmpTime*
			sin((1.0+newT)*a_PeriodTime*newT*c_PI*2.0);
	}


	return newPos;
}

vec4 p1()
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


	outColor=a_Color;

	return newPosition;

}




void main()
{
	gl_Position =GraphSin();
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
