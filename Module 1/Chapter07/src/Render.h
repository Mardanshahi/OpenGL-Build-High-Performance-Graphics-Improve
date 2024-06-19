#ifndef ISO_H
#define ISO_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
//#include "Camera.h"
//#include "SimText.h"
#include "SimText3D.h"
//#include <ivec3.h>
//#include <vec.h>
//#include "RenderingMethod.h"

//class ShaderProgram;

/*
#define BRICK_SIZE 256
#define MAX_DATA_SIZE_X 512
#define MAX_DATA_SIZE_Y 512
#define MAX_DATA_SIZE_Z 512*4
*/
#define MAX_DATA_SIZE_X 512*2
#define MAX_DATA_SIZE_Y 512*2
#define MAX_DATA_SIZE_Z 512*6
#define BRICK_SIZE 256

#define VOLUME_DIV_X (MAX_DATA_SIZE_X/BRICK_SIZE)
#define VOLUME_DIV_Y (MAX_DATA_SIZE_Y/BRICK_SIZE)
#define VOLUME_DIV_Z (MAX_DATA_SIZE_Z/BRICK_SIZE)
#define VOLUME_NUM (VOLUME_DIV_X*VOLUME_DIV_Y*VOLUME_DIV_Z)


#define BRICKS_OVERLAP 2

#define TXT_SCREEN		0
#define TXT_TF			1
#define TXT_VD			2
#define TXT_SCREEN2		3
#define TXT_SCREEN3		4
#define TXT_DISTANCE_MAP	5
#define TXT_ACC	6
//#define TXT_MIN 7

enum
{
	RENDERING_MODE_DVR=0,
	RENDERING_MODE_wlDVR=1,
	RENDERING_MODE_SSD=2,
	RENDERING_MODE_MIP=3,
	RENDERING_MODE_COLORED_SURFACE=4,
	RENDERING_MODE_MinIP=5
};

class IsoViewer
{
public:

	//IsoViewer();
	~IsoViewer();
	

	void ClearST();
	//void ApplyCamera(Camera* c);

	void UploadFieldData(unsigned short**data, glm::vec3 size, glm::vec3 spacing);

	void UploadAccStr(unsigned char*data, glm::vec3 size);
	//void UploadMin(unsigned short**data,ivec3 size);

	//void Draw(Camera* c);

	void SetBoundingBox(glm::vec3 a, glm::vec3 b);
	glm::vec3 GetMinBox(){return b1;}
	glm::vec3 GetMaxBox(){return b2;}
	glm::vec2 GetCurBounds();
	
	void ReloadShader();
	
	void DragWindow(glm::vec2 w0, glm::vec2 w1);
	


	void FitBoundingBox();
	void MakeBrick(unsigned short**data, glm::vec3 size, glm::vec3 pos, glm::vec3 b_size,int brick_id);
	bool draw_frame_is;
	
//	void UploadColorTable();
	glm::vec4* GetCurTF();
	
	glm::vec2 GetTFWW();
	

	void SetRM(int rm);//0-dvr 1-mip 2-ssd
	int frames_acc;	

	//private:
	//void ClearRMs();
	
	void DrawSubBrick(glm::vec3 bb1, glm::vec3 bb2);
	//RenderingMethod*GetCurRM();
	//RenderingMethod*GetActRM();


	int DataValueKoef; 
	glm::vec3 anag[3];


	SimText3D* st_i;//, *st_min;

	SimText3D* st[VOLUME_NUM];
	
	unsigned short max_val[VOLUME_NUM];
	glm::vec3 box1[VOLUME_NUM];
	glm::vec3 box2[VOLUME_NUM];
	glm::vec3 txt_box1[VOLUME_NUM];
	glm::vec3 txt_box2[VOLUME_NUM];
	std::vector<int> max_x[VOLUME_NUM];
	std::vector<int> max_y[VOLUME_NUM];
	std::vector<int> max_z[VOLUME_NUM];
	glm::vec3 block_size_arr[VOLUME_NUM];

	//std::vector<RenderingMethod*> rendering_methods;

	glm::vec3 b1,b2;
//
	int frames_to_accumulate,cur_rendering_method,active_rendering_method;

	long int occ_mem;
	float occ_space;

	bool need_reload_acc;
	
	glm::vec3 size;
	glm::vec3 data_spacing;
	int data_type;//0-short 1-byte
	float Threshold;

	int bi, bj, bk;

};



#endif