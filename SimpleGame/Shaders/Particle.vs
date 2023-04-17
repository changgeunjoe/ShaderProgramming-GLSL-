#version 330

in vec3 a_Position;
in vec3 a_Vel;
in vec4 a_Color;
in float a_EmitTime;
in float a_LifeTime;
in float a_Amp;
in float a_Period;
in float a_Theta;

in vec2 a_Texcoord;

uniform float u_Time;
uniform vec3 u_Accel;

const vec3 c_Vel = vec3(0.0, -1.0, 0.0);
const float c_resetTime = 2.f;
const float c_PI = 3.141592;
const float c_Amp = 0.5f;
const float c_Period = 1.0f;

out vec4 v_Color;
out vec2 v_UV;

vec4 GraphSin()
{
   vec4 newPos = vec4(0, 0, 0, 1);
   float t = u_Time - a_EmitTime;
   if(t < 0.0)
   {
   }
   else
   {   
      float newT = a_LifeTime * fract(t / a_LifeTime);
      newPos.x = a_Position.x + c_Vel.x * newT;
      newPos.y = a_Position.y + c_Vel.y * newT;

      vec2 nVel = vec2(-c_Vel.y, c_Vel.x);   // 기준 벡터
      nVel = normalize(nVel);

      newPos.xy = newPos.xy + nVel * newT * a_Amp * sin((1 + newT) * a_Period * newT * 2.0f * c_PI);

   }
   return newPos;
}

vec4 P1()
{
   vec4 newPosition = vec4(0, 0, 0, 1);
   float t = u_Time - a_EmitTime;
   if(t < 0)
   {
   }
   else
   {   
      float newT = a_LifeTime * fract(t / a_LifeTime);
      newPosition.xyz = a_Position
               + a_Vel * newT
               + 0.5 * u_Accel * newT * newT;
      newPosition.w= 1;
   }
   return newPosition;
}

void DrawCircle()
{
   vec4 newPos = vec4(0, 0, 0, 1);
   float alpha;
   float t = u_Time - a_EmitTime;
   if(t < 0.0)
   {
   }
   else
   {
      float newT = a_LifeTime * fract(t/a_LifeTime);   // fract : 소수 부분만 나타내는 것
      float paraX = sin(a_Theta * 2 * c_PI);
      float paraY = cos(a_Theta * 2 * c_PI);
      newPos.x = a_Position.x + paraX + c_Vel.x * newT;
      newPos.y = a_Position.y + paraY + c_Vel.y * newT;
      
      vec2 nVel = vec2(-c_Vel.y, c_Vel.x);   // 기준 벡터
      nVel = normalize(nVel);

      newPos.xy = newPos.xy + nVel * newT * a_Amp * sin((1 + newT) * a_Period * newT * 2.0f * c_PI);

      //newPos.xy = newPos.xy + nVel * newT * a_Amp * sin((1 + newT) * a_Period * newT * 2.0f * c_PI);

      // newPos.x = (a_Position.x + sin(a_Theta * 2.0f * c_PI * newT)) / 2;
      // newPos.y = (a_Position.y + cos(a_Theta * 2.0f * c_PI * newT)) / 2;

      alpha = (1 - newT / a_LifeTime);
   }
   gl_Position = newPos;
   v_Color = vec4(a_Color.rgb, a_Color * alpha);
   v_UV=a_Texcoord;
}

void main()
{
   DrawCircle();
}