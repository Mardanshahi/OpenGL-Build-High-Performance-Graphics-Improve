#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "..\src\GLSLShader.h"
#include <fstream>


#include <string.h>
#include <vector>
#include "Tables.h"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);


//#pragma comment(lib, "glew32.lib")

using namespace std;






//our vertex struct stores the position and normals
struct Vertex {
	glm::vec3 pos, normal;
};

//TetrahedraMarcher class
class TetrahedraMarcher
{
public:
	//constructor/destructor
	TetrahedraMarcher(void);
	~TetrahedraMarcher(void);

	//function to set the volume dimension
	void SetVolumeDimensions(const int xdim, const int ydim, const int zdim);

	//function to set the total number of sampling voxels
	//more voxels will give a higher density mesh
	void SetNumSamplingVoxels(const int x, const int y, const int z);

	//set the isosurface value
	void SetIsosurfaceValue(const GLubyte value);

	//load the volume dataset
	bool LoadVolume(const std::string& filename);

	//march the volume dataset
	void MarchVolume();

	//get the total number of vertices generated
	size_t GetTotalVertices();

	//get the pointer to the vertex buffer
	Vertex* GetVertexPointer();

protected:
	//volume sampling function, give the x,y,z values returns the density value 
	//in the volume at that location
	GLubyte SampleVolume(const int x, const int y, const int z);

	//get the normal at the given location using center finite difference approximation
	glm::vec3 GetNormal(const int x, const int y, const int z);

	//samples a voxel at the given location and scale
	void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale);

	//returns the offset between the two sample values
	float GetOffset(const GLubyte v1, const GLubyte v2);

	//the volume dataset dimensions and inverse volume dimensions
	int XDIM, YDIM, ZDIM;
	glm::vec3 invDim;

	//sampling distances in voxels
	int X_SAMPLING_DIST;
	int Y_SAMPLING_DIST;
	int Z_SAMPLING_DIST;

	//volume data pointer
	GLubyte* pVolume;

	//the given isovalue to look for
	GLubyte isoValue;

	//vertices vector storing positions and normals
	std::vector<Vertex> vertices;
};







//set screen size
const int WIDTH  = 1280;
const int HEIGHT = 720;

//camera transform variables
int state = 0, oldX=0, oldY=0;
float rX=4, rY=50, dist = -2;

//grid object
#include "..\src\Grid.h"
CGrid* grid;

//modelview and projection matrices
glm::mat4 MV,P;
 
//flag to set wireframe rendering mode
bool bWireframe = false;

//volume marcher vertex array and vertex buffer object IDs
GLuint volumeMarcherVBO;
GLuint volumeMarcherVAO;

//shader
GLSLShader shader;

//background colour
glm::vec4 bg=glm::vec4(0.5,0.5,1,1);

//volume filename
const std::string volume_file = "../media/Engine256.raw";
 
//TetrahedraMarcher instance
#include "Tables.h"
TetrahedraMarcher* marcher;

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
}

//mouse move event handler
void OnMouseMove(int x, int y)
{
	if (state == 0) {
		dist += (y - oldY)/50.0f;
	} else {
		rX += (y - oldY)/50.0f;
		rY += (x - oldX)/50.0f;
	}
	oldX = x;
	oldY = y;

	glutPostRedisplay();
}
 
//OpenGL initialization function
void OnInit() {

	GL_CHECK_ERRORS

	//create a uniform grid of size 20x20 in XZ plane
	grid = new CGrid(20,20);

	GL_CHECK_ERRORS

	//create a new TetrahedraMarcher instance
	marcher = new TetrahedraMarcher();
	//set the volume dataset dimensions
	marcher->SetVolumeDimensions(256,256,256);
	//load the volume dataset
	marcher->LoadVolume(volume_file);
	//set the isosurface value
	marcher->SetIsosurfaceValue(48);
	//set the number of sampling voxels 
	marcher->SetNumSamplingVoxels(128,128,128);
	//begin tetrahedra marching
	marcher->MarchVolume();

	//setup the volume marcher vertex array object and vertex buffer object
	glGenVertexArrays(1, &volumeMarcherVAO);
	glGenBuffers(1, &volumeMarcherVBO);
	glBindVertexArray(volumeMarcherVAO);
	glBindBuffer (GL_ARRAY_BUFFER, volumeMarcherVBO);

	//pass the obtained vertices from the tetrahedra marcher and pass to the
	//buffer object memory
	glBufferData (GL_ARRAY_BUFFER, marcher->GetTotalVertices()*sizeof(Vertex), marcher->GetVertexPointer(), GL_STATIC_DRAW);

	//enable vertex attribute array for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex),0);

	//enable vertex attribute array for normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex),(const GLvoid*)offsetof(Vertex, normal));

	GL_CHECK_ERRORS

	//load the shader 
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/marcher.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/marcher.frag");

	//compile and link the shader program
	shader.CreateAndLinkProgram();
	shader.Use();
		//add attribute and uniform
		shader.AddAttribute("vVertex");
		shader.AddAttribute("vNormal");
		shader.AddUniform("MVP");
	shader.UnUse();

	GL_CHECK_ERRORS

	//set the background colour
	glClearColor(bg.r, bg.g, bg.b, bg.a);

	//enable depth test and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	cout<<"Initialization successfull"<<endl;
}

