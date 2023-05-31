#include <algorithm>

#include "compute_source.h"
#include "csv_utils.h"
#include "udf.h"
#include "global_vars.h"
#include <vector>


namespace udf_vbm
{
	std::vector<vbm_domain> vbms;

	std::vector<std::vector<real>> fill_arrays(const std::string filename)
	{
		std::vector<std::vector<real>> arrays;
		auto file = csv_file(filename);
		auto parse = file.read();
		for (auto& row : parse)
		{
			while (arrays.size() < row.size())
			{
				arrays.emplace_back();
			}
			for (int i = 0; i < row.size(); ++i)
			{
				arrays[i].push_back(row[i]);
			}
		}
		file.~csv_file();
		return arrays;
	}

	void init()
	{
		//Geometry
		auto geom_arr = fill_arrays("./Setup/Geom.csv");
		geom geometry{ geom_arr[0], geom_arr[1], geom_arr[2] };

		//Aerodynamic coefficients
		auto coef_arr = fill_arrays("./Setup/Coef.csv");
		aero_coef coefficients{ coef_arr[0], coef_arr[1], coef_arr[2] };

		//VBM parameters
		auto params_file = csv_file("./Setup/Params.csv");
		auto params = params_file.read();
		params_file.~csv_file();
		for (int i = 0; i < params.size(); i++)
		{
			auto& row = params[i];
			vbm_param param(row);
			vbm_domain domain{ param, geometry, coefficients };
			if (static_cast<int>(vbms.size()) <= i)
			{
				vbms.emplace_back(domain);
			}
			else
			{
				vbms[i] = domain;
			}
		}
	}

	DEFINE_EXECUTE_ON_LOADING(name, libname)
	{
		init();
		offset = Reserve_User_Memory_Vars(6);
		if (offset == -1)
		{
			//Error("Not able to reserve UDMs, please reload library.");
			offset = 0;
		}
		for (int i = 0; i < 3; ++i)
		{
			Set_User_Memory_Units(0, "m/s");
			//Set_User_Memory_Units(i + 3, "N/m^3");
		}
		Set_User_Memory_Name(0, "Relative velocity r");
		Set_User_Memory_Name(1, "Relative velocity phi");
		Set_User_Memory_Name(2, "Relative velocity z");
		Set_User_Memory_Name(3, "Source x");
		Set_User_Memory_Name(4, "Source y");
		Set_User_Memory_Name(5, "Source z");
	}

	DEFINE_ON_DEMAND(reinit)
	{
		init();
	}

	DEFINE_ON_DEMAND(print_vals)
	{
#if RP_HOST
		for (auto& vbm : vbms)
		{
			vbm.print_values();
		}
#endif
	}

	vbm_domain get_vbm(Thread* thread)
	{
		int id = THREAD_ID(thread);
		int index = 0;
		for (; index < vbms.size(); index++)
		{
			if (vbms[index].getthreadid() == id)
			{
				break;
			}
		}
		if (index == vbms.size())
		{
			Error("Can't find ThreadID: %d", id);
		}
		return vbms[index];
	}

	void get_fluent_vars(cell_t cell, Thread* thread, std::vector<real>& velocities, std::vector<real>& coordinates,
		real& density, real& volume)
	{
		real coord_temp[3];
		C_CENTROID(coord_temp, cell, thread);
		for (real coord : coord_temp)
		{
			coordinates.push_back(coord);
		}
		velocities = { C_U(cell, thread),C_V(cell, thread),C_W(cell, thread) };
		density = C_R(cell, thread);
		volume = C_VOLUME(cell, thread);
	}

	std::vector<real> get_prev_sources(cell_t cell, Thread* thread)
	{
		std::vector<real> result;
		for (int i = 0; i < 3; ++i)
		{
			result.push_back(C_UDMI(cell, thread, offset + i + 3));
		}
		return result;
	}

	void save_udm(cell_t cell, Thread* thread, std::vector<real> values)
	{
		for (int i = 0; i < 6; ++i)
		{
			C_UDMI(cell, thread, offset + i) = values[i];
		}
	}

	DEFINE_SOURCE(x_source, cell, thread, dS, eqn)
	{
		auto current_vbm = get_vbm(thread);
		std::vector<real> velocities, coordinates;
		real density, volume;
		get_fluent_vars(cell, thread, velocities, coordinates, density, volume);

		std::vector<real> save_to_udm;
		std::vector<real> sources = current_vbm.calculate_source(velocities, coordinates, density, volume,
			get_prev_sources(cell, thread), save_to_udm);
		save_udm(cell, thread, save_to_udm);
		return sources[0];
	}

	DEFINE_SOURCE(y_source, cell, thread, dS, eqn)
	{
		auto current_vbm = get_vbm(thread);
		std::vector<real> velocities, coordinates;
		real density, volume;
		get_fluent_vars(cell, thread, velocities, coordinates, density, volume);

		std::vector<real> save_to_udm;
		std::vector<real> sources = current_vbm.calculate_source(velocities, coordinates, density, volume,
			get_prev_sources(cell, thread), save_to_udm);
		return sources[1];
	}

	DEFINE_SOURCE(z_source, cell, thread, dS, eqn)
	{
		auto current_vbm = get_vbm(thread);
		std::vector<real> velocities, coordinates;
		real density, volume;
		get_fluent_vars(cell, thread, velocities, coordinates, density, volume);

		std::vector<real> save_to_udm;
		std::vector<real> sources = current_vbm.calculate_source(velocities, coordinates, density, volume,
			get_prev_sources(cell, thread), save_to_udm);
		return sources[2];
	}

	DEFINE_INIT(initialize_udm, domain)
	{
		cell_t cell;
		Thread* thread;
		thread_loop_c(thread, domain)
		{
			begin_c_loop_all(cell, thread)
			{
				for (int i = 0; i < 6; ++i)
				{
					C_UDMI(cell, thread, offset + i) = 0.0;
				}
			}
			end_c_loop_all(cell, thread)
		}
	}

	DEFINE_REPORT_DEFINITION_FN(moment)
	{
		auto dom = Get_Domain(1);
		auto thread = Lookup_Thread(dom, vbms[0].getthreadid());
		real res = 0;
		cell_t c;
		begin_c_loop(c, thread)
		{
			real coords[3];
			C_CENTROID(coords, c, thread);
			real volume = C_VOLUME(c, thread);
			res += coords[0] * C_UDMI(c, thread, 5) * volume - coords[1] * C_UDMI(c, thread, 4) * volume;
		}
		end_c_loop(c, thread)

			node_to_host_real_1(res);
		return res;
	}
}
