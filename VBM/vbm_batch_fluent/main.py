import math

import ansys.fluent.core as pyfluent
import pandas as pd

if __name__ == '__main__':
    solver_session = pyfluent.launch_fluent(precision="double", processor_count=4, mode="solver")
    solver_session.check_health()
    solver_session.tui.file.read_case('zero_vel_150.cas.h5')
    omega_arr=[1000,1400,1700,1900,2000] # Omega values to simulate
    v_arr=[1,2,3,4,5] # Velocity values to simulate
    angle_arr=[-80,-85,-90] # Velocity angle values to simulate
    out_arr=[]
    v_inlets=["inlet-geom-3-inner_top","inlet-geom-3-inner_top_cyl_top","inlet-geom-3-outer","side-inlet"]
    run_arg=[]
    # Uncomment the following two lines to simulate zero velocity case:
    #for omega in omega_arr:
    #    run_arg.append([0,0,omega])
    for vmag in v_arr:
        for angle in angle_arr:
            for omega in omega_arr:
                run_arg.append([vmag,angle,omega])
    index=0
    for args in run_arg:
        vmag=args[0]
        angle=args[1]*math.pi/180
        omega=args[2]
        params = pd.read_csv("Setup/Params.csv", sep=';', header=None)
        params[10] = omega
        params.to_csv("Setup/Params.csv",sep=';',index=False,header=False)
        solver_session.tui.file.read_data('zero_vel_150_2.dat.h5')
        solver_session.execute_tui("define/user-defined/execute-on-demand/\"reinit::libudf\"")
        vx=vmag*math.cos(angle)
        vy=vmag*math.sin(angle)
        for inlet_name in v_inlets:
            solver_session.setup.boundary_conditions.velocity_inlet[inlet_name].velocity_component[0].constant=-vx
            solver_session.setup.boundary_conditions.velocity_inlet[inlet_name].velocity_component[2].constant=-vy
        solver_session.tui.solve.iterate(49)
        defs=solver_session.solution.report_definitions.compute(report_defs=["minus-lift","moment"])
        solver_session.tui.solve.iterate(1)
        defs2 = solver_session.solution.report_definitions.compute(report_defs=["minus-lift", "moment"])
        out_arr.append([vmag,angle,omega,(defs["minus-lift"][0]+defs2["minus-lift"][0])/2,(defs2["moment"][0]+defs["moment"][0])/2])
        index+=1
        print("{}/{}".format(index, len(run_arg)))
    out_frame=pd.DataFrame(out_arr,columns=["v","angle","omega","lift","moment"])
    print(out_frame)
    out_frame.to_csv("Res/out_m90.csv",sep=';',index=False)
    solver_session.exit()
