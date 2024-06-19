#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include "Render.h"
#include "CPU_VD.h"
//#include "Shader.h"
//#include "AllDef.h"
//#include "Draw.h"
//#include <windows.h>
//#include "vec.h"
//#include "str.h"
//#include "Camera.h"
//#include "Draw2D.h"
//#include "RenderToTexture.h"
//#include "CT.h"

bool high_quality=0;


long int occ_mem=0;
float occ_space=0;

extern int GL_CANVAS_NUM;

//IsoViewer::IsoViewer():b1(0),b2(1),cur_rendering_method(0)
//{
//
//	data_spacing.set(1,1,1);
//	data_type=0;//0-short 1-byte
//
//	Threshold=0;
//	frames_to_accumulate=1;
//
//
//	need_reload_acc=1;
//
//	st_i=0;
////	st_min=0;
//	for(int i=0;i<VOLUME_NUM;i++){st[i]=0;}
//	draw_frame_is=0;
//	//ReloadShader();
//	frames_acc=0;
//
//	CT::rendering_methods_to_show.clear();
//	CT::rendering_methods_to_show.push_back(0);
//	CT::rendering_methods_to_show.push_back(1);
//	CT::rendering_methods_to_show.push_back(2);
//	CT::rendering_methods_to_show.push_back(3);
//
//	rendering_methods.resize(GL_CANVAS_NUM);
//	for(int i=0;i<rendering_methods.size();i++)
//	{
//		RenderingMethod*rm = new RenderingMethod();
//		rm->this_id=i;
//		rm->tf_id=i;
//		rm->cur_rm_mode = i%4;
//		rendering_methods[i] = rm;
//	}
//	
//
//}
//RenderingMethod* IsoViewer::GetCurRM()
//{
//	return rendering_methods[cur_rendering_method];
//}
//RenderingMethod* IsoViewer::GetActRM()
//{
//	return rendering_methods[active_rendering_method];
//}


//установка координат концов ограничивающей коробки
void IsoViewer::SetBoundingBox(glm::vec3 a, glm::vec3 b)
{
	
	b1=a;
	b2=b;
	if(b1.x>b2.x)
		std::swap(b1.x,b2.x);
	if(b1.y>b2.y)
		std::swap(b1.y,b2.y);
	if(b1.z>b2.z)
		std::swap(b1.z,b2.z);
	//CT::need_rerender2d=1;
}

//установка положения и ориентации камеры
//void IsoViewer::ApplyCamera(Camera* c)
//{
//	GetActRM()->ps->Use();
//	GetActRM()->ps->SetVar("pos",c->GetPosition());
//	GetActRM()->ps->SetVar("nav",c->GetNav());
//	GetActRM()->ps->SetVar("LightDir",(c->GetNav()+c->GetTop()*0.1f+c->GetLeft()*0.1f).normalized()*(-1));
//
//	GetActRM()->ps->UnUse();
//}
//vec2 IsoViewer::GetTFWW()
//{
//	return GetCurRM()->tf_ww;
//}
//
//vec4* IsoViewer::GetCurTF()
//{
//	return GetCurRM()->color_table;
//}
//void IsoViewer::ClearRMs()
//{
//	for(int i=0;i<rendering_methods.size();i++)
//		delete rendering_methods[i];
//	rendering_methods.clear();
//}
IsoViewer::~IsoViewer()
{
	
	//ClearRMs();
	ClearST();
}
//void IsoViewer::ReloadShader()
//{
//	for(int i=0;i<rendering_methods.size();i++)
//		rendering_methods[i]->need_reload_shader=1;
//}
//
//void IsoViewer::DragWindow(ivec2 w0,ivec2 w1)
//{
//	if(!(w0.y-w0.x))return;
//	
//	RenderingMethod*rm = GetCurRM();
//	for(int i=0;i<rm->tf_points.size();i++)
//	{
//		rm->tf_points[i].value = (w1.y-w1.x)*(rm->tf_points[i].value-w0.x)/(w0.y-w0.x)+w1.x;
//	}
//	rm->ApplyPoints();
//}


