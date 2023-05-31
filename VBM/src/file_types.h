#ifndef FILE_TYPES_H
#define FILE_TYPES_H
#include <vector>

#include "udf.h"

namespace udf_vbm
{
	struct vbm_param
	{
		int id;
		int n_blades;
		real pitch_angle;
		real bank_angle;
		real coning_angle;
		real beta_1_c;
		real beta_1_s;
		real alpha_coll;
		real alpha_a;
		real alpha_b;
		real omega;
		real thickness;
		real origin[3];

		explicit vbm_param(std::vector<real>);
	};

	struct aero_coef
	{
		std::vector<real> aoa;
		std::vector<real> cl;
		std::vector<real> cd;
	};

	struct geom
	{
		std::vector<real> rad_dists;
		std::vector<real> chord_lengths;
		std::vector<real> twist_angles;
	};
}
#endif // FILE_TYPES_H
