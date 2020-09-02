clear all;
close all;
clc;
train = load("regression_train.csv");

M = 10
Y = train(:,2);
tempX = train(:,1);
phi(1,:) = ones(1,20);
for i = 2:M+1
	phi(i,:) = (tempX'.^i);
end
w = (inv(phi'*phi)*phi')'*Y

cost = sqrt(norm(phi'*w - Y)/20)
scatter(train(:,1), train(:,2))
n = [0:0.001:0.95];
hold on;
line = 0;
for i = 1:M+1
	line = line + w(i) * (n.^(i-1)) ;
end
plot(n, line)

test = load("regression_test.csv");
tstY = test(:,2);
tstphi(1,:) = ones(1,20);
for i = 2:M+1
	tstphi(i,:) = (test(:,1)'.^i);
end
tstcost = sqrt(norm(tstphi'*w - tstY)/20);



%end



