#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void Class0310();
	void Class3010_Rendering();
	void DrawParticle();
	void Moveparticle();
private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);
	void Class0317();
	void CreateParticleVBO(int nummParticleCount);
	GLuint m_ParticleShader = -1;
	GLuint m_ParticleVertex = -1;
	GLuint m_ParticleVBO = -1;
	GLuint m_ParticlevelVBO = -1;

	GLuint m_EmitTimeVBO = -1;
	GLuint m_LifeTimeVBO = -1;
	//GLuint m_EmitTimeVBO = -1;

	
	
	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO = 0;
	GLuint m_ColorVBO = 0;

	int particleCount = 0;
	float g_time = 0;

};

