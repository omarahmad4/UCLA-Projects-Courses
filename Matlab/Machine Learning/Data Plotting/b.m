close;
clear;
%clc;
load data.csv;
%hold on;
num0 = 0;
num1 = 0;
mu0  = 0;
mu1  = 0;

for i = 1:length(data)
   if data(i,3) == 0
       num0 = num0 + 1;
       mu0 = mu0 + data(i,1:2);
   else 
       num1 = num1 + 1;
       mu1 = mu1 + data(i,1:2);
   end
end

Py0 = num0 / length(data);
mu0 = mu0  / num0;
mu1 = mu1  / num1;
%mu0 = mu0';
%mu1 = mu1';

Sigma = 0;
for i = 1:length(data)
   if data(i,3) == 0
       temp = data(i,1:2) - mu0;
       Sigma = Sigma + temp'*temp;
   else
       temp = data(i,1:2) - mu1;
       Sigma = Sigma + temp'*temp;
   end
end
Sigma = Sigma / length(data);
Sinv = inv(Sigma);

w = Sinv*(mu0 - mu1)';
B = -1/2 * (mu0*Sinv*mu0' - mu1*Sinv*mu1') + log (Py0/ (1-Py0));
%w = mu0*Sinv - mu1*Sinv;
%B = (mu1.^2 - mu0.^2)*Sinv/2 + log(Py0/(1-Py0));

hold on;
for i = 1:length(data)
   if data(i,3) == 1
       scatter(data(i,1),data(i,2),'b');
   else
       scatter(data(i,1),data(i,2),'r');    
   end
end

X = [-1:9];
Y = (-w(1)*X-B)/w(2);
plot(X,Y);
ylim([-7,2]);

G0 = gmdistribution(mu0, Sigma);
G1 = gmdistribution(mu1, Sigma);
F0 = @(x,y) pdf(G0,[x,y]);
contour0 = fcontour(F0);
contour0.LevelList = logspace(-3,-1,7);
F1 = @(x,y) pdf(G1,[x,y]);
contour1 = fcontour(F1);
contour1.LevelList = logspace(-3,-1,7);

%Y = w'*X;
%for i = 1:length(Y)
%    Y(:,i)  = Y(:,i) + B';
%end
%plot(Y(1,:), Y(2,:));
%Y = w'*X + B';

%plot(X, Y);
%Y = w*X + B;

disp('Sigma')
disp(Sigma)

