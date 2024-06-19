#ifndef SEGMENTATION_H
#define SEGMENTATION_H

//#include "vec3.h"
//#include "ivec3.h"
#include "VolumeData.h"

//struct CutAction3D
//{
//	CutAction3D(){}
//	CutAction3D(v2vec _cutting_contour, Camera& _cam) :
//		cutting_contour(_cutting_contour),
//		cam(_cam)	{}
//	v2vec cutting_contour;
//	Camera cam;
//};

class SegmentationMask3D
{
public:
	SegmentationMask3D();
	~SegmentationMask3D();
	void AssignData(VData* _volume_data);
	void CutByCounter(std::vector<glm::vec2> cutting_contour);//, Camera& cam, int delta_value);

	void Renull();
	void Undo();
	bool IsUsed();

	glm::vec3 GetCellSize();
	//VData* GetMaskData();
	//std::vector<CutAction3D> getActions() { return cut_actions; }
	//void setActions(std::vector<CutAction3D> actions);
	glm::vec3 GetSize(){ return mask_size; }
	unsigned char* GetMaskData();
private:
	bool is_inited();
	
	//std::vector<CutAction3D> cut_actions;
	glm::vec3 cell_size;
	bool need_init;
	unsigned char* mask_data;
	glm::vec3 mask_spacing;
	glm::vec3 mask_size;

	VData* volume_data;
	float scale_3d = 0.002;//mm

};

//extern v2vec cutting_contour;
extern std::vector<glm::vec2> cutting_contour;


#endif