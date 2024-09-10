# canny
OpenCV의 Canny 함수와 가우시안 필터 + 그래디언트 + 비최대 억제를 적용한 myCanny 함수와 비교

![image](https://github.com/user-attachments/assets/5aec2e57-fdca-4540-990f-5d10f6339b62)
# houghcircle
HoughCircles 함수로 그림의 코인 테두리 원 추출(HOUGH_GRADIENT 와 HOUGH_GRADIENT_ALT 비교)

![image](https://github.com/user-attachments/assets/d11b0a80-5982-4f5b-87db-4c13b066af5d)

# CoinCounter
-. 이미지를 HSV 색 공간으로 변경하여 hsv 이미지에 저장
-. hsv 이미지를 split() 함수로 세 개의 플레인 hsv_planes로 나누기
-. HUE에 해당하는 hsv_planes[0]를 uchar가 아닌 16비트 또는 32비트 정수형으로 변환 후 
   여기에 40을 더한 후 평균(mean_of_hue)을 계산(mask 이미지 활용)
-. 평균 mean_of_hue가 90보다 작으면 10원, 90보다 크면 100원으로 간주

![CoinCounter1-min](https://github.com/user-attachments/assets/783f9e04-7215-4879-b62f-76abedd291aa)
