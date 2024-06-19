#include "CPU_VD.h"

namespace CPU_VD
{
	#define ACC_DATA_TYPES 3 //0-min,1-max,2-average
	#define ACC_CONST data_stat.MinValue //to overcome 16-bit limit when calculating average
	VData acc_data[ACC_DATA_TYPES][3];
	std::vector<bool> acc_data_inited[ACC_DATA_TYPES][3];
	int acc_chunk=32;


	VData full_data;
	glm::vec3 data_offset;


	VData* GetData()
	{
		
		return &full_data;
	}
	


	

	
}