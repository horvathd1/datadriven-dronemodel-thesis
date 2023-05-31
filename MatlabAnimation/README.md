In this folder the Simulink, and Matlab code can be found for the animation of the drone.
- Drone.m contains the drone class, which deals with the simulation of the drone.
- Drone_model.slx is a Simulink helper file to calculate the motion of the drone.
- The three other Simulink files create the animations for different prescribed trajectories:
    - Animation_circle.slx: circular trajectory
    - Animation_square.slx: discontinous trajectory
    - Animation_square_v15.slx: discontinous trajectory with a higher frequency.