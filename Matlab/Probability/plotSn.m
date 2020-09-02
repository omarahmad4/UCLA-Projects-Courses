function [] = plotSn(n)
%Sn = X1 + X2 + ... + Xn
%Let Xi be a uniform continuous random variable taking values in the interval
%(1, 4). Write a MATLAB program to plot the pdf and cdf of Sn. Consider
%n = 1; 3; 5; 10; 20; 30 and compare your results
x = zeros(1, 10000);
Sn = 0;
for i = 1:n
    for j = 1:10000
        x(j) = 3*rand + 1;
    end
    Sn = Sn + x;
end
pd = fitdist(Sn', 'Kernel');
xvals = 1*n:0.001*n:4*n;
PDF = pdf(pd, xvals);
CDF = cdf(pd, xvals);
calcNormal = makedist('Normal', 'mu', 2.5*n, 'sigma', sqrt(0.75*n));
calcPDF = pdf(calcNormal, xvals);
calcCDF = cdf(calcNormal, xvals);

subplot(2, 1, 1), plot(xvals, PDF, 'LineWidth', 2)
title(['PDF of Sn (n = ',num2str(n),')']);
xlabel('X') ;
ylabel('Probability Density');
hold on
plot(xvals, calcPDF, '--', 'LineWidth', 3.5)
hold off
legend('Sn', 'Gaussian', 'Location', 'northwest')

subplot(2, 1, 2), plot(xvals, CDF, 'LineWidth', 2)
title(['CDF of Sn (n = ',num2str(n),')']);
xlabel('X') ;
ylabel('Cumulative Probability');
hold on
plot(xvals, calcCDF, '--', 'LineWidth', 3.5)
hold off
legend('Sn', 'Gaussian', 'Location', 'northwest')
end

