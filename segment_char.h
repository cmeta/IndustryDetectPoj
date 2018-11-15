#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int segment_solid_number(cv::Mat& Img, cv::Size& Kernel, const int& length, const int& charNum, std::vector<cv::Mat>& charImageSet);
int filterCharImage(cv::Mat& charImage);
bool mergeUpDownBbox(cv::Rect& rect1, cv::Rect& rect2, int threshX, int threshY);
bool mergeLeftRightBbox(cv::Rect& rect1, cv::Rect& rect2, int thres);