//release all allocated resources
void OnShutdown() {
	shader.DeleteShaderProgram();
	glDeleteVertexArrays(1, &volumeMarcherVAO);
	glDeleteBuffers(1, &volumeMarcherVBO);

	delete grid;
	delete marcher;
	cout<<"Shutdown successfull"<<endl;
}

//resize event handler
void OnResize(int w, int h) {
	//set the viewport
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	//setup the projection matrix
	P = glm::perspective(7.0f,(float)w/h, 0.1f,1000.0f);
}

//display callback function
void OnRender() {
	GL_CHECK_ERRORS
	//set the camera transform
	glm::mat4 Tr	= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx	= glm::rotate(Tr,  rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV    = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f));

	//clear the colour and depth buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//get the combined modelview projection matrix
    glm::mat4 MVP	= P*MV;

	//render the grid object
	grid->Render(glm::value_ptr(MVP));

	//set the modelling transform to move the marhing result to origin
	glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(-0.5,-0.5,-0.5));

	//if rendering mode set to wireframe we set the front and back 
	//polygon mode to line
	if(bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//set the volume marcher vertex array object
	glBindVertexArray(volumeMarcherVAO);
		//bind the shader
		shader.Use();
			//set the shader uniforms
			glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP*T));
				//render the triangles
				glDrawArrays(GL_TRIANGLES, 0, marcher->GetTotalVertices());
		//unbind the shader
		shader.UnUse();
	
	//restore the default polygon mode
	if(bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//swap front and back buffers to show the rendered result
	glutSwapBuffers();
}

//keyboard function to change the wireframe rendering mode
void OnKey(unsigned char key, int x, int y) {
	switch(key) {
		case 'w': 	bWireframe = !bWireframe;	break; 
	}
	//recall display function
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	//freeglut initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion (3, 3);
	glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Iso-surface extraction using Marching Tetrahedra - OpenGL 3.3");

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

	//attach callbacks
	glutCloseFunc(OnShutdown);
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKey);

	//run main loop
	glutMainLoop();

	return 0;
}















TetrahedraMarcher::TetrahedraMarcher(void)
{
	XDIM = 256;
	YDIM = 256;
	ZDIM = 256;
	pVolume = NULL;

}

TetrahedraMarcher::~TetrahedraMarcher(void)
{
	if (pVolume != NULL) {
		delete[] pVolume;
		pVolume = NULL;
	}
}

void TetrahedraMarcher::SetVolumeDimensions(const int xdim, const int ydim, const int zdim) {
	XDIM = xdim;
	YDIM = ydim;
	ZDIM = zdim;
	invDim.x = 1.0f / XDIM;
	invDim.y = 1.0f / YDIM;
	invDim.z = 1.0f / ZDIM;
}
void TetrahedraMarcher::SetNumSamplingVoxels(const int x, const int y, const int z) {
	X_SAMPLING_DIST = x;
	Y_SAMPLING_DIST = y;
	Z_SAMPLING_DIST = z;
}
void TetrahedraMarcher::SetIsosurfaceValue(const GLubyte value) {
	isoValue = value;
}

bool TetrahedraMarcher::LoadVolume(const std::string& filename) {
	std::ifstream infile(filename.c_str(), std::ios_base::binary);

	if (infile.good()) {
		pVolume = new GLubyte[XDIM * YDIM * ZDIM];
		infile.read(reinterpret_cast<char*>(pVolume), XDIM * YDIM * ZDIM * sizeof(GLubyte));
		infile.close();
		return true;
	}
	else {
		return false;
	}
}

