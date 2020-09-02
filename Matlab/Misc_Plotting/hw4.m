clear;
close;
clc;
for iter = -pi:0.001:pi
    if abs(iter) <= pi/2
        X(floor(1000*iter) + 3143) = 1;
    else
        X(floor(1000*iter) + 3143) = 0;
    end
end

%N = 7;
omega = -pi:0.001:pi;
index = 1;
for N = [1 3 5 10 20 50]
    aprox = 1/2; %const for n = 0
    for n = 1:N %add the n and the -n for n from 1 to N 
        aprox = aprox + sin(pi/2*n)/(pi*n)*exp(-i*omega*n) + sin(pi/2*-n)/(pi*-n)*exp(i*omega*n);
    end
    


    subplot(6,1,index);
    index = index + 1;
    plot(omega, X);
    hold on;
    plot(omega, aprox);
    titulo = sprintf("N = %d", N);
    title(titulo);
end


%omega = -pi:pi/N:pi;
%omega(N+1) = [];


%sin(pi*n/2)/(pi*n).*exp(-i*omega.*n);
%aprox = 1/2 + sum(over n) (sin(pi*n/2)/(pi*n).*exp(-i*omega.*n));
%aprox = 1/2 + sum((sin(pi*n/2)/(pi*n).*exp(-i*n)))*exp(omega);
