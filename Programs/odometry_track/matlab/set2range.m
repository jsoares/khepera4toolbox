function [angle1] = set2range(angle)
% SET2RANGE set angle to the range(-pi, pi]

        % 2000.02.11 Gilles Caprari EPFL - ASL

        angle1 = mod(angle, 2 * pi);
if angle1 > pi
    angle1 = angle1 - 2 * pi;
        end;
