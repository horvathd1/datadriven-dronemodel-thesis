classdef Drone
    
    properties
        K1=-7;
        K2=-1.5;
        K3=0.04;
        D1=-1.42;
        D2=-1.95;
        D3=0.002;
        J=0.0000116458;
        L=0.035;
        m=0.086;
        omega_max=2.0106e+03;
        omega_min=1.0472e+03;
        scale=4050;
        g=9.81;
        is_data_driven;
    end
    
    methods
        function obj = Drone(is_force_data_driven)
            obj.is_data_driven=is_force_data_driven;
        end
        
        function [ddtranslation,ddrotation]=Compute_state(obj,translation,...
                rotation,dtranslation,drotation,traj,dtraj)
            x=translation(2);
            dx=dtranslation(2);
            y=translation(3);
            dy=dtranslation(3);
            phi=rotation(3);
            dphi=drotation(3);
            xp=traj(1);
            yp=traj(2);
            dxp=dtraj(1);
            dyp=dtraj(2);
            [omega11,omega21]=obj.compute_control_input(x,xp,dx,dxp,y,yp,dy,dyp,phi,dphi);
            omega1=max([min([omega11,obj.omega_max]),obj.omega_min]);
            omega2=max([min([omega21,obj.omega_max]),obj.omega_min]);
            if(obj.is_data_driven)
                F1=obj.data_driven_force(omega1,dx*cos(phi)+dy*sin(phi), ...
                    -dy*cos(phi)-dx*sin(phi)-obj.L*dphi);
                F2=obj.data_driven_force(omega2,dx*cos(phi)+dy*sin(phi), ...
                    -dy*cos(phi)-dx*sin(phi)+obj.L*dphi);
            else
                F1=obj.simple_force(omega1,dx*cos(phi)+dy*sin(phi),-dy*cos(phi)-dx*sin(phi));
                F2=obj.simple_force(omega2,dx*cos(phi)+dy*sin(phi),-dy*cos(phi)-dx*sin(phi));
            end
            u1=F1+F2;
            u2=obj.L/2*(F1-F2);
            ddtranslation=[0,u1*sin(phi)/obj.m,-obj.g+u1*cos(phi)/obj.m];
            ddrotation=[0,0,u2/obj.J];
        end
        
        function [omega1,omega2]=compute_control_input(obj,x,xp,dx,dxp,y,yp,dy,dyp,phi,dphi)
            u11=obj.m*obj.g+obj.K1*(y-yp)+obj.D1*(dy-dyp);
            phides=0.13*tanh(obj.K2*(x-xp)+obj.D2*(dx-dxp));
            u21=obj.K3*(phides-phi)-obj.D3*dphi;
            omega1=(u11+2*u21/obj.L)/2*obj.scale;
            omega2=(u11-2*u21/obj.L)/2*obj.scale;
        end
    end
    methods(Static)
        function force=data_driven_force(omega,vx,vy)
            force=-2*(0.0005917217101953038*vx^2-0.00002557102446913359*vx^4-...
            0.0009764562665337519*vy-0.00013859859899200091*vx^2*vy+...
            0.00004560884655456854*vy^3-7.893807214858706*10^(-8)*omega^2);
        end
        
        function force=simple_force(omega,~,~)
            force=1.5042947579635582*10^(-7)*omega^2;
        end
    end
end

