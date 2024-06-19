#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include "SimText3D.h"
#pragma comment(lib,"glew32.lib")

#include <windows.h>
//#include "output.h"
//#include <GL/glu.h>

/*
GLenum texture_target=GL_TEXTURE_RECTANGLE_ARB;
GLenum internal_format=GL_RGBA32F_ARB;
GLenum texture_format=GL_RGBA;
*/
//void checkGLErrors(const char* label);

void SimText3D::setupTexture(unsigned int texID)
{
	glBindTexture(texture_target, texID);

	glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}



void SimText3D::transferToTexture(GLuint texID)
{
	if (texID == -1)texID = texture;
	glBindTexture(texture_target, texID);

	//static PFNGLTEXIMAGE3DPROC glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
	//static PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");



	if (dim_num == 1)glTexImage1D(texture_target, 0, internal_format, width, 0, texture_format, t_type, arr);
	if (dim_num == 2)glTexImage2D(texture_target, 0, internal_format, width, height, 0, texture_format, t_type, arr);
	if (dim_num == 3)
	{
		glTexImage3D(texture_target, 0, internal_format, width, height, depth, 0, texture_format, t_type, 0);

		glTexSubImage3D(texture_target, 0, 0, 0, 0, width, height, depth, texture_format, t_type, arr);
	}
	GLenum errCode;
	const GLubyte* errString;
	errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		//output::general.println((char*)errString);
	}

}

SimText3D::SimText3D(int dim_num1, int w, int h, int d, int pixel_size, void* data, int n_ID_to_use, bool n_linear, int nt_type) :linear(n_linear), t_type(nt_type), dim_num(dim_num1)
{

	ID_to_use = 1;//n_ID_to_use;//GetFreeID();
	glActiveTexture(GL_TEXTURE0 + ID_to_use);

	if (dim_num == 1)texture_target = GL_TEXTURE_1D;
	if (dim_num == 2)texture_target = GL_TEXTURE_2D;
	if (dim_num == 3)texture_target = GL_TEXTURE_3D;

	//glEnable(texture_target);
	arr = data;
	internal_format = GL_RED;
	texture_format = GL_RED;

	width = w;
	height = h;
	depth = d;

	glGenTextures(1, &texture);

	setupTexture(texture);


	transferToTexture(texture);


	//glDisable(texture_target);
	glActiveTexture(GL_TEXTURE0);
};

SimText3D::~SimText3D()
{
	glDeleteTextures(1, &texture);
}

unsigned int SimText3D::SetID(int id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(texture_target, texture);
	return id;
}
