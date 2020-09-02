fs = 100;      %sampling freq
t = 0:1/fs:50; %time axis
x = 2*sin(2*pi*30*t) + 3*sin(2*pi*20*(t-2)) + 3*sin(2*pi*10*(t-4)); %signal
N = length(x);

omega = 2*pi*(0:N-1)/N;
omega = fftshift(omega);
omega = unwrap(omega-2*pi); %creating the frequency axis
X = fft(x,N) ; % compute N point DFT of x
X = X/max(X) ; % rescale the DFT
plot(omega,abs(fftshift(X)),'LineWidth',2); % center DFT at 0 and plot the magnitude
title ('DTFT of x[n]' , 'fontsize' ,14)
set (gca ,'fontsize' ,14)
xlabel( 'Radians' , 'fontsize' ,14)