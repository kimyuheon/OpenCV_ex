#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat calcHueHist(const Mat& hue, const Mat& mask);
Mat getHueHistImage(const Mat& hist);

Scalar HSV2BGR(double h, double s, double v);
void putTextCenter(Mat& img, string text);

int main(int argc, char* argv[])
{
	string filename = "coins1.jpg";

	if (argc > 1)
		filename = argv[1];

	Mat src = imread(filename);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return -1;
	}

	Mat gray, blr;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, blr, Size(), 1.0);

	vector<Vec3f> circles;
	HoughCircles(blr, circles, HOUGH_GRADIENT, 1, 50, 150, 40, 20, 80);

	if (circles.empty()) {
		cout << "No circles are detected!" << endl;
		return 0;
	}

	int sum_of_money = 0;
	Mat dst = src.clone();

	imshow("dst", dst);
	//moveWindow("dst", 200, 200);
	waitKey();

	for (auto& circle : circles) {
		int cx = cvRound(circle[0]);
		int cy = cvRound(circle[1]);
		int radius = cvRound(circle[2]);

		int x1 = cx - radius;
		int y1 = cy - radius;
		int diameter = radius * 2;

		// 1. circle을 감싸는 바운딩 박스의 부분 이미지 추출하여 crop에 저장

		Rect box(x1, y1, diameter, diameter);

		Mat crop;
		crop = dst(box);

		// 2. crop 이미지에서 중앙의 원 안쪽은 흰색(255), 바깥쪽은 검정색(0)으로 채워진 마스크 이미지 mask 생성
		
		Mat mask(crop.size(), CV_8UC1, Scalar(0));

		Point center(crop.cols / 2, crop.rows / 2);

		cv::circle(mask, center, radius, Scalar(255), -1);

		// 3. crop 이미지를 HSV 색 공간으로 변경하여 hsv 이미지에 저장
		
		Mat hsv;
		cvtColor(crop, hsv, COLOR_BGR2HSV);

		// 4. hsv 이미지를 split() 함수로 세 개의 플레인 hsv_planes로 나누기

		vector<Mat> hsv_planes;
		split(hsv, hsv_planes);

		// 5. HUE에 해당하는 hsv_planes[0]를 uchar가 아닌 16비트 또는 32비트 정수형으로 변환하고, 여기에 40을 더한 후 평균(mean_of_hue)을 계산(mask 이미지 활용)

		Mat hsv_planes_hue_16bit;

		hsv_planes[0].convertTo(hsv_planes_hue_16bit, CV_16UC1, 1.0);

		hsv_planes_hue_16bit += Scalar(40);

		Scalar mean_hsv_planes_hue = mean(hsv_planes_hue_16bit, mask);
		int16_t mean_of_hue = (int16_t)mean_hsv_planes_hue[0];

		// 평균 mean_of_hue가 90보다 작으면 10원, 90보다 크면 100원으로 간주.
		// sum_of_money에 해당 동전 금액을 더함

		int won = 100;
		if (mean_of_hue < 90)
			won = 10;

		sum_of_money += won;

		// 부분 이미지의 중앙에 현재 동전의 금액을 출력
		// dst 이미지에 현재 동전의 테두리(원)를 빨간색으로 그리기

		putTextCenter(crop, format("%d", won));
		cv::circle(dst, Point(cx, cy), radius, Scalar(0, 0, 255), 2, LINE_AA);

		// Hue 히스토그램 이미지 생성
		Mat hist = getHueHistImage(calcHueHist(hsv_planes[0], mask));
		
		imshow("dst", dst);
		imshow("hist", hist);
		waitKey();
	}

	destroyWindow("hist");

	putText(dst, format("Total: %d won", sum_of_money), Point(20, 50),
		FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2, LINE_AA);

	imshow("dst", dst);
	waitKey();
	destroyAllWindows();
}

Mat calcHueHist(const Mat& hue, const Mat& mask)
{
	CV_Assert(hue.type() == CV_8U);

	Mat hist;
	int channels[] = {0};
	int dims = 1;
	const int histSize[] = {180};
	float graylevel[] = {0, 180};
	const float* ranges[] = {graylevel};

	calcHist(&hue, 1, channels, mask, hist, dims, histSize, ranges);

	return hist;
}

Mat getHueHistImage(const Mat& hist)
{
	CV_Assert(hist.type() == CV_32F);
	CV_Assert(hist.size() == Size(1, 180));

	double histMax = 0.;
	minMaxLoc(hist, 0, &histMax);

	Mat imgHist(100, 180, CV_8UC3, Scalar(255, 255, 255));
	for (int i = 0; i < 180; i++) {
		Scalar bgr = HSV2BGR(i * 2, 1, 1);
		line(imgHist, Point(i, 100),
			Point(i, 100 - cvRound(hist.at<float>(i) * 100 / histMax)), bgr);
	}

	resize(imgHist, imgHist, Size(), 2, 1.5, INTER_NEAREST);
	copyMakeBorder(imgHist, imgHist, 20, 20, 20, 20, BORDER_CONSTANT, Scalar(255, 255, 255));

	return imgHist;
}

Scalar HSV2BGR(double h, double s, double v)
{
	int i = static_cast<int>(h / 60) % 6;
	double f = (h / 60) - i;
	double p = v * (1 - s);
	double q = v * (1 - f * s);
	double t = v * (1 - (1 - f) * s);

	double r, g, b;

	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	// Convert from [0, 1] to [0, 255]
	Scalar bgr;
	bgr[0] = static_cast<int>(b * 255);
	bgr[1] = static_cast<int>(g * 255);
	bgr[2] = static_cast<int>(r * 255);

	return bgr;
}

void putTextCenter(Mat& img, string text)
{
	int fontFace = FONT_HERSHEY_SIMPLEX;
	double fontScale = 0.8;
	int thickness = 1;

	Size sizeText = getTextSize(text, fontFace, fontScale, thickness, 0);
	Size sizeImg = img.size();

	Point org((sizeImg.width - sizeText.width) / 2, (sizeImg.height + sizeText.height) / 2);
	putText(img, text, org, fontFace, fontScale, Scalar(255, 0, 0), thickness, LINE_AA);
}
