% !scp root@192.168.1.2 : / root / motget.txt .

        meas = load('motget.txt');

figure(1);
subplot(2, 1, 1);
plot(diff(meas( :, 1)));
title('Robot 1, motor 1');
xlabel('Sample');
ylabel('Measured speed');
% a = axis;
% a(3) = -50;
% a(4) = 50;
% axis(a);

subplot(2, 1, 2);
plot(diff(meas( :, 2)));
title('Robot 1, motor 2');
xlabel('Sample');
ylabel('Measured speed');
% a = axis;
% a(3) = -50;
% a(4) = 50;
% axis(a);
