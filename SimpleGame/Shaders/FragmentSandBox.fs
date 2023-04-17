#version 330

layout(location=0) out vec4 FragColor;

varying vec2 v_Texcoord;
uniform vec2 u_EnemyPos;
uniform vec2 u_EnemyPoss[3];
uniform float u_Time;

const float c_PI =3.14;


void Circle()
{
	
	vec2 newValue = v_Texcoord - u_EnemyPoss[0];
	float d =length(newValue);

	if(d<0.5)
	{
	FragColor = vec4(v_Texcoord.xy, 0.0f, 1.0f);
	}
	else
	{
	FragColor = vec4(0.0f, 0.0f,0.0f, 1.0f);
	
	}
}

void radar()
{
	float obj_mask =0.0;
	vec2 newValue =v_Texcoord-vec2(0.5,0.5);
	float d =length(newValue);
	float value=sin(2*c_PI*d -13*u_Time)-0.9;
		float ring_mask =ceil(value);
	
for(int i=0;i<3;i++)
	{	
		vec2 temp =v_Texcoord-u_EnemyPoss[i];
		float d =length(temp);
			if(d<0.1)
	{
		obj_mask+=1.0f;
	}

	}
	FragColor=vec4(ring_mask*obj_mask+10*value);
}

void Circles()
{
	vec2 newValue = v_Texcoord - u_EnemyPoss[1];
	float d =length(newValue);
	FragColor =vec4(sin(4*c_PI*d-5*u_Time));
}

void main()
{
	radar();
}
