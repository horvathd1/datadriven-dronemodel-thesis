#include "file_types.h"

udf_vbm::vbm_param::vbm_param(std::vector<real> parameters)
{
	id = static_cast<int>(parameters[0]);
	n_blades = static_cast<int>(parameters[1]);
	pitch_angle = parameters[2];
	bank_angle = parameters[3];
	coning_angle = parameters[4];
	beta_1_c = parameters[5];
	beta_1_s = parameters[6];
	alpha_coll = parameters[7];
	alpha_a = parameters[8];
	alpha_b = parameters[9];
	omega = parameters[10];
	thickness = parameters[11];
	for (size_t i = 0; i < 3; ++i)
	{
		origin[i] = parameters[12 + i];
	}
}
