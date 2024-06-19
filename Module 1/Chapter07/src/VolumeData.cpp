#include "VolumeData.h"

VData::VData():size(0),spacing(1.0/512.0),allocated(0)
{
}
VData::VData(glm::vec3 n_size):size(0),spacing(1.0/512.0),allocated(0)
{
	Allocate(n_size);
}
bool VData::Allocated()
{
	return allocated;
}
void VData::Allocate(glm::vec3 n_size)
{
	Clear();
	
	value_size = 2;
	size = n_size;
	dt_s = new short*[n_size.z];
	for(int i=0;i<n_size.z;i++)
	{
		dt_s[i] = new short[n_size.x*n_size.y];
	}
	allocated=1;
}
void VData::SetAllValues(short val)
{

	if(dt_s)
	for(int k=0;k<size.z;k++)
	{
		short*dz = dt_s[k];
		for(int i=0;i<size.x*size.y;i++,dz++)
			*dz=val;
	}
	
}
void VData::ShiftAllValues(short val)
{

	if(dt_s)
	for(int k=0;k<size.z;k++)
	{
		short*dz = dt_s[k];
		for(int i=0;i<size.x*size.y;i++,dz++)
			*dz+=val;
	}
	

}
VData::~VData()
{
	Clear();
}

void VData::Clear()
{
	if(!allocated)return;
	allocated=0;
	for(int i=0;i<size.z;i++)
	{
		delete[] dt_s[i];
	}
	delete[]dt_s;
	

}

//
//
//short VData::GetValue(glm::vec3 id)
//{
//	if(id.x<0 || id.x>=size.x || id.y<0 || id.y>=size.y || id.z<0 || id.z>=size.z)return 0;
//	return *(dt_s[id.z]+id.x+id.y*size.x);
//
//}
//
//short VData::GetValue_fast(glm::vec3& id)
//{
//	return *(dt_s[id.z]+id.x+id.y*size.x);
//
//}
//
//short VData::GetValue(int i,int j,int k)
//{
//	if(i<0 || i>=size.x || j<0 || j>=size.y || k<0 || k>=size.z)return 0;
//		return *(dt_s[k]+i+j*size.x);
//
//}
//
//void VData::SetValue(short val, glm::vec3 id)
//{
//		*(dt_s[(int)id.z] + (int)id.x + (float)id.y*size.x) = val;	
//
//}
//
//void VData::SetValue(short val,int i,int j,int k)
//{
//		*(dt_s[k]+i+j*size.x) = val;
//
//}

void*VData::GetSlice(int z)
{
	if(!allocated)return 0;
		return dt_s[z];

}

void**VData::GetSlices()
{
	if(!allocated)return 0;
		return (void**)dt_s;

}



