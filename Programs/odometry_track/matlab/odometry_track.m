function trackposition()

wheeldistance=0.08841;
wheelradius=0.021;
stepspertour=2764.8;
conversion=2*pi*wheelradius/stepspertour;

pos=load('pos.txt');
mot1=pos(:, 1)*conversion;
mot2=pos(:, 2)*conversion;

x(1)=0;
y(1)=0;
theta=0;
for i=2:length(mot1)
	% Integrate robot position
	deltaSl=mot1(i)-mot1(i-1);
	deltaSr=mot2(i)-mot2(i-1);
	deltax=(deltaSr+deltaSl)/2*cos(theta);
	deltay=(deltaSr+deltaSl)/2*sin(theta);
	x(i)=x(i-1)+deltax;
	y(i)=y(i-1)+deltay;
	theta=(mot2(i)-mot1(i))/wheeldistance;
	theta=set2range(theta);
end

% Plot
figure(1);
plot(x, y, 'b');
axis equal;

figure(2);
plot(mot1, 'b');
hold on;
plot(mot2, 'b');
hold off;

figure(3);
k3x=pos(:, 3);
k3y=pos(:, 4);
plot(k3x, k3y, 'r');
axis equal;
