#include <iostream>
#include "opencv2/opencv.hpp"

#include "mycanny.h"

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("circuit.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return -1;
	}

	Mat dst1, dst2;
	Canny(src, dst1, 50, 150);
	myCanny(src, dst2, 50, 150);

	imshow("src", src);
	imshow("OpenCV Canny", dst1);
	imshow("My Canny", dst2);

	waitKey();
	destroyAllWindows();
}