function plot_verification(data)

angle=data(:, 1);
wind_angle=data(:, 2:2:end);
likelihood=data(:, 3:2:end);

for i=1:length(angle)
	
	wind_angle_hist=hist(wind_angle, 0.5:1:360);
end
