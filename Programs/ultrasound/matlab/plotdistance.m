function plotdistance()

figure(1);
clf;
hold on;

plotdistanceone(20, load('data20cm.txt'), 200);
plotdistanceone(21, load('data21cm.txt'), 200);
plotdistanceone(22, load('data22cm.txt'), 200);
plotdistanceone(23, load('data23cm.txt'), 200);
plotdistanceone(24, load('data24cm.txt'), 200);
plotdistanceone(25, load('data25cm.txt'), 200);
plotdistanceone(26, load('data26cm.txt'), 200);
plotdistanceone(30, load('data30cm.txt'), 200);
plotdistanceone(40, load('data40cm.txt'), 200);
plotdistanceone(50, load('data50cm.txt'), 200);
plotdistanceone(60, load('data60cm.txt'), 200);
plotdistanceone(66, load('data66cm.txt'), 200);

title('Real vs. measured distance');
xlabel('Real distance');
ylabel('Measured distance');
grid;
hold off;

function plotdistanceone(dist, data, thres)

sel=find(data(:, 2)>thres);
data=data(sel, :);

maxamplitude=max(data(:, 2));
minamplitude=min(data(:, 2));
for i=1:size(data, 1);
	H=rectangle('Position', [dist-0.5, data(i, 3)-0.01, 1, 0.02], 'Curvature', [1, 1]);
	c=1-(data(i, 2)-minamplitude)/(maxamplitude-minamplitude);
	set(H, 'FaceColor', [c, c, c])
	set(H, 'EdgeColor', [c, c, c])
end
