#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	Class0310();
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs",
		"./Shaders/SolidRect.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs",
		"./Shaders/Particle.fs");
	m_FragmentSandboxShader = CompileShaders("./Shaders/FragmentSandbox.vs",
		"./Shaders/FragmentSandbox.fs");

	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticleVBO(10000);

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float rect1[]
		=
	{
		-1.f, -1.f, 0.f, 0.f, 1.f,	// x,y,z,u,v
		-1.f, 1.f, 0.f, 0.f, 0.f,
		1.f, 1.f, 0.f, 1.f, 0.f,//Triangle1
		-1.f, -1.f, 0.f,  0.f, 1.f,
		1.f, 1.f, 0.f, 1.f, 0.f,
		1.f , -1.f, 0.f, 1.f, 1.f,//Triangle2
	};

	glGenBuffers(1, &m_FragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string* target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float g_time;
void Renderer::Class0310_Rendering()
{
	//Program select
	glUseProgram(m_SolidRectShader);	// 

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), 0, 0, 0, 1);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), 1, 1, 1, 1);

	int attribLocation_Position = -1;
	attribLocation_Position = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribLocation_Position);							// 사용자가 어디선가 0번으로 정의를 한 것
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);	// index는 glEnableVertexAttribArray에서 설정한 인덱스와 똑같이 맞춰서 설정해주면 된다.

	int attribLocation_Position1 = -1;
	attribLocation_Position1 = glGetAttribLocation(m_SolidRectShader, "a_Position1");
	glEnableVertexAttribArray(attribLocation_Position1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glVertexAttribPointer(attribLocation_Position1, 3, GL_FLOAT, GL_FALSE, 0, 0);	// 3개씩 읽겠다는 것


	int uniformScale = glGetUniformLocation(m_SolidRectShader, "u_Scale");	//DrawCall 하기 전에 Set 해줘야 영향을 미친다. // 쉐이더에서 사용하지 않으면 -1로 적용된다.
	glUniform1f(uniformScale, g_time);
	g_time += 0.016f;
	if (g_time > 10.f)
		g_time = 0.f;

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::DrawParticle()
{
	GLuint program = m_ParticleShader;
	glUseProgram(program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//int posLoc = glGetAttribLocation(program, "a_Position");
	//glEnableVertexAttribArray(posLoc);
	//glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	//glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//int colorLoc = glGetAttribLocation(program, "a_Color");
	//glEnableVertexAttribArray(colorLoc);
	//glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	//glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	int posLoc = glGetAttribLocation(program, "a_Position");
	glEnableVertexAttribArray(posLoc);
	int colorLoc = glGetAttribLocation(program, "a_Color");
	glEnableVertexAttribArray(colorLoc);
	int uvLoc = glGetAttribLocation(program, "a_Texcoord");
	glEnableVertexAttribArray(uvLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColUVVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT,
		GL_FALSE, sizeof(float) * 9, 0);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT,
		GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT,
		GL_FALSE, sizeof(float) * 9, (GLvoid*)(sizeof(float) * 7));

	int speedLoc = glGetAttribLocation(program, "a_Vel");
	glEnableVertexAttribArray(speedLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glVertexAttribPointer(speedLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int emitTimeLoc = glGetAttribLocation(program, "a_EmitTime");
	glEnableVertexAttribArray(emitTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(emitTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int lifeTimeLoc = glGetAttribLocation(program, "a_LifeTime");
	glEnableVertexAttribArray(lifeTimeLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(lifeTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int ampLoc = glGetAttribLocation(program, "a_Amp");
	glEnableVertexAttribArray(ampLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glVertexAttribPointer(ampLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int periodLoc = glGetAttribLocation(program, "a_Period");
	glEnableVertexAttribArray(periodLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glVertexAttribPointer(periodLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int thetaLoc = glGetAttribLocation(program, "a_Theta");
	glEnableVertexAttribArray(thetaLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleThetaVBO);
	glVertexAttribPointer(thetaLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);


	int timeLoc = glGetUniformLocation(program, "u_Time");
	glUniform1f(timeLoc, g_time);

	int accelLoc = glGetUniformLocation(program, "u_Accel");
	glUniform3f(accelLoc, 0.f, -2.28f, 0.0f);
	g_time += 0.0001f;

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVertexCount);
	// 그리는 버텍스 수는 어레이 0번지부터 3번지 까지 읽어 오겠다. m_testVBO1에 몇개가 들어가는가를 의미

	glDisable(GL_BLEND);
}

void Renderer::DrawFragmentSandbox()
{
	GLuint shader = m_FragmentSandboxShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	int texLoc = glGetAttribLocation(shader, "a_Texcoord");

	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT,
		GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT,
		GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));


	/// <적군 위치>
	int Enemypos = glGetUniformLocation(shader, "u_EnemyPos");
	glUniform2f(Enemypos, 0.4f, 0.4f);

	int Enemyposs = glGetUniformLocation(shader, "u_EnemyPoss");
	float v_Enemyposs[] = { 0.0,0.0,0.5,0.5,0.3,0.3 };
	glUniform2fv(Enemyposs, 3, v_Enemyposs);

	int Utime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(Utime, g_time);
	g_time += 0.001f;

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
}

void Renderer::GetGLPosition(float x, float y, float* newX, float* newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::Class0310()
{
	float vertices[] = { 0, 0, 0,
						0, 1, 0,
						1, 1, 0 };

	glGenBuffers(1, &m_testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		GL_STATIC_DRAW);

	float vertices1[] = { -1, -1, 0,
						0, -1, 0,
						0, 0, 0 };
	glGenBuffers(1, &m_testVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1,
		GL_STATIC_DRAW);

	float color[] = { 1, 0, 0, 1,
					0, 1, 0, 1,
					0, 0, 1, 1 };

	glGenBuffers(1, &m_ColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color,
		GL_STATIC_DRAW);

	//int size = 400'000'000'000;
	//float* testTemp = new float[size];
	//memset(testTemp, 1, sizeof(float) * size);

	//GLuint testVBO1 = 0;
	//glGenBuffers(1, &testVBO1);
	//glBindBuffer(GL_ARRAY_BUFFER, testVBO1);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, testTemp,
	//	GL_STATIC_DRAW);
}

void Renderer::CreateParticleVBO(int numParticleCount)
{
	int vertexCount = 6;	// 파티클당 필요한 버텍스의 수
	int particleCount = numParticleCount;	// 파티클의 수
	int floatCount = 3;		// 한 Vertex에 필요한 float의 수
	int totalFloatCount = particleCount * vertexCount * floatCount;
	int totalFloatCountSingle = particleCount * vertexCount * 1;
	int totalFloatCountFour = particleCount * vertexCount * 4;
	m_ParticleVertexCount = particleCount * vertexCount;

	float* vertices = NULL;
	vertices = new float[totalFloatCount];

	float particleSize = 0.5f;

	int index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.0f;// 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		float particleCenterY = 0.0f;// 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);

		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0.0f; index++;

		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0.0f; index++;

		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0.0f; index++;

		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY + particleSize; index++;
		vertices[index] = 0.0f; index++;

		vertices[index] = particleCenterX - particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0.0f; index++;

		vertices[index] = particleCenterX + particleSize; index++;
		vertices[index] = particleCenterY - particleSize; index++;
		vertices[index] = 0.0f; index++;

	}

	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCount * sizeof(float), vertices, GL_STATIC_DRAW);
	delete[] vertices;

	float* verticesVel = NULL;
	verticesVel = new float[totalFloatCount];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float xSpeed = 2.f * ((float)rand() / (float)RAND_MAX - 0.5f);
		float ySpeed = 5.f * ((float)rand() / (float)RAND_MAX);

		for (int j = 0; j < 6; j++)
		{
			verticesVel[index++] = xSpeed;
			verticesVel[index++] = ySpeed;
			verticesVel[index++] = 0.0f;
		}
	}

	glGenBuffers(1, &m_ParticleVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCount * sizeof(float), verticesVel,
		GL_STATIC_DRAW);

	delete[] verticesVel;
	//EmitTime
	float* verticesEmitTime = NULL;
	verticesEmitTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float emitTime = 10.f * ((float)rand() / (float)RAND_MAX);
		for (int j = 0; j < 6; j++)
		{
			verticesEmitTime[index++] = emitTime;
		}
	}
	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountSingle * sizeof(float), verticesEmitTime,
		GL_STATIC_DRAW);
	delete[] verticesEmitTime;

	//LifeTime
	float* verticesLifeTime = NULL;
	verticesLifeTime = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float lifeTime = 2.f * ((float)rand() / (float)RAND_MAX);
		for (int j = 0; j < 6; j++)
		{
			verticesLifeTime[index++] = lifeTime;
		}
	}
	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountSingle * sizeof(float), verticesLifeTime,
		GL_STATIC_DRAW);
	delete[] verticesLifeTime;

	//Amp
	float* verticesAmp = NULL;
	verticesAmp = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float amp = (2.f * ((float)rand() / (float)RAND_MAX) - 1.f);
		for (int j = 0; j < 6; j++)
		{
			verticesAmp[index++] = amp;
		}
	}
	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountSingle * sizeof(float), verticesAmp,
		GL_STATIC_DRAW);
	delete[] verticesAmp;

	//Period
	float* verticesPeriod = NULL;
	verticesPeriod = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float period = 1.f * ((float)rand() / (float)RAND_MAX);
		for (int j = 0; j < 6; j++)
		{
			verticesPeriod[index++] = period;
		}
	}
	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountSingle * sizeof(float), verticesPeriod,
		GL_STATIC_DRAW);
	delete[] verticesPeriod;

	//theta
	float* verticestheta = NULL;
	verticestheta = new float[totalFloatCountSingle];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float period = 1.f * ((float)rand() / (float)RAND_MAX);
		for (int j = 0; j < 6; j++)
		{
			verticestheta[index++] = period;
		}
	}
	glGenBuffers(1, &m_ParticleThetaVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleThetaVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountSingle * sizeof(float), verticestheta,
		GL_STATIC_DRAW);
	delete[] verticestheta;

	//Color
	float* verticesColor = NULL;
	verticesColor = new float[totalFloatCountFour];

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float Rcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Gcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Bcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Acolor = 1.f;
		for (int j = 0; j < 6; ++j)
		{
			verticesColor[index++] = Rcolor;
			verticesColor[index++] = Gcolor;
			verticesColor[index++] = Bcolor;
			verticesColor[index++] = Acolor;
		}
	}
	glGenBuffers(1, &m_ParticleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountFour * sizeof(float), verticesColor,
		GL_STATIC_DRAW);
	delete[] verticesColor;


	// pos+color vbo m_ParticlePosColVBO;
	int totalFloatCountPosColUV = numParticleCount * 6 * (3 + 4+2);
	float* verticesPosColorUV = NULL;
	verticesPosColorUV = new float[totalFloatCountPosColUV];

	particleSize = 0.01f;

	index = 0;
	for (int i = 0; i < numParticleCount; ++i)
	{
		float particleCenterX = 0.0f;// 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		float particleCenterY = 0.0f;// 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		float Rcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Gcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Bcolor = 1.f * ((float)rand() / (float)RAND_MAX);
		float Acolor = 1.f * ((float)rand() / (float)RAND_MAX);

		// V0
		verticesPosColorUV[index++] = particleCenterX - particleSize;
		verticesPosColorUV[index++] = particleCenterY + particleSize;
		verticesPosColorUV[index++] = 0.0f;//xyz
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;//Rgba
		verticesPosColorUV[index++] = 0.f;
		verticesPosColorUV[index++] = 0.f;//UV //v1

		// V1
		verticesPosColorUV[index++] = particleCenterX - particleSize;
		verticesPosColorUV[index++] = particleCenterY - particleSize;
		verticesPosColorUV[index++] = 0.0f;
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;
		verticesPosColorUV[index++] = 0.f;
		verticesPosColorUV[index++] = 1.f;//UV //v2

		// V2
		verticesPosColorUV[index++] = particleCenterX + particleSize;
		verticesPosColorUV[index++] = particleCenterY + particleSize;
		verticesPosColorUV[index++] = 0.0f;
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;
		verticesPosColorUV[index++] = 1.f;
		verticesPosColorUV[index++] = 0.f;//UV //v3

		// V3
		verticesPosColorUV[index++] = particleCenterX + particleSize;
		verticesPosColorUV[index++] = particleCenterY + particleSize;
		verticesPosColorUV[index++] = 0.0f;
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;
		verticesPosColorUV[index++] = 1.f;
		verticesPosColorUV[index++] = 0.f;//UV //v4

		verticesPosColorUV[index++] = particleCenterX - particleSize;
		verticesPosColorUV[index++] = particleCenterY - particleSize;
		verticesPosColorUV[index++] = 0.0f;
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;
		verticesPosColorUV[index++] = 0.f;
		verticesPosColorUV[index++] = 1.f;//UV //v1

		verticesPosColorUV[index++] = particleCenterX + particleSize;
		verticesPosColorUV[index++] = particleCenterY - particleSize;
		verticesPosColorUV[index++] = 0.0f;
		verticesPosColorUV[index++] = Rcolor;
		verticesPosColorUV[index++] = Gcolor;
		verticesPosColorUV[index++] = Bcolor;
		verticesPosColorUV[index++] = Acolor;
		verticesPosColorUV[index++] = 1.f;
		verticesPosColorUV[index++] = 1.f;//UV //v1

	}

	glGenBuffers(1, &m_ParticlePosColUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColUVVBO);
	glBufferData(GL_ARRAY_BUFFER, totalFloatCountPosColUV * sizeof(float), verticesPosColorUV,
		GL_STATIC_DRAW);
	delete[] verticesPosColorUV;

}
