#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

//for floating point inaccuracy
const float EPSILON = 0.0001f;

#pragma comment(lib, "glew32.lib")

using namespace std;

//screen dimensions
const int WIDTH  = 1280;
const int HEIGHT = 960;

//camera transform variables
int state = 0, oldX=0, oldY=0;
float rX=4, rY=50, dist = -2;

//grid object
#include "..\src\Grid.h"
CGrid* grid;

//modelview and projection matrices
glm::mat4 MV,P;

//volume vertex array and buffer objects
GLuint volumeVBO;
GLuint volumeVAO;


//maximum number of slices
const int MAX_SLICES = 512;

//sliced vertices
glm::vec3 vTextureSlices[MAX_SLICES*12];

//background colour
glm::vec4 bg=glm::vec4(0.5,0.5,1,1);

//volume data files
const std::string volume_file = "../media/vista-ct.raw";
bool is16bit = true;

//dimensions of volume data
const int XDIM = 512;
const int YDIM = 512;
const int ZDIM = 54;

//total number of slices current used
int num_slices =  256;

//OpenGL volume texture id
GLuint textureID;

//transfer function (lookup table) texture id
GLuint tfTexID;

//flag to see if the view is rotated
//volume is resliced if the view is rotated
bool bViewRotated = false;

//unit cube vertices
glm::vec3 vertexList[8] = {glm::vec3(-0.5,-0.5,-0.5),
						   glm::vec3( 0.5,-0.5,-0.5),
						   glm::vec3(0.5, 0.5,-0.5),
						   glm::vec3(-0.5, 0.5,-0.5),
						   glm::vec3(-0.5,-0.5, 0.5),
						   glm::vec3(0.5,-0.5, 0.5),
						   glm::vec3( 0.5, 0.5, 0.5),
						   glm::vec3(-0.5, 0.5, 0.5)};

