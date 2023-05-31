In this folder the mathematica code can be found to produce the results of my MSc thesis. The plots are saved to the fig folder, which you have to create manually.
 - ml_model.nb contains the construction of the data-driven aerodynamic model, for which the CFD results are in the Res folder. 
 - Plot_Trajectories_oldController.nb contains a comparison between the data-driven and a simpler aerodynamic model for some prescribed trajectories. 
 - Jacobi.nb contains a stability analysis, and the determination of the fine-tuned controller parameters. 
 - Plot_Trajectories_newController.nb contains the comparison of the fine-tuned controller with the original. 
 - Trajectory_Tracking_Comparison.nb contains the comparison of the simulation with the trajectory tracking of the real drone. For this the motion of the drone is found in the .csv files in the root folder.