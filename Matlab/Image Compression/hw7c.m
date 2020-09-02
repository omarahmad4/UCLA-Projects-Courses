clear;
close;
%clc;

img = double(imread('UCLA_Bruin.jpg'));
K = 320;
numIter = 5;
u(1,:) = double(squeeze(img(1,1,:)))';
numMeans = 1;

for k = 2:K
    for i = 1:numMeans
        %dist of all pixels to ith mu (u)
        dist(:,:,i) = sqrt((img(:,:,1) - u(i,1)).^2 + (img(:,:,2) - u(i,2)).^2 + (img(:,:,3) - u(i,3)).^2);
    end
    % now know dist of every pixel to every mu (that currently exists)
    
    % the min dist for each pixel (to the closest u)
    minDists = min(dist,[],3);
    
    [M,I] = max(minDists);
    [N,J] = max(M); %N is largest elem of minDists
    Z = I(J);
    if(N ~= minDists(Z,J))
        disp("ERROR!");
    end
    %dist(Z,J) is the max dist
    %img(Z,J,:) is the RGB for that pixel
    
    newMeanIndex = numMeans+1;
    u(newMeanIndex,1) = img(Z,J,1);
    u(newMeanIndex,2) = img(Z,J,2);
    u(newMeanIndex,3) = img(Z,J,3);
    numMeans = numMeans + 1;
end
% now initialized (k mu's set by furthest-first rule)


%each iter do assignment and re-estimation of center of cluster
%and also calc loss func for tracking the alg 
Loss = zeros(1,numIter);
for iter = 1:numIter
    %assignment
    for k = 1:K
        dist(:,:,k) = sqrt((img(:,:,1) - u(k,1)).^2 + (img(:,:,2) - u(k,2)).^2 + (img(:,:,3) - u(k,3)).^2);      
    end
    for i = 1:size(img,1)
        for j = 1:size(img,2)
            [M,I] = min(dist(i,j,:));
            assign(i,j) = I;
        end
    end
    
    %re-estimation
    % pts(i,:) is the sum of the RGB elem of all pts in img w/ assign() = i
    pts = zeros(K,3); 
    numPts = zeros(1,K);
    for i = 1:size(img,1)
       for j = 1:size(img,2)
           tmp = assign(i,j);
           pts(tmp,:) = pts(tmp,:) + squeeze(img(i,j,:))';
           numPts(tmp) = numPts(tmp) + 1;
       end
    end
    for k = 1:K
       u(k,:) = pts(k,:)/numPts(k);
    end
     
    %loss func
    for i = 1:size(img,1)
        for j = 1:size(img,2)
            tmp = assign(i,j);
            Loss(iter) = Loss(iter) + norm(squeeze(img(i,j,:))' - u(tmp,:));           
        end
    end
end

newImage = img;
for i = 1:size(img,1)
    for j = 1:size(img,2)
        newImage(i,j,1) = u(assign(i,j), 1);
        newImage(i,j,2) = u(assign(i,j), 2);
        newImage(i,j,3) = u(assign(i,j), 3);
    end
end
imshow(uint8(newImage));

