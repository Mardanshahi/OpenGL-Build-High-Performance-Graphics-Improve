#ifndef VDATA_H
#define VDATA_H

//#include "AllDef.h"
//#include "vec3.h"
//#include "vec2.h"
//#include "ivec3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

struct VolumeData
{
	VolumeData();
	short** data;
	glm::vec3 size;
	glm::vec3 spacing;
};


class VData
{
public:
	VData();
	~VData();
	VData(glm::vec3 n_size);

	void Allocate(glm::vec3 n_size);
	void Clear();

	void SetAllValues(short val);
	void ShiftAllValues(short val);
	void SetValue(short val, glm::vec3 id);
	void SetValue(short val,int i,int j,int k);
	short GetValue(glm::vec3 id);
	short GetValue_fast(glm::vec3& id);
	short GetValue(int i,int j,int k);
	bool Allocated();
	void*GetSlice(int z);
	void**GetSlices();
	glm::vec3 GetSize() { return size; }
	int GetGLFormat() { return GL_SHORT; }
	glm::vec3 spacing;
	int GetValueSize(){return value_size;}


private:
	bool allocated;
	short**dt_s;
	glm::vec3 size;

	int value_size;
};


#endif