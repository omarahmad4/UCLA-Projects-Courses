% Moving average Filter. Choose window lengths to be 1, 5, 10, 50, and 100, and play the output audio signal y[n] from the system corresponding to each window length. Report the window length that makes the output signal y[n] sound better, and explain the differences, based on what you hear, as you change the window lengths.



close all; 
clear all; 
clc;

WINDOW_LEN = 5


% Load the sound track
[X, Fs] = audioread('inception_sound_track.wav');

% -------------------------------------------------------------------------------
% Please enter your code here
% Design the system as shown in the figure that performs upsampling followed by a smoother

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
win_min = i-WINDOW_LEN;
if win_min <= 0
win_min = 1;
end
Y(i) = 1/WINDOW_LEN * sum(Y(win_min:i)); 
end
end


% -------------------------------------------------------------------------------
% Play the output signal Y (y[n]) from the system should be slower

ap_x = audioplayer(Y, Fs); % Play the output audio file with original sampling frequency
play(ap_x)

% Please attach/print your code to the homework submission