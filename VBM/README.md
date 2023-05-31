This folder contains the implementation of the virtual blade model for ANSYS Fluent. There are two possibilities to compile the UDF, either use the builtin menu in ANSYS Fluent, or the powershell scripts in this folder. It has been tested only in the 2022R2 version.

## Compilation outside ANSYS Fluent

First modify the property file named FluentDirst.props, and update the version, and paths to the ANSYS Fluent installation directory. Then copy the sconstruct.udf file from the fluent installation directory (should be located in src/udf), and rename it to SConstruct. Copy also the user.txt file. If you want to do debugging, than create a copy of this file and name it as SConstruct_debug. Edit this file as follows:
- add /Zi, and /ZI to the cflags variable (this variable is located at lines 255, 274)
- add env['LFLAGS']="/debug" after the CFLAGS environment variable assignment (lines 254, 276)
- at line 235 between /dll, and /out flags add $LFLAGS.

Alternatively an ed script is included in the SConstruct_debug_diff file. Then create a win64 folder, and 3ddp_host, and 3ddp_node folders under it. Then run init.ps1, or init_debug.ps1 depending on if you want to run debugging or not. Afterward, you can compile the UDF by running build.ps1.

## Debugging

To debug the UDF load the compiled dll into Fluent, then attach a debugger to every fl_mpi_*.exe process, where the value of * depends on the version of Fluent. Then you can set a breakpoint in the code, and it will be hit during the execution of the UDF.

## Setup folder
The files in this folder need to be put to the base folder of the simulations, where the case/data files are located. This is usually one level above the UDF folder. 

- Coef.csv contains the lift and moment coefficients for the airfoil. The first column is the angle, the second is the lift coefficient, while the third is the moment coefficient. The first column needs to be in increasing order.
- Geom.csv contains the geometrical parameters. The first column is the radial distance from the hub, the second is the chord length, while the third one is the angle of the twist of the blade. The first column needs to be in increasing order.
- Param.csv contains other parameters for the VBM. It should have as many rows, as there are VBMs, the parameters in order are the following: 
    1. Fluent ID of the VBM region
    2. Number of blades
    3. Pitch angle of the VBM region
    4. Bank angle of the VBM region
    5. Coning angle $(\beta_0)$ (Eq. 2.5)
    6. $\beta_{1c}$ (Eq. 2.5)
    7. $\beta_{1s}$ (Eq. 2.5)
    8. Collective angle of attack (Eq. 2.4)
    9. A from Eq. 2.4
    10. B from Eq. 2.4
    11. Angular velocity of the rotor
    12. Thickness of the VBM region
    13. The last three numbers are the x,y,z coordinates of the VBM.

The equation numbers correspond to the following publication, where more details can be found about the algorithm:

Stefano Wahono: Development of Virtual Blade Model for Modelling Helicopter Rotor Downwash in OpenFOAM

## Run batch simulation
In order to run lots of simulations, a python script is included in the vbm_batch_fluent folder. It should be placed into the main directory, where the case and data files are located, which should be named zero_vel_150.cas.h5 and zero_vel_150.dat.h5