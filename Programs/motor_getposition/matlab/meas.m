meas1=load('meas1.txt');
meas2=load('meas2.txt');
meas3=load('meas3.txt');
meas4=load('meas4.txt');

figure(1);
subplot(2, 1, 1);
plot(meas1(1:400, 3));
title('Robot 1, motor 1');
xlabel('Sample');
ylabel('Measured speed');

subplot(2, 1, 2);
plot(meas1(1:400, 4));
title('Robot 1, motor 2');
xlabel('Sample');
ylabel('Measured speed');

figure(2);
subplot(2, 1, 1);
plot(meas3(1:400, 3));
title('Robot 2, motor 1');
xlabel('Sample');
ylabel('Measured speed');

subplot(2, 1, 2);
plot(meas3(1:400, 4));
title('Robot 2, motor 2');
xlabel('Sample');
ylabel('Measured speed');
