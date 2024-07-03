#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

//#include "AllInc.h"


#include <vector>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <ctime>

#include <windows.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdlib.h>



class ShaderProgram
{
public:
	ShaderProgram(const GLchar*vs,const GLchar*fs);
	ShaderProgram(const GLchar*vs,std::string fs_code);

	~ShaderProgram();
	void Use();
	void UnUse();
//	void PrintLog();

	void SetVar(const GLchar *name,glm::vec2 val);
	void SetVar(const GLchar *name,glm::vec3 val);
	void SetVar(const GLchar *name,glm::ivec3 val);
	void SetVar(const GLchar *name,glm::vec4 val);
	void SetVar(const GLchar *name,float val);
	void SetVar(const GLchar *name,unsigned int val);
	void SetVar(const GLchar *name,int val);
	void SetMatrix3(const GLchar *name,float*m);

	void SetVar(const GLchar *name,int num,glm::vec4* val);
	void SetVar(const GLchar *name,int num,float* val);
	
	template <class T>
	void SetVar1(std::string name,T val)
	{
		SetVar(name.c_str(),val);
	}
	template <class T>
	void SetVar(std::string name,int num,T val)
	{
		SetVar(name.c_str(),num,val);
	}


private:
	GLint getUniLoc(const GLchar *name);
	GLuint h_vert;
    GLuint h_frag;
    GLuint h_program;

};

#endif