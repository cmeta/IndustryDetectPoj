#ifndef _SEGMENTAION_H_
#define _SEGMENTAION_H_

#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int image_registration(cv::Mat baseimg, cv::Mat currimg, int &x, int &y);

int segmentation_with_dilate(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x = 0, int dilate_y = 0, float thresh = 0.7);
int segmentation_chinese(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x = 0, int dilate_y = 0, float thresh = 0.7);
int segmentation_number(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x = 0, int dilate_y = 0, float thresh = 0.7);

int segmentation_number_vector(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, float thresh);

int row_segmentation(cv::Mat image, std::vector<int> &start_point, std::vector<int> &end_point, int row_num, int dilate_x, int dilate_y,int angle);

int layout_analysis_with_skewcorrect(cv::Mat image, int dilate_x, int dilate_y, int &top, int &bottom, int &left, int &right);
int layout_analysis(cv::Mat image, int dilate_x, int dilate_y, int &top, int &bottom, int &left, int &right);

int connect_analysis(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point);
int connect_analysis_number(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point);
int project_method(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point);

void count_colsum(cv::Mat image, std::vector<int> &colsum);
void count_rowsum(cv::Mat image, std::vector<int> &rowsum);
void count_dif_colsum(cv::Mat image, std::vector<int> &dif_colsum);
void count_dif_rowsum(cv::Mat image, std::vector<int> &dif_rowsum);
void guass_filter(std::vector<int> &dif_rowsum, int len);

//图像旋转与倾斜校正
void rotate90(cv::Mat& src);
void rotate180(cv::Mat& src);
void rotate270(cv::Mat& src);
int rotate_correction(cv::Mat& src, int angle);
void rotate(cv::Mat src, cv::Mat &dst, double angle);
int skew_correction(cv::Mat &src, int &top, int &bottom);

void resize_image_keepratio(cv::Mat& src, int max_width, int max_height);
void fill_background(cv::Mat& src);


#endif
