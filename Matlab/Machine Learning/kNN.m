clear all;
close all;
clc;
trainX = load("dataTraining_X.csv");
trainY = load("dataTraining_Y.csv");
testX = load("dataTesting_X.csv");
testY = load ("dataTesting_Y.csv");

testPts = trainX;    %could be trainX or testX
testLabels = trainY; %the Y version of whatever is above
testSize = size(testLabels,1);
trainSize = size(trainY,1);
dists = zeros(testSize, trainSize);
prediction = zeros(testSize, 1);

YTIE = 0;
K = 1;

for YTIE = 0:1
	for K = 1:15
		for i = 1:testSize  %for all test pts
			for j = 1:trainSize  %for all neighbors
				dists(i,j) = sum((testPts(i,:) - trainX(j,:)).^2); %don't need to take sqrt so not actually the dist
			end
			[nn, I] = mink(dists(i,:), K); %nn is the smallest K values and I is their indices
			sumLabels = 0;
			for j = 1:size(I,2)
				sumLabels = sumLabels + trainY(I(1,j));
			end
			if sumLabels > K/2
				prediction(i) = 1;
			elseif sumLabels < K/2 
				prediction(i) = 0;
			else %sumLabels == K/2 (tie)
				prediction(i) = YTIE;
			end
		end

		%prediction accuracy measurement:
		errors = sum(abs(testLabels - prediction));
		predictionAccuracy(K) = (testSize - errors)/testSize;
	end

	k = 1:15;
	plot(k, predictionAccuracy)
	xlabel('k')
	ylabel("Prediction Accuracy")
	hold on;
	predictionAccuracy(:)
end
legend('Ytie = 0', 'Ytie = 1')


	