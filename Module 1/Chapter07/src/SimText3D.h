#pragma once
#ifndef SIMTEXT3D_H
#define SIMTEXT3D_H

//#include "define.h"

#include <vector>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <ctime>
#include <GL/glew.h>

//#include "Shader.h"


typedef short field_type;
#define gl_field_type GL_SHORT


class SimText3D
{
public:
	SimText3D(int dim_num1, int w, int h, int d, int pixel_size, void* data, int n_ID_to_use, bool n_linear, int nt_type);
	~SimText3D();

	int ID_to_use;

	unsigned int SetID(int id);

	int width, height, depth;

	unsigned int texture;
	//private:
	int dim_num;
	void transferToTexture(unsigned int texID = -1);

	void setupTexture(unsigned int texID);

	bool linear;

	void* arr;

	GLenum t_type;
	GLenum texture_target;
	GLenum internal_format;
	GLenum texture_format;

};

#endif