void TetrahedraMarcher::SampleVoxel(const int x, const int y, const int z, glm::vec3 scale) {
	GLubyte cubeCornerValues[8];
	int flagIndex, edgeFlags, i;
	glm::vec3 edgeVertices[12];
	glm::vec3 edgeNormals[12];

	//Make a local copy of the values at the cube's corners
	for (i = 0; i < 8; i++) {
		cubeCornerValues[i] = SampleVolume(x + (int)(a2fVertexOffset[i][0] * scale.x),
			y + (int)(a2fVertexOffset[i][1] * scale.y),
			z + (int)(a2fVertexOffset[i][2] * scale.z));
	}

	//Find which vertices are inside of the surface and which are outside
	//Obtain a flagIndex based on if the value at the cube vertex is less 
	//than the given isovalue
	flagIndex = 0;
	for (i = 0; i < 8; i++) {
		if (cubeCornerValues[i] <= isoValue)
			flagIndex |= 1 << i;
	}

	//Find which edges are intersected by the surface
	edgeFlags = aiCubeEdgeFlags[flagIndex];

	//If the cube is entirely inside or outside of the surface, then there will be no intersections
	if (edgeFlags == 0)
	{
		return;
	}

	//for all edges
	for (i = 0; i < 12; i++)
	{
		//if there is an intersection on this edge
		if (edgeFlags & (1 << i))
		{
			//get the offset 
			float offset = GetOffset(cubeCornerValues[a2iEdgeConnection[i][0]], cubeCornerValues[a2iEdgeConnection[i][1]]);

			//use offset to get the vertex position
			edgeVertices[i].x = x + (a2fVertexOffset[a2iEdgeConnection[i][0]][0] + offset * a2fEdgeDirection[i][0]) * scale.x;
			edgeVertices[i].y = y + (a2fVertexOffset[a2iEdgeConnection[i][0]][1] + offset * a2fEdgeDirection[i][1]) * scale.y;
			edgeVertices[i].z = z + (a2fVertexOffset[a2iEdgeConnection[i][0]][2] + offset * a2fEdgeDirection[i][2]) * scale.z;

			//use the vertex position to get the normal
			edgeNormals[i] = GetNormal((int)edgeVertices[i].x, (int)edgeVertices[i].y, (int)edgeVertices[i].z);
		}
	}

	//Draw the triangles that were found.  There can be up to five per cube
	for (i = 0; i < 5; i++)
	{
		if (a2iTriangleConnectionTable[flagIndex][3 * i] < 0)
			break;

		for (int j = 0; j < 3; j++)
		{
			int vertex = a2iTriangleConnectionTable[flagIndex][3 * i + j];
			Vertex v;
			v.normal = (edgeNormals[vertex]);
			v.pos = (edgeVertices[vertex]) * invDim;
			vertices.push_back(v);
		}
	}
}

void TetrahedraMarcher::MarchVolume() {
	vertices.clear();
	int dx = XDIM / X_SAMPLING_DIST;
	int dy = YDIM / Y_SAMPLING_DIST;
	int dz = ZDIM / Z_SAMPLING_DIST;
	glm::vec3 scale = glm::vec3(dx, dy, dz);
	for (int z = 0; z < ZDIM; z += dz) {
		for (int y = 0; y < YDIM; y += dy) {
			for (int x = 0; x < XDIM; x += dx) {
				SampleVoxel(x, y, z, scale);
			}
		}
	}
}

size_t TetrahedraMarcher::GetTotalVertices() {
	return vertices.size();
}
Vertex* TetrahedraMarcher::GetVertexPointer() {
	return  &vertices[0];
}

GLubyte TetrahedraMarcher::SampleVolume(const int x, const int y, const int z) {
	int index = (x + (y * XDIM)) + z * (XDIM * YDIM);
	if (index < 0)
		index = 0;
	if (index >= XDIM * YDIM * ZDIM)
		index = (XDIM * YDIM * ZDIM) - 1;
	return pVolume[index];
}

glm::vec3 TetrahedraMarcher::GetNormal(const int x, const int y, const int z) {
	glm::vec3 N;
	N.x = (SampleVolume(x - 1, y, z) - SampleVolume(x + 1, y, z)) * 0.5f;
	N.y = (SampleVolume(x, y - 1, z) - SampleVolume(x, y + 1, z)) * 0.5f;
	N.z = (SampleVolume(x, y, z - 1) - SampleVolume(x, y, z + 1)) * 0.5f;
	return glm::normalize(N);
}
float TetrahedraMarcher::GetOffset(const GLubyte v1, const GLubyte v2) {
	float delta = (float)(v2 - v1);
	if (delta == 0)
		return 0.5f;
	else
		return (isoValue - v1) / delta;
}










