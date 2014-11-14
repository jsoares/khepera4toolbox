function plotdata(fig, data, tit)

figure(fig);
clf;

maxamplitude=max(data(:, 2));
minamplitude=min(data(:, 2));
for i=1:size(data, 1);
	H=rectangle('Position', [data(i, 1)-0.5, data(i, 3)-0.05, 1, 0.1], 'Curvature', [1, 1]);
	c=1-(data(i, 2)-minamplitude)/(maxamplitude-minamplitude);
	set(H, 'FaceColor', [c, c, c])
	set(H, 'EdgeColor', [c, c, c])
end

title(tit);
xlabel('Time [s]');
ylabel('Distance [m]');
grid;
