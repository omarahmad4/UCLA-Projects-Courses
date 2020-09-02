clear all;
close all;
clc;
train = load("regression_train.csv");
ETA = 0.05;
MAX_ITER = 0;
X = [ones(20,1), train(:,1)];
Y = train(:,2);
w = zeros(1,2);

for iter = 1:MAX_ITER
	temp = X*w' - Y;
	oldCost = mean(temp.^2);
	w(1) = w(1) - ETA * sum(temp.*X(:,1));
	w(2) = w(2) - ETA * sum(temp.*X(:,2));
	cost = mean((X*w' - Y).^2);
	if abs(oldCost - cost) < 0.0001
		break
	end
end

cost = mean((X*w' - Y).^2);
scatter(train(:,1), train(:,2))
n = [0:0.001:1];
hold on;
line = w(1) + w(2) * n;
plot(n, line)