void IsoViewer::ClearST()
{
	if(st_i)delete st_i;
//	if(st_min)delete st_min;
//	st_min=0;
	st_i=0;
	if(st[0])
		for(int i=0;i<VOLUME_NUM;i++){if(st[i])delete st[i];st[i]=0;}
	
}
//void IsoViewer::SetRM(int rm)
//{
//	if(GetCurRM()->cur_rm_mode == rm)return;
//	GetCurRM()->cur_rm_mode = rm;
//	GetCurRM()->NeedReloadShader();
//}


void IsoViewer::FitBoundingBox()
{
	b1 = -(CPU_VD::GetData()->spacing);
	b2 = CPU_VD::GetData()->GetSize() * CPU_VD::GetData()->spacing+CPU_VD::GetData()->spacing;
}

void IsoViewer::MakeBrick(unsigned short** data, glm::vec3 size, glm::vec3 pos, glm::vec3 b_size, int brick_id)
{
	glm::vec3 b_size1 = glm::min(b_size, size - pos);
	auto fff = b_size1.x;
	if ((int)b_size1.x % 2)b_size1.x++;
	if ((int)b_size1.y % 2)b_size1.y++;
	if ((int)b_size1.z % 2)b_size1.z++;


	unsigned short* s_data = new unsigned short[b_size1.x * b_size1.y * b_size1.z];

	unsigned short* s_data_cur = s_data, v0 = 0;
	glm::vec3 bb1 = b_size1, bb0(0);
	glm::vec3 cell_size = data_spacing;


	max_x[brick_id].resize(b_size1.x); for (int i = 0; i < max_x[brick_id].size(); i++)max_x[brick_id][i] = 0;
	max_y[brick_id].resize(b_size1.y); for (int i = 0; i < max_y[brick_id].size(); i++)max_y[brick_id][i] = 0;
	max_z[brick_id].resize(b_size1.z); for (int i = 0; i < max_z[brick_id].size(); i++)max_z[brick_id][i] = 0;


	for (int kk = 0; kk < b_size1.z; kk++)
		for (int jj = 0; jj < b_size1.y; jj++)
		{
			unsigned short* data_cur = data[(int)pos.z + kk] + ((int)pos.x + (int)size.x * ((int)pos.y + jj));
			for (int ii = 0; ii < b_size1.x; ii++, s_data_cur++, data_cur++)
			{
				if (pos.x + ii < 0 || pos.x + ii >= size.x || pos.y + jj < 0 || pos.y + jj >= size.y || pos.z + kk < 0 || pos.z + kk >= size.z)
				{
					*s_data_cur = 0;
				}
				else
				{
					*s_data_cur = *data_cur;
				}
				if (v0 < *s_data_cur)v0 = *s_data_cur;

				if (max_x[brick_id][ii] < *s_data_cur)max_x[brick_id][ii] = *s_data_cur;
				if (max_y[brick_id][jj] < *s_data_cur)max_y[brick_id][jj] = *s_data_cur;
				if (max_z[brick_id][kk] < *s_data_cur)max_z[brick_id][kk] = *s_data_cur;
			}
		}



	bb0 = glm::max(glm::vec3(0), bb0 - glm::vec3(BRICKS_OVERLAP));
	bb1 = glm::min(b_size1 - glm::vec3(1), bb1 + glm::vec3(BRICKS_OVERLAP));

	max_val[brick_id] = v0;
	glm::vec3 bbb1 = glm::vec3(bb0.x / float(b_size.x - 1), bb0.y / float(b_size.y - 1), bb0.z / float(b_size.z - 1));
	glm::vec3 bbb2 = glm::vec3(bb1.x / float(b_size.x - 1), bb1.y / float(b_size.y - 1), bb1.z / float(b_size.z - 1));
	box1[brick_id] = cell_size + glm::vec3((bi) * (b_size.x - BRICKS_OVERLAP - 1) * cell_size.x, (bj) * (b_size.y - BRICKS_OVERLAP - 1) * cell_size.y, (bk) * (b_size.z - BRICKS_OVERLAP - 1) * cell_size.z);
	box2[brick_id] = cell_size + glm::vec3((bi + 1) * (b_size.x - BRICKS_OVERLAP - 1) * cell_size.x, (bj + 1) * (b_size.y - BRICKS_OVERLAP - 1) * cell_size.y, (bk + 1) * (b_size.z - BRICKS_OVERLAP - 1) * cell_size.z);

	glm::vec3 n_b1 = box1[brick_id] + bbb1 * (box2[brick_id] - box1[brick_id]);
	glm::vec3 n_b2 = box1[brick_id] + bbb2 * (box2[brick_id] - box1[brick_id]);

	box1[brick_id] = n_b1;
	box2[brick_id] = n_b2;
	txt_box1[brick_id] = box1[brick_id] - cell_size * (float)(BRICKS_OVERLAP - 0.5);
	txt_box2[brick_id] = box2[brick_id] + cell_size * (float)(BRICKS_OVERLAP - 0.5);

	//	if(max_val[brick_id]>l1*DataValueKoef)
	{
		occ_mem += b_size1.x * b_size1.y * b_size1.z * 2;
		block_size_arr[brick_id] = b_size1;
		st[brick_id] = new SimText3D(3, b_size1.x, b_size1.y, b_size1.z, 1, s_data, 1, true, GL_SHORT);
	}
	//	else  st[brick_id]=0;

	delete[] s_data;


}



