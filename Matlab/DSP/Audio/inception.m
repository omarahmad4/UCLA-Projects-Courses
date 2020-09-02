close all; 
clear all; 
clc;

% Load the sound track
[X, Fs] = audioread('inception_sound_track.wav');

% -------------------------------------------------------------------------------
% Please enter your code here
% Design the system as shown in the figure that performs upsampling followed by a smoother

% Upsampling


% Smoother using moving average and exponential smoother
for i = 1:3*size(X,1)
if mod(i,3) == 0
Y(i) = X(i/3);
else
Y(i) = 0;
end
end



% -------------------------------------------------------------------------------
% Play the output signal Y (y[n]) from the system should be slower

ap_x = audioplayer(Y, Fs); % Play the output audio file with original sampling frequency
play(ap_x)

% Please attach/print your code to the homework submission