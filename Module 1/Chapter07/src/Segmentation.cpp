#include "CPU_VD.h"
//#include "vec.h"
//#include "CT.h"
#include "Segmentation.h"
//#include "MyApp.h"
//#include "MainFrame.h"

//void KillBG_RC();
std::vector<glm::vec2> cutting_contour;
bool line_inside_lines(std::vector<glm::vec2>& pt2, glm::vec2 cur_pt)
{
	int cr = 0;
	for (int i = 0; i < pt2.size(); i++)
	{
		glm::vec2& p1 = pt2[i];
		glm::vec2& p2 = pt2[(i + 1) % pt2.size()];
		if (p2.y - p1.y != 0)
			if ((p1.y < cur_pt.y && p2.y >= cur_pt.y) || (p1.y >= cur_pt.y && p2.y < cur_pt.y))
			{
				//if(p2.y-p1.y==0)cr++;else
				if (p1.x + (p2.x - p1.x) * (cur_pt.y - p1.y) / (p2.y - p1.y) < cur_pt.x)cr++;
			}
	}
	return cr % 2;
}

void GetBounds(std::vector<glm::vec2>& pt2, glm::vec2& p0, glm::vec2& p1)
{
	p0 = pt2[0];
	p1 = p0;
	for (int i = 1; i<(int)pt2.size(); i++)
	{
		if (p0.x>pt2[i].x)p0.x = pt2[i].x;
		if (p0.y > pt2[i].y)p0.y = pt2[i].y;
		if (p1.x < pt2[i].x)p1.x = pt2[i].x;
		if (p1.y < pt2[i].y)p1.y = pt2[i].y;
	}
}



//void segUndo()
//{
//	CT::seg_mask.Undo();
//	if(!CT::seg_mask.IsUsed())
//		CT::iso->ReloadShader();
//	CT::SetNeedRerenderAll(1);
//	CT::iso->need_reload_acc=1;
//}
//void segUndoAll()
//{
//	if(CT::seg_mask.IsUsed())
//	{
//		CT::seg_mask.Renull();
//		CT::iso->ReloadShader();
//		CT::SetNeedRerenderAll(1);
//	}
//}

SegmentationMask3D::SegmentationMask3D() :
	need_init(0),
	mask_data(0),
	volume_data(0)
{

}
//bool SegmentationMask3D::IsUsed()
//{
//	return cut_actions.size();
//}
SegmentationMask3D::~SegmentationMask3D()
{
	Renull();
}
glm::vec3 SegmentationMask3D::GetCellSize()
{
	return cell_size;
}
void SegmentationMask3D::AssignData(VData* _volume_data)
{
	if (volume_data)return;

	volume_data = _volume_data;
	//cut_actions.clear();

	if (mask_data)
		delete mask_data;
	
	cell_size = glm::vec3(4,4,4);

	cell_size.z = 4 * volume_data->spacing.x / volume_data->spacing.z;
	if (!cell_size.z)cell_size.z++;

	mask_size = volume_data->GetSize() / cell_size;
	if (mask_size.x > 8)mask_size.x = 8; //must be multiply of 2
	if (mask_size.y > 8)mask_size.y = 8; //must be multiply of 2
	if (mask_size.z > 8)mask_size.z = 8; //must be multiply of 2

	int mask_size_bytes = mask_size.x*mask_size.y*mask_size.z;
	mask_data = new unsigned char[mask_size_bytes];
	mask_spacing = volume_data->spacing*volume_data->GetSize() / mask_size;
	memset(mask_data, 0, mask_size_bytes);
}

unsigned char* SegmentationMask3D::GetMaskData()
{
	return mask_data;
}

void SegmentationMask3D::Renull()
{
	volume_data = 0;
	//KillBG_RC();
	if (mask_data)
		delete mask_data;
	mask_data = 0;
	
	//cut_actions.clear();
}
//void SegmentationMask3D::Undo()
//{
//	//KillBG_RC();
//	if (cut_actions.size() < 2)
//	{
//		Renull();
//	}
//	else
//	{
//		CutByCounter(cut_actions[cut_actions.size() - 1].cutting_contour, cut_actions[cut_actions.size() - 1].cam, -1);
//		//cut_actions.resize(cut_actions.size() - 1);
//		cut_actions.pop_back();
//	}
//}

bool SegmentationMask3D::is_inited()
{
	return mask_data;
}

//void SegmentationMask3D::setActions(std::vector<CutAction3D> actions)
//{
//	//KillBG_RC();
//	for (int i = 0; i < actions.size(); i++)
//	{
//		CutByCounter(actions[i].cutting_contour, actions[i].cam, 1);
//	}
//}

void SegmentationMask3D::CutByCounter(std::vector<glm::vec2> cutting_contour)//, Camera& cam,int delta_value)
{
	int delta_value = 1;
	if (cutting_contour.size()<3)return;
	if (!is_inited())return;
	//KillBG_RC();
	/*if (delta_value==1)
		cut_actions.push_back(CutAction3D(cutting_contour, cam));*/

	glm::vec3 scale_ps = mask_spacing * scale_3d;
	
	glm::vec2 p0, p1;
	GetBounds(cutting_contour, p0, p1);
	double counter1 = 0;
	double counter2 = 0;
	unsigned char*dt = mask_data;
	for (int k = 0; k < mask_size.z; k++)
		for (int j = 0; j < mask_size.y; j++)
			for (int i = 0; i < mask_size.x; i++, dt++)
				//if (!*dt)
				{
					counter1++;
					glm::vec2 ipt2 = glm::vec2(121, 57); // cam.GetScreenCoords(glm::vec3(scale_ps.x * (i + 0.5), scale_ps.y * (j + 0.5), scale_ps.z * (k + 0.5)));
					glm::vec2 pt2(ipt2.x, ipt2.y);
					if (pt2.x>p0.x && pt2.y>p0.y && pt2.x < p1.x && pt2.y < p1.y)
						if (line_inside_lines(cutting_contour, pt2))
						{
							(*dt) = (*dt) + delta_value;
							counter2++;
						}
							
				}
}

