% Load the data
data=load('matlab_run_results');

% Sort
data=sortrows(data, 1);
real_speed=data(:, 2);
speed_command=data(:, 1);
speed_factor=real_speed./abs(speed_command);

% Plot the real speed
plot(speed_command, real_speed, 'b');
title('Real speed');
xlabel('Speed command');
grid;

% Plot the speed factor
plot(speed_command, speed_factor, 'b');
title('Speed factor');
xlabel('Speed command');
grid;

% Calculate the mean speed factor
speed_factor_mean=mean(speed_factor);
disp(['Mean speed factor: ' num2str(speed_factor_mean)]);
