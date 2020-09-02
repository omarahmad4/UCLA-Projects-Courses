%Exponential smoother. Report the best  = 0; 0:3; 0:5; 0:8; 1 that makes the output signal y[n] sounds better, and explain the diferences with the other values of.
close all; 
clear all; 
clc;

% Load the sound track
[X, Fs] = audioread('inception_sound_track.wav');

% -------------------------------------------------------------------------------
% Please enter your code here
% Design the system as shown in the figure that performs upsampling followed by a smoother

ALPHA = 0.8
% Upsampling
for i = 1:3*size(X,1)
if mod(i,3) == 0
Y(i) = X(i/3);
else
Y(i) = 0;
end
end


% Smoother using moving average and exponential smoother
for i = 1:size(Y,2)
if Y(i) == 0
if i > 2
Y(i) = ALPHA * Y(i-1) + (1-ALPHA) * Y(i-2);
end
end
end

%for i = 1:3*size(X,1)
%if mod(i,3) == 0
%Y(i) = X(i/3);
%else
%if i > 5
%Y(i) = ALPHA * X(floor(i/3)) + (1-ALPHA) * X(floor(i/3) - 1);
%else
%Y(i) = X(1);
%end
%end
% end

% -------------------------------------------------------------------------------
% Play the output signal Y (y[n]) from the system should be slower

ap_x = audioplayer(Y, Fs); % Play the output audio file with original sampling frequency
play(ap_x)

% Please attach/print your code to the homework submission