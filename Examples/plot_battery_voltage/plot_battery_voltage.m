% Load the file
battery_voltage = load('matlab_battery_voltage');

% Plot it
figure(1);
clf;
plot(battery_voltage);
xlabel('Time [s]');
ylabel('Battery voltage [V]');
title('Battery voltage vs time');
grid;
