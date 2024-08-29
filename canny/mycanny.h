#pragma once

#include "opencv2/core.hpp"

void myCanny(const cv::Mat& src, cv::Mat& dst, double threshold1, double threshold2);