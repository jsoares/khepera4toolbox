function plot_run(folder_path)
% PLOT_RUN Display an animated plot of the specified calibration run.
% The files generated via the run_calibration.sh script should
% be located in the folder defined by the folder_path variable.
% PLOT_RUN(folder_path) takes the path to the folder containing the
% the generated data files. This folder should contains a
% 'positions.txt' file and the one or several 'rssi_robot_x.txt' files.

% Configuration variables
duration = 0.5;
% frame length
speed_up = 20;
% speed up
radius = 0.06;
% robots radius
colors = {'b-', 'g-', 'r-', 'k-'};
% robots colors

        % Load data
        [nrobots, t, X, Y, A, rssis] = extract_run(folder_path, true);

% Display the animation
figure(1);
for i = 0 : duration : t(end)
    nx = interp1(t, X, i);
ny = interp1(t, Y, i);
na = interp1(t, A, i);
clf;
hold on;

for j = 1 : nrobots
        % Plot robot
        draw_robot(nx(j), ny(j), na(j), j);

% Plot RSSIs
for k = 1 : nrobots
    if (k == j), continue;
end
rssi = interp1(rssis{j, k}
.t, rssis{j, k}
.values, i, 'nearest');
index = rssi(1);
rssi = rssi(2 : end);
if (isnan(index)), continue;
end
if (abs(rssis{j, k}
    .t(index) - i) > 0.5), continue;
end
% Plot rssi data(in the right color)
draw_rssi(rssi, nx(j), ny(j), na(j), k);
end
end

hold off;
axis equal;
axis([0 3 0 3]);
grid on;
title(sprintf('t = %.2f seconds', i));

drawnow;
pause(duration / speed_up);
end

function draw_robot(x_, y_, a_, id_)
circle_ = rsmak('circle');
c_ = fncmb(fncmb(circle_, [radius 0; 0 radius]), [x_; y_]);
p_ = fnplt(c_);
fill(p_(1, :), p_(2, :), [0.9 0.9 0.9], 'EdgeColor', [0.9 0.9 0.9]);

plot(p_(1, :), p_(2, :), colors {
    mod(id_ - 1, length(colors)) + 1});
dir_x_ = cos(a_)*[0 radius] + x_;
dir_y_ = sin(a_)*[0 radius] + y_;
plot(dir_x_, dir_y_, colors{mod(id_ - 1, length(colors)) + 1});
text(x_, y_, num2str(id_), 'FontSize', 9);
end

function draw_rssi(rssi_, x_, y_, a_, id2_)
for r_ = 1 : 8
        val_rssi_ = [0 0.2] * rssi_(r_) / 5000 + radius;
angle_ = a_ - 2 * pi / 8 * (r_ - 1);
dir_x0_ = cos(angle_) * val_rssi_ + x_;
dir_y0_ = sin(angle_) * val_rssi_ + y_;
dir_x1_ = cos(angle_ + pi / 8) * val_rssi_ + x_;
dir_y1_ = sin(angle_ + pi / 8) * val_rssi_ + y_;
dir_x2_ = cos(angle_ - pi / 8) * val_rssi_ + x_;
dir_y2_ = sin(angle_ - pi / 8) * val_rssi_ + y_;
fill([dir_x1_ dir_x0_(2) fliplr(dir_x2_) dir_x0_(1)], [dir_y1_ dir_y0_(2) fliplr(dir_y2_) dir_y0_(1)], colors {
    mod(id2_ - 1, 4) + 1
}, 'FaceAlpha', 0.5);
end
end
end