void IsoViewer::UploadFieldData(unsigned short** data, glm::vec3 size, glm::vec3 spacing)
{
	//Progress progress(/*wxT*/(MY_TXT("Loading data to GPU...", "Загрузка данных в GPU")));

	ClearST();
	DataValueKoef = 128 * 256;
	//ivec3 s_size = size/VOLUME_DIV;
	data_spacing = spacing;
	glm::vec3 s_size(BRICK_SIZE);
	glm::vec3 s_size1 = s_size - glm::vec3(BRICKS_OVERLAP * 2 - 1);
	int st_id = 0;
	int it_num = (1 + size.x / (BRICK_SIZE + BRICKS_OVERLAP * 2)) * (1 + size.y / (BRICK_SIZE + BRICKS_OVERLAP * 2)) * (1 + size.z / (BRICK_SIZE + BRICKS_OVERLAP * 2)), it_done = 0;
	//Progress::inst->SetPercent(0);
	for (int k = 0; k < VOLUME_DIV_Z; k++)
		for (int j = 0; j < VOLUME_DIV_Y; j++)
			for (int i = 0; i < VOLUME_DIV_X; i++)
			{
				bi = i; bj = j; bk = k;
				glm::vec3 pos = glm::vec3(i, j, k) * s_size1;

				if (pos.x < size.x - 8 && pos.y < size.y - 8 && pos.z < size.z - 8)
				{
					MakeBrick(data, size, pos, s_size, st_id);
				}
				else
				{
					st[st_id] = 0;
				}

				st_id++;
				it_done++;

				//Progress::inst->SetPercent((it_done) / float(it_num));

			}

	//UpdateBricksBounds();
	//FitBoundingBox();

//	UploadMin((unsigned short**)sub_min.data,sub_min.size);


}
void IsoViewer::UploadAccStr(unsigned char* data, glm::vec3 size)
{
	if (st_i)delete st_i;
	st_i = new SimText3D(3, size.x, size.y, size.z, 1, data, TXT_ACC, 0, GL_UNSIGNED_BYTE);
	need_reload_acc = 0;
}

