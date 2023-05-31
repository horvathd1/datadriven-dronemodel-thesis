#include "udf.h"
#include "global_vars.h"
#include <vector>
#include <cmath>
#include "compute_source.h"
#include <algorithm>
#include <string>

namespace udf_vbm
{
	void vbm_domain::obtain_generic_angle_trig_values()
	{
		trig_vals_.cpitch = cos(params_.pitch_angle);
		trig_vals_.spitch = sin(params_.pitch_angle);

		trig_vals_.cbank = cos(params_.bank_angle);
		trig_vals_.sbank = sin(params_.bank_angle);
	}

	void vbm_domain::obtain_specific_angle_trig_values(real psi)
	{
		trig_vals_.cpsi = cos(psi);
		trig_vals_.spsi = sin(psi);

		// flapping angle (beta)
		real beta = params_.coning_angle - params_.beta_1_c * trig_vals_.cpsi - params_.beta_1_s * trig_vals_.spsi;

		// Trig functions of beta
		trig_vals_.cbeta = cos(beta);
		trig_vals_.sbeta = sin(beta);
	}

	std::vector<real> vbm_domain::retransform_forces(real fz, real fpsi)
	{
		real forces_rsp_cyl[3] = {trig_vals_.sbeta * fz, fpsi, trig_vals_.cbeta * fz};
		real forces_rsp_xyz[3] = {
			trig_vals_.cpsi * forces_rsp_cyl[0] - trig_vals_.spsi * forces_rsp_cyl[1],
			trig_vals_.spsi * forces_rsp_cyl[0] + trig_vals_.cpsi * forces_rsp_cyl[1],
			forces_rsp_cyl[2]
		};
		std::vector<real> forces_global = {
			forces_rsp_xyz[0] * trig_vals_.cpitch + forces_rsp_xyz[1] * trig_vals_.sbank * trig_vals_.spitch -
			forces_rsp_xyz[2] * trig_vals_.spitch * trig_vals_.cbank,
			forces_rsp_xyz[1] * trig_vals_.cbank + forces_rsp_xyz[2] * trig_vals_.sbank,
			forces_rsp_xyz[0] * trig_vals_.spitch - forces_rsp_xyz[1] * trig_vals_.sbank * trig_vals_.cpitch +
			forces_rsp_xyz[2] * trig_vals_.cpitch * trig_vals_.cbank
		};
		return forces_global;
	}

	real vbm_domain::interpolate(real x_0, real x_1, real y_0, real y_1, real x)
	{
		return (y_1 - y_0) / (x_1 - x_0) * (x - x_0) + y_0;
	}

	std::vector<real> vbm_domain::get_array_values(std::vector<real> selector,
	                                               const std::vector<std::vector<real>> values,
	                                               const real search_value) const
	{
		int index = 0;
		for (; index < selector.size() && selector[index] < search_value; ++index)
		{
		}
		std::vector<real> result = {};
		if (index == 0 || index == selector.size())
		{
			for (auto& array : values)
			{
				result.push_back(index == 0 ? *array.begin() : *(array.end() - 1));
			}
			return result;
		}
		index--;
		for (auto& array : values)
		{
			auto value = interpolate(selector[index], selector[index + 1], array[index], array[index + 1],
			                         search_value);
			result.push_back(value);
		}
		return result;
	}

	void vbm_domain::obtain_file_values(real const r, real const aoa_induced, real& chord_length, real& cl, real& cd)
	{
		const auto geom_arr_vals = get_array_values(geometry_.rad_dists, {
			                                            geometry_.chord_lengths, geometry_.twist_angles
		                                            }, r);
		chord_length = geom_arr_vals[0];
		const real twist_angle = geom_arr_vals[1];

		const real aoa_geom = params_.alpha_coll + params_.alpha_a * trig_vals_.cpsi + params_.alpha_b * trig_vals_.spsi
			+ twist_angle;
		const real aoa_eff = aoa_geom - aoa_induced;
		const auto coef_arr_vals = get_array_values(coefs_.aoa, {coefs_.cl, coefs_.cd}, aoa_eff);
		cl = coef_arr_vals[0];
		cd = coef_arr_vals[1];
	}

	std::vector<real> vbm_domain::transform_velocities(const std::vector<real>& coords, const std::vector<real>& vel,
	                                                   const vbm_param& params) const
	{
		// RSP: Rotor Shaft Plane Stationary Cartesian velocities
		real vel_RSP_C[3]{};

		vel_RSP_C[0] = vel[0] * trig_vals_.cpitch + vel[2] * trig_vals_.cbank * trig_vals_.spitch + vel[1] * trig_vals_.
			spitch * trig_vals_.sbank;
		vel_RSP_C[1] = vel[1] * trig_vals_.cbank - vel[2] * trig_vals_.sbank;
		vel_RSP_C[2] = vel[2] * trig_vals_.cpitch * trig_vals_.cbank - vel[0] * trig_vals_.spitch + vel[1] * trig_vals_.
			spitch * trig_vals_.sbank;

		// RSP cylindrical velocities
		real vel_RSP_cyl[3]{};

		vel_RSP_cyl[0] = vel_RSP_C[0] * trig_vals_.cpsi + vel_RSP_C[1] * trig_vals_.spsi;
		vel_RSP_cyl[1] = vel_RSP_C[1] * trig_vals_.cpsi - vel_RSP_C[0] * trig_vals_.spsi;
		vel_RSP_cyl[2] = vel_RSP_C[2];

		std::vector<real> vel_LRF(3);

		vel_LRF[0] = vel_RSP_cyl[0] * trig_vals_.cbeta - vel_RSP_cyl[2] * trig_vals_.sbeta;
		vel_LRF[1] = vel_RSP_cyl[1];
		vel_LRF[2] = vel_RSP_cyl[2] * trig_vals_.cbeta + vel_RSP_cyl[0] * trig_vals_.sbeta;
		return vel_LRF;
	}

