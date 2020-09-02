import cv2
path = r'C:\Users\omar_\Documents\pic1.jpg'
src = cv2.imread(path)
dst = cv2.cvtColor(src, cv2.COLOR_BGR2GREY)
cv2.imwrite('greypic1.jpg', dst)