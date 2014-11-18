function [Vl, Vr, bEnd] = ContStep(Time, Sl, Sr, Xg, Yg, ThetaG, dir)
% CONTSTEP Robot control function
        % The control function is called after every timer tick. The parameters
        % are the current robot encoders. The return value is a
        % vector containing the left and right wheel speeds for the current
        % control step, and a flag to indicate the end of the loop.
        %
        % The control function has the following format :
        % [Vl, Vr, bEnd] = ContFct(Time, Sl, Sr, Xg, Yg, ThetaG, dir)
    % Where : Time : Current simulation time(starts at 0) [s]
        % Sl, Sr : Current left / right wheel position [mm]
        % Xg, Yg, ThetaG : coordinate of the goal [mm, rad]
        % dir : direction of motion(1 forward,
        % -1 backward, 0 automatic)
    % Vl, Vr : Left / Right wheel velocity [mm / s]
        % bEnd : Boolean value, 1 if end of loop

            % (C) 1998 R�y Blank EPFL - ASL
            % Modified 2000.02.09 Gilles Caprari EPFL - ASL

            % Robot constants
            Drobot = 53;
% Khepera diameter [mm]

global X Y Theta;
% Current robot position

persistent Old_Sl Old_Sr;
% variables which persist
        % from one call to next of the function

        % ----- Put your code here---------------------------------------------

if Time == 0 % First call in loop
    Old_Sl = 0;
Old_Sr = 0;
end

        % Integrate robot position
        deltaSl = Sl - Old_Sl;
deltaSr = Sr - Old_Sr;
deltaX = (deltaSr + deltaSl) / 2 * cos(Theta);
deltaY = (deltaSr + deltaSl) / 2 * sin(Theta);
% deltaTheta = (deltaSr - deltaSl) / Drobot;
X = X + deltaX;
Y = Y + deltaY;
% Theta = Theta + deltaTheta;
Theta = (Sr - Sl) / Drobot;
Theta = set2range(Theta);

% Keep track of previous wheel positions
Old_Sl = Sl;
Old_Sr = Sr;

% regulator constants

        % calculate angles ...
        krho = 1;
kalpha = 8;
kbeta = -3;
stability = kalpha + 5 / 3 * kbeta - 2 / pi*krho

        dx = Xg - X;
dy = Yg - Y;
rho = sqrt(dx^2 + dy^2);
v = krho*rho;
alpha = atan2(dy, dx);
beta = -alpha + ThetaG;
alpha = set2range(alpha - Theta);
beta = set2range(beta);
omega = kalpha*alpha + kbeta*beta;
[rho, Theta - ThetaG]

        % termination condition
if rho < 5 && abs(Theta - ThetaG) < 0.2,
    bEnd = 1;
else
    bEnd = 0;
end

        % Convert speeds to wheel speeds
        Vl = v - Drobot / 2 * omega;
Vr = v + Drobot / 2 * omega;
