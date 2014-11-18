function mygoto(xt, yt)

figure(1);
clf;
hold on;

wheeldistance = 0.089;

x = 0;
y = 0;
theta = 0;

prevdelta1 = 0;
prevdelta2 = 0;

for i = 1 : 1000
        dx = xt - x;
dy = yt - y;
dist = sqrt(dx^2 + dy^2);
angle = atan2(dy, dx) - theta;
while (angle > pi)
    angle = angle - 2 * pi;
end
while (angle<-pi)
    angle = angle + 2 * pi;
end

speedfactor = dist * 11;
if speedfactor > 2, speedfactor = 2;
end
logdist(i) = speedfactor;
delta1 = speedfactor*(pi - 2 * abs(angle) + angle);
delta2 = speedfactor*(pi - 2 * abs(angle) - angle);
delta1 = floor(10 * delta1 + 0.5);
delta2 = floor(10 * delta2 + 0.5);
if delta1 > prevdelta1, delta1 = prevdelta1 + 1;
end
if delta1 < prevdelta1, delta1 = prevdelta1 - 1;
end
if delta2 > prevdelta2, delta2 = prevdelta2 + 1;
end
if delta2 < prevdelta2, delta2 = prevdelta2 - 1;
end
prevdelta1 = delta1;
prevdelta2 = delta2;
if abs(delta1) < 10, delta1 = 0;
end
if abs(delta2) < 10, delta2 = 0;
end
logdelta1(i) = delta1;
logdelta2(i) = delta2;

delta1 = delta1 * 0.0001;
delta2 = delta2 * 0.0001;
deltatheta = (delta1 - delta2) / wheeldistance;
x = x + (delta1 + delta2) / 2 * cos(theta + deltatheta / 2);
y = y + (delta1 + delta2) / 2 * sin(theta + deltatheta / 2);
theta = theta + deltatheta;
plot(x, y, '.');
end

grid;
axis equal;
hold off;

figure(2);
plot(logdelta1, 'b');
hold on;
plot(logdelta2, 'r');
plot(logdist, 'k');
hold off;