	std::vector<real> vbm_domain::calculate_forces(real density, real r, std::vector<real> vel_LRF)
	{
		real v_rel_tang = -vel_LRF[1] + r * params_.omega;
		real aoa_induced = atan2(vel_LRF[2], v_rel_tang);

		real chord_length;
		real cl;
		real cd;
		obtain_file_values(r, aoa_induced, chord_length, cl, cd);
		// Local velocity
		real u_local = sqrt(vel_LRF[2] * vel_LRF[2] + v_rel_tang * v_rel_tang);

		//Forces
		real lift;
		real drag;
		lift = density / 2 * u_local * u_local * chord_length * cl;
		drag = -density / 2 * u_local * u_local * chord_length * cd;
		if (r > 0.95*geometry_.rad_dists[geometry_.rad_dists.size()-1])
		{
			lift = 0;
		}
		//Forces aligning with coordinates
		real sin_aoa_induced = sin(aoa_induced);
		real cos_aoa_induced = cos(aoa_induced);
		real fz = lift * cos_aoa_induced - drag * sin_aoa_induced;
		real fpsi = lift * sin_aoa_induced + drag * cos_aoa_induced;
		return std::vector<real>({fpsi, fz});
	}

	vbm_domain::vbm_domain(const vbm_param& params, geom geometry, aero_coef coefs) : params_(params),
		geometry_(std::move(geometry)),
		coefs_(std::move(coefs))
	{
		obtain_generic_angle_trig_values();
		prev_sources_ = {};
	}


	std::vector<real> vbm_domain::calculate_source(const std::vector<real> velocities, std::vector<real> coords,
	                                               const real density, const real volume,
	                                               const std::vector<real> prev_sources, std::vector<real>& to_save_udm)
	{
		//Coordinate transforms
		for (int i = 0; i < 3; ++i)
		{
			coords[i] -= params_.origin[i];
		}
		const real r = sqrt(coords[0] * coords[0] + coords[1] * coords[1]);
		const real psi = atan2(coords[1], coords[0]);
		obtain_specific_angle_trig_values(psi);
		auto rel_cyl_vels = transform_velocities(coords, velocities, params_);

		//Force calculation, and transform
		auto forces = calculate_forces(density, r, rel_cyl_vels);
		real fpsi = forces[0], fz = forces[1];
		//Averaging
		auto area = volume / params_.thickness;
		real mean_fz = params_.n_blades / (2 * M_PI) * fz * area / r;
		real mean_fpsi = params_.n_blades / (2 * M_PI) * fpsi * area / r;
		//Retransform
		auto forces_xyz = retransform_forces(mean_fz, mean_fpsi);

		//Source calculation
		std::vector<real> sources;
		real mult_curr = 1;
		//Filtering
		for (int i = 0; i < forces_xyz.size(); i++)
		{
			const real force = forces_xyz[i];
			const auto source = -force / volume;
			sources.push_back(source * mult_curr + (1 - mult_curr) * prev_sources[i]);
		}
		to_save_udm.resize(6);
		for (int i = 0; i < 3; ++i)
		{
			to_save_udm[i] = rel_cyl_vels[i];
			to_save_udm[i + 3] = sources[i];
		}
		return sources;
	}

	void vbm_domain::print_values() const
	{
		std::string string_params = "Read parameters: %d,%d";
		for (int i = 0; i < 13; ++i)
		{
			string_params.append(",%f");
		}
		string_params.append("\n");
		Message(string_params.c_str(), params_.id, params_.n_blades, params_.pitch_angle, params_.bank_angle,
		        params_.coning_angle, params_.beta_1_c, params_.beta_1_s, params_.alpha_coll, params_.alpha_a,
		        params_.alpha_b, params_.omega, params_.thickness, params_.origin[0], params_.origin[1],
		        params_.origin[2]);
		Message("Read geometry:\n");
		for (int i = 0; i < geometry_.rad_dists.size(); i++)
		{
			Message("%f,%f,%f\n", geometry_.rad_dists[i], geometry_.chord_lengths[i], geometry_.twist_angles[i]);
		}
		Message("Read coefficients:\n");
		for (int i = 0; i < coefs_.aoa.size(); i++)
		{
			Message("%f,%f,%f\n", coefs_.aoa[i], coefs_.cl[i], coefs_.cd[i]);
		}
		Message("---END---\n");
	}

	int vbm_domain::getthreadid() const
	{
		return params_.id;
	}
}
