This repository contains the code used for my MSc thesis. It has three parts:
- VBM: The implementation of the virtual blade model for CFD simulations using ANSYS Fluent. It is used to calculate the aerodynamic forces generated by a propeller.
- MathematicaPostprocessing: Mathematica code used to construct the data-driven aerodynamic model, and compare it to a simpler aerodynamic model. It also contains a stability analysis, using which a fine-tuned controller was created, which is compared to the original controller. A comparison of the simulations with the motion of the real drone is also included.
- SimulinkAnimation: Simulink, and MATLAB code for creating animations of the drone.