//unit cube edges
int edgeList[8][12] = {
	{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
	{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
	{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
	{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
	{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
	{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
	{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
	{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
};
const int edges[12][2]= {{0,1},{1,2},{2,3},{3,0},{0,4},{1,5},{2,6},{3,7},{4,5},{5,6},{6,7},{7,4}};

//transfer function (lookup table) colour values
const glm::vec4 jet_values[4] = {
								glm::vec4(0.5, 0.3, 0.0, 0.0),
								glm::vec4(0.5, 0.1, 0.0, 1.0),
								glm::vec4(0.0, 0.2, 0.5, 1.0),
								glm::vec4(0.0, 0.3, 0.5, 0.0),
								 };

//current viewing direction
glm::vec3 viewDir;

//function that load a volume from the given raw data file and generates an OpenGL 3D texture from it
bool LoadVolume() {
	std::ifstream infile(volume_file.c_str(), std::ios_base::binary);

	if(infile.good()) {
		//read the volume data file
		GLubyte* pData = new GLubyte[XDIM*YDIM*ZDIM];
		infile.read(reinterpret_cast<char*>(pData), XDIM*YDIM*ZDIM*sizeof(GLubyte));
		infile.close();

		//generate OpenGL texture
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);

		// set the texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//set the mipmap levels (base and max)
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

		//allocate data with internal format and foramt as (GL_RED)		
		glTexImage3D(GL_TEXTURE_3D,0,GL_RED,XDIM,YDIM,ZDIM,0,GL_RED,GL_UNSIGNED_BYTE,pData);

		std::cout << glGetError() << std::endl;
		GL_CHECK_ERRORS

		//generate mipmaps
		glGenerateMipmap(GL_TEXTURE_3D);

		//delete the volume data allocated on heap
		delete [] pData;

		return true;
	} else {
		return false;
	}
}

bool LoadVolumeUShort() {
	std::ifstream infile(volume_file.c_str(), std::ios_base::binary);

	if (infile.good()) {
		//read the volume data file
		GLushort* pData = new GLushort[XDIM * YDIM * ZDIM];
		infile.read(reinterpret_cast<char*>(pData), XDIM * YDIM * ZDIM * sizeof(GLushort));
		infile.close();

		//generate OpenGL texture
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);

		// set the texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//set the mipmap levels (base and max)
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

		//allocate data with internal format and foramt as (GL_RED)		
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, XDIM, YDIM, ZDIM, 0, GL_RED, GL_UNSIGNED_SHORT, pData);

		std::cout << glGetError() << std::endl;
		GL_CHECK_ERRORS

			//generate mipmaps
			glGenerateMipmap(GL_TEXTURE_3D);

		//delete the volume data allocated on heap
		delete[] pData;

		return true;
	}
	else {
		return false;
	}
}
//function to generate interpolated colours from the set of colour values (jet_values)
//this function first calculates the amount of increments for each component and the
//index difference. Then it linearly interpolates the adjacent values to get the 
//interpolated result.
void LoadTransferFunction() {
	float pData[256][4];
	int indices[4];

	//fill the colour values at the place where the colour should be after interpolation
	// index must be below 256
	int index0[] = {130, 131, 140, 141};
		
		for (int i = 0; i < 4; i++)
		{
			pData[index0[i]][0] = jet_values[i].x;
			pData[index0[i]][1] = jet_values[i].y;
			pData[index0[i]][2] = jet_values[i].z;
			pData[index0[i]][3] = jet_values[i].w;
			indices[i] = index0[i];

		}


	//for each adjacent pair of colours, find the difference in the rgba values and then interpolate
	for(int j=0;j<4-1;j++)
	{
		float dDataR = (pData[indices[j+1]][0] - pData[indices[j]][0]);
		float dDataG = (pData[indices[j+1]][1] - pData[indices[j]][1]);
		float dDataB = (pData[indices[j+1]][2] - pData[indices[j]][2]);
		float dDataA = (pData[indices[j+1]][3] - pData[indices[j]][3]);
		int dIndex = indices[j+1]-indices[j];

		float dDataIncR = dDataR/float(dIndex);
		float dDataIncG = dDataG/float(dIndex);
		float dDataIncB = dDataB/float(dIndex);
		float dDataIncA = dDataA/float(dIndex);
		for(int i=indices[j]+1;i<indices[j+1];i++)
		{
			pData[i][0] = (pData[i-1][0] + dDataIncR);
			pData[i][1] = (pData[i-1][1] + dDataIncG);
			pData[i][2] = (pData[i-1][2] + dDataIncB);
			pData[i][3] = (pData[i-1][3] + dDataIncA);
		}
	}

	//generate the OpenGL texture
	glGenTextures(1, &tfTexID);
	//bind this texture to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, tfTexID);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//allocate the data to texture memory. Since pData is on stack, we donot delete it 
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,pData);
	
	GL_CHECK_ERRORS
}

//mouse down event handler
void OnMouseDown(int button, int s, int x, int y)
{
	if (s == GLUT_DOWN)
	{
		oldX = x;
		oldY = y;
	}

	if(button == GLUT_MIDDLE_BUTTON)
		state = 0;
	else
		state = 1;

	if(s == GLUT_UP)
		bViewRotated = false;
}

//mouse move event handler
void OnMouseMove(int x, int y)
{
	if (state == 0) {
		dist += (y - oldY)/50.0f;
	} else {
		rX += (y - oldY)/50.0f;
		rY -= (x - oldX)/50.0f;
		bViewRotated = true;
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}



//OpenGL initialization
void OnInit() {

	GL_CHECK_ERRORS



	//load volume data and generate the volume texture
	if (is16bit? LoadVolumeUShort(): LoadVolume())
		std::cout<<"Volume data loaded successfully."<<std::endl;
	else {
		std::cout<<"Cannot load volume data."<<std::endl;
		exit(EXIT_FAILURE);
	}

	//load the transfer function data and generate the trasnfer function (lookup table) texture
	LoadTransferFunction();

	//set background colour
	glClearColor(bg.r, bg.g, bg.b, bg.a);

	//setup the current camera transform and get the view direction vector
	glm::mat4 T	= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx	= glm::rotate(T,  rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV    = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));

	//get the current view direction vector
	viewDir = -glm::vec3(MV[0][2], MV[1][2], MV[2][2]);

	//setup the vertex array and buffer objects
	glGenVertexArrays(1, &volumeVAO);
	glGenBuffers(1, &volumeVBO);

	glBindVertexArray(volumeVAO);
	glBindBuffer (GL_ARRAY_BUFFER, volumeVBO);

	//pass the sliced vertices vector to buffer object memory
	glBufferData (GL_ARRAY_BUFFER, sizeof(vTextureSlices), 0, GL_DYNAMIC_DRAW);

	GL_CHECK_ERRORS

	//enable vertex attribute array for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0,0);

	glBindVertexArray(0);

	//slice the volume dataset initially
	
	cout<<"Initialization successfull"<<endl;
}

//release all allocated resources
void OnShutdown() {

	glDeleteVertexArrays(1, &volumeVAO);
	glDeleteBuffers(1, &volumeVBO);

	glDeleteTextures(1, &textureID);
	glDeleteTextures(1, &tfTexID);

	delete grid;
	cout<<"Shutdown successfull"<<endl;
}

//resize event handler
void OnResize(int w, int h) {
	//setup the viewport
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	//setup the projection matrix
	P = glm::perspective(7.0f,(float)w/h, 0.1f,1000.0f);
}

//display function
void OnRender() {
	GL_CHECK_ERRORS

	//Render
	float fFrameCount = (float)data.dim.z;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.05f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// Translate and make 0.5f as the center 
	// (texture co ordinate is from 0 to 1. so center of rotation has to be 0.5f)
	glTranslatef(0.5f, 0.5f, 0.5f);

	// A scaling applied to normalize the axis 
	// (Usually the number of slices will be less so if this is not - 
	// normalized then the z axis will look bulky)
	// Flipping of the y axis is done by giving a negative value in y axis.
	// This can be achieved either by changing the y co ordinates in -
	// texture mapping or by negative scaling of y axis
	glScaled((float)data.dim.x / (float)data.dim.x,
		-1.0f * (float)data.dim.x / (float)(float)data.dim.y,
		(float)data.dim.x / (float)data.dim.z);

	// Apply the user provided transformations
	glMultMatrixd(rotationMatrix.ptr());

	glTranslatef(-0.5f, -0.5f, -0.5f);

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, data.texture);
	for (float fIndx = -1.0f; fIndx <= 1.0f; fIndx += 0.01f)
	{
		glBegin(GL_QUADS);
		MAP_3DTEXT(fIndx);
		glEnd();
	}


	//setup the camera transform
	//glm::mat4 Tr	= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, dist));
	//glm::mat4 Rx	= glm::rotate(Tr,  rX, glm::vec3(1.0f, 0.0f, 0.0f));
	//glm::mat4 MV    = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));

	////get the viewing direction
	//viewDir = -glm::vec3(MV[0][2], MV[1][2], MV[2][2]);

	////clear the colour and depth buffers
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	////get the combined modelview projection matrix
 //   glm::mat4 MVP	= P*MV;

	////render the grid object
	//grid->Render(glm::value_ptr(MVP));

	////if view is rotated, reslice the volume
	//if(bViewRotated)
	//{
	//	SliceVolume();
	//}

	////enable alpha blending (use over operator)
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	////bind volume vertex array object
	//glBindVertexArray(volumeVAO);
	////use the volume shader
	////pass the shader uniform
	////draw the triangles
	//glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices)/sizeof(vTextureSlices[0]));
	////unbind the shader

	////disable blending
	//glDisable(GL_BLEND);

	////swap front and back buffers to show the rendered result
	//glutSwapBuffers();
}

//keyboard function to change the number of slices
void OnKey(unsigned char key, int x, int y) {
	switch(key) {
		case '-':
			num_slices--;
			break;

		case '+':
			num_slices++;
			break;
	}
	//check the range of num_slices variable
	num_slices = min(MAX_SLICES, max(num_slices,3));



	//recall display function
	glutPostRedisplay();
}

int main(int argc, char** argv) {

	//freeglut initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion (3, 3);
	glutInitContextFlags (GLUT_COMPATIBILITY_PROFILE| GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Volume Rendering using 3D Texture Slicing - OpenGL 3.3");

	//glew initialization
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		cerr<<"Error: "<<glewGetErrorString(err)<<endl;
	} else {
		if (GLEW_VERSION_3_3)
		{
			cout<<"Driver supports OpenGL 3.3\nDetails:"<<endl;
		}
	}
	err = glGetError(); //this is to ignore INVALID ENUM error 1282
	GL_CHECK_ERRORS

	//output hardware information
	cout<<"\tUsing GLEW "<<glewGetString(GLEW_VERSION)<<endl;
	cout<<"\tVendor: "<<glGetString (GL_VENDOR)<<endl;
	cout<<"\tRenderer: "<<glGetString (GL_RENDERER)<<endl;
	cout<<"\tVersion: "<<glGetString (GL_VERSION)<<endl;
	cout<<"\tGLSL: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<endl;

	GL_CHECK_ERRORS
	
	//OpenGL initialization
	OnInit();

	//callback hooks
	glutCloseFunc(OnShutdown);
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKey);

	//main loop call
	glutMainLoop();

	return 0;
}























