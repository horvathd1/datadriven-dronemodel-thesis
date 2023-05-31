#ifndef SOURCE
#define SOURCE
#include <vector>
#include "udf.h"
#include "file_types.h"

namespace udf_vbm
{
	struct angle_trig_values
	{
		real cpitch, spitch, cbank, sbank, cpsi, spsi, cbeta, sbeta;
	};

	class vbm_domain
	{
	public:
		vbm_domain(const vbm_param& params, geom geometry, aero_coef coefs);

		std::vector<real> calculate_source(std::vector<real> velocities, std::vector<real> coords, real density,
		                                   real volume, std::vector<real> prev_sources, std
		                                   ::vector<real>& to_save_udm
		);
		void print_values() const;
		int getthreadid() const;
	private:
		std::vector<real> vbm_domain::calculate_forces(real density, real r, std::vector<real> vel_LRF);
		void obtain_generic_angle_trig_values();
		void obtain_specific_angle_trig_values(real psi);
		std::vector<real> retransform_forces(real fz, real fpsi);
		static real interpolate(real x_0, real x_1, real y_0, real y_1, real x);
		std::vector<real> get_array_values(std::vector<real> selector, std::vector<std::vector<real>> values,
		                                   real search_value) const;
		void obtain_file_values(real r, real aoa_induced,
		                        real& chord_length, real& cl, real& cd);
		std::vector<real> transform_velocities(const std::vector<real>& coords, const std::vector<real>& vel,
		                                       const vbm_param& params) const;
		vbm_param params_;
		angle_trig_values trig_vals_;
		geom geometry_;
		aero_coef coefs_;
		std::vector<real> prev_sources_;
	};
}
#endif
