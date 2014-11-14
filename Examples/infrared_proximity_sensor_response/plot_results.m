% Load the data of both front sensors
data5=load('matlab_run_results_5');
data6=load('matlab_run_results_6');

% Scale the distance
distance_per_step = 0.0045;
x5=(data5(:, 1) - 1) * distance_per_step;
x6=(data6(:, 1) - 1) * distance_per_step;

% Calculate the mean sensor values
y5=mean(data5(:, 2:end), 2);
y6=mean(data6(:, 2:end), 2);

% Plot
figure(2);
clf;
plot(x5, y5, 'r');
hold on;
plot(x6, y6, 'b');
grid;
xlabel('Distance [m]');
ylabel('Sensor value');
legend({'Sensor 5', 'Sensor 6'});

% This is the data that can be written to a text file for use in other programs
txtdata = [x5, (y5 + y6) / 2];
