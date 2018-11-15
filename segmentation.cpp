#include "segmentation.h"
#include "segment_char.h"
#include<iostream>
#include <time.h>

//用相位相关法计算图像相对于基准图像的偏移
int image_registration(cv::Mat baseimg, cv::Mat currimg, int &x, int &y)
{

	//输入图像必须为单通道图像，且两幅图像大小必须相等
	if (baseimg.channels() != 1 || currimg.channels() != 1 || baseimg.rows != currimg.rows || baseimg.cols != currimg.cols)
	{
		return -1;
	}

	//phaseCorrelate函数只能处理32位或64位浮点型的单通道图像
	cv::Mat src1, src2;
	baseimg(cv::Rect(baseimg.cols / 4, baseimg.rows / 4, baseimg.cols / 2, baseimg.rows / 2)).convertTo(src1, CV_32FC1);
	currimg(cv::Rect(currimg.cols / 4, currimg.rows / 4, currimg.cols / 2, currimg.rows / 2)).convertTo(src2, CV_32FC1);
	
	//baseimg(cv::Rect(150, 30, 420, 240)).convertTo(src1, CV_32FC1);
	//currimg(cv::Rect(150, 30, 420, 240)).convertTo(src2, CV_32FC1);

	cv::Mat hann;
	createHanningWindow(hann, src2.size(), CV_32FC1);

	cv::Point2d phase_shift;
	phase_shift = cv::phaseCorrelate(src1, src2, hann);

	x = int(phase_shift.x);
	y = int(phase_shift.y);

	return 0;
}
//计算差分列和
void count_dif_colsum(cv::Mat image, std::vector<int> &dif_colsum)
{
}
//计算差分行和
void count_dif_rowsum(cv::Mat image, std::vector<int> &dif_rowsum)
{
	std::vector<int> rowsum(image.rows);
	for (int i = 0; i < image.rows; i++)
	{
		rowsum[i] = 0;
	}

	uchar *pdata;
	for (int i = 0; i < image.rows; i++)
	{
		pdata = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols; j++)
		{
			if (pdata[j] == 255)//黑底白字
			{
				rowsum[i] ++;
			}
		}
	}

	for (int i = 0; i < image.rows; i++)
	{
		dif_rowsum[i] = 0;
	}

	dif_rowsum[0] = rowsum[0];
	for (int i = 0; i < image.rows - 1; i++)
	{
		dif_rowsum[i + 1] = rowsum[i + 1] - rowsum[i];
	}
}

//计算差分行和的积分
int count_dif_rowsum(cv::Mat image)
{
	std::vector<int> rowsum(image.rows);
	for (int i = 0; i < image.rows; i++)
	{
		rowsum[i] = 0;
	}

	uchar *pdata;
	for (int i = 0; i < image.rows; i++)
	{
		pdata = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols; j++)
		{
			if (pdata[j] == 255)//黑底白字
			{
				rowsum[i] ++;
			}
		}
	}

	int sum = 0;
	for (int i = 0; i < image.rows - 1; i++)
	{
		sum += abs(rowsum[i + 1] - rowsum[i]);
	}
	return sum;
}

//一维高斯滤波，核大小为3
void guass_filter(std::vector<int> &dif_rowsum, int len)
{
	std::vector<int> temp(len);

	for (int i = 0; i < len; i++)
	{
		if (i == 0)
		{
			temp[i] = (2 * dif_rowsum[i] + 1 * dif_rowsum[i + 1]) / 4;
		}
		else if (i == len - 1)
		{
			temp[i] = (2 * dif_rowsum[i] + 1 * dif_rowsum[i - 1]) / 4;
		}
		else
		{
			temp[i] = (2 * dif_rowsum[i] + 1 * dif_rowsum[i - 1] + 1 * dif_rowsum[i + 1]) / 4;
		}
	}
	for (int i = 0; i < len; i++)
	{
		dif_rowsum[i] = temp[i];
	}
}
//计算行和
void count_rowsum(cv::Mat image, std::vector<int> &rowsum)
{
	for (int i = 0; i < image.rows; i++)
	{
		rowsum[i] = 0;
	}

	uchar *pdata;
	for (int i = 0; i < image.rows; i++)
	{
		pdata = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols; j++)
		{
			if (pdata[j] == 255)//黑底白字
			{
				rowsum[i] ++;
			}
		}
	}
}

//计算列和
void count_colsum(cv::Mat image, std::vector<int> &colsum)
{
	for (int i = 0; i < image.cols; i++)
	{
		colsum[i] = 0;
	}

	uchar *pdata;
	for (int i = 0; i < image.rows; i++)
	{
		pdata = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols; j++)
		{
			if (pdata[j] == 255)//黑底白字
			{
				colsum[j] ++;
			}
		}
		
	}
}

//分割行,输入为2值图，返回每一行的起点和终点
int row_segmentation(cv::Mat image, std::vector<int> &start_point, std::vector<int> &end_point, int row_num, int dilate_x, int dilate_y, int angle)
{
	cv::medianBlur(image, image, 3);

	cv::Mat binary_img;
	cv::threshold(image, binary_img, 0, 255, CV_THRESH_OTSU);
	binary_img = 255 - binary_img;

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
	dilate(binary_img, binary_img, element);

	for (int j = 0; j < row_num; j++)
	{
		end_point[j] = 0;
	}
	
	//计算行和，并进行高斯滤波
	std::vector<int> rowsum(binary_img.rows);
	count_rowsum(binary_img, rowsum);
	//guass_filter(rowsum, image.rows);

	//估计每一行的宽度
	int length = binary_img.rows / row_num;

	int start = 0;
	int end = length;
	for (int j = 0; j < row_num; j++)
	{
		//查找字符上边界,从起始点开始找第一非零值
		for (int i = start; i < end - 5; i++)
		{
			if (i == end - 6)
			{
				//std::cout << "行数目缺失" << std::endl;
				return -1;
			}
			if (rowsum[i] > 0 && rowsum[i + 1] > 0 && rowsum[i + 2] > 0 && rowsum[i + 3] > 0 && rowsum[i + 4] > 0 && rowsum[i + 5] > 0)
			{
				start_point[j] = i;
				break;
			}
		}
		//查找字符下边界,从上边界开始找第一为零值
		start = (start_point[j] + 1 < binary_img.rows - 2) ? start_point[j] + 1 : binary_img.rows - 2;
		end = (int(start_point[j] + 1.2*length) <  binary_img.rows - 1) ? int(start_point[j] + 1.2*length) : binary_img.rows - 1;
		for (int i = start; i < end; i++)
		{
			if (rowsum[i] == 0 && rowsum[i + 1] <= 2 && (i - start) > 5) //连续两个值为0视为下边界
			{
				end_point[j] = i;
				break;
			}
		}
		//若未找到下边界，则选择区间内最小值作为右边界
		if (end_point[j] == 0)
		{
			if (j == row_num - 1)
			{
				end_point[j] = binary_img.rows - 1;
			}
			else
			{
				start = (int(start_point[j] + 0.8*length) < binary_img.rows - 1) ? int(start_point[j] + 0.8*length) : binary_img.rows - 1;
				end = (int(start_point[j] + 1.2*length) < binary_img.rows) ? int(start_point[j] + 1.2*length) : binary_img.rows;

				int min = rowsum[start];
				end_point[j] = start;

				for (int i = start; i < end; i++)
				{
					if (rowsum[i] < min)
					{
						min = rowsum[i];
						end_point[j] = i;
					}
				}
			}
		}
		start = (end_point[j] + 1 < binary_img.rows - 1) ? end_point[j] + 1 : binary_img.rows - 1;
		end = (end_point[j] + length < binary_img.rows) ? end_point[j] + length : binary_img.rows;
	}

	if (end_point[row_num - 1] - start_point[row_num - 1] + 1 < length / 4)
	{
		return -1;
	}

	for (int j = 0; j < row_num; j++)
	{
		if (end_point[j] > image.rows || end_point[j] <=  start_point[j])
		{
			return -4;//溢出保护
		}
	}
	
	return 0;
}

//版面分析，包括字符串的倾斜校正以及确定字符串上下左右边界
int layout_analysis_with_skewcorrect(cv::Mat image, int dilate_x, int dilate_y, int &top, int &bottom, int &left, int &right)
{

	//对倾斜的字符串进行校正，并确定字符上下边界
	int start_y = 0;
	int end_y = image.rows - 1;
	if (skew_correction(image, start_y, end_y) != 0)
	{
		return -1;//未能找到字符串上边界
	}
	start_y = start_y + int(dilate_y / 2); //去除图像膨胀对分割造成的影响
	end_y = end_y - int(dilate_y / 2);

	//计算列和
	cv::Mat cut_img = image(cv::Rect(0, start_y, image.cols, end_y - start_y + 1)).clone();
	
	std::vector<int> colsum(image.cols);
	count_colsum(cut_img, colsum);

	//确定字符串左右边界
	int start_x, end_x;
	for (int j = 0; j < image.cols; j++)
	{
		//未能找到字符左边界
		if (j == image.cols - 1)
		{
			std::cout << "未能找到字符串左边界" << std::endl;
			return -2;
		}
		if (colsum[j] > 0 && colsum[j + 1] > (image.rows / 8))//
		{
			start_x = (j - 1 > 0) ? j - 1 : 0;
			break;
		}
	}
	for (int j = image.cols - 1; j >= 0; j--)
	{
		//未能找到字符串右边界
		if (j == start_x + 1)
		{
			std::cout << "未能找到字符串右边界" << std::endl;
			return -3;
		}
		if (colsum[j] > 0 && colsum[j - 1] > (image.rows / 8))
		{
			end_x = (j + 1 < image.cols - 1) ? j + 1 : image.cols - 1;
			break;
		}
	}

	start_x = start_x + int(dilate_x / 2);
	end_x = end_x - int(dilate_x / 2);

	if (end_x - start_x <= 5)
	{
		return -3;
	}

	if (end_y - start_y <= 5)
	{
		return -3;
	}
	top = start_y;
	bottom = end_y;
	left = start_x;
	right = end_x;
	return 0;
}

//版面分析，确定字符串上下左右边界
int layout_analysis(cv::Mat image, int dilate_x, int dilate_y, int &top, int &bottom, int &left, int &right)
{

	//计算行和
	std::vector<int> rowsum(image.rows);
	count_rowsum(image, rowsum);

	int start_y = 0;
	int end_y = image.rows - 1;

	//确定字符上下边界
	int i = 0;
	while (1)
	{
		//首先查找字符上边界，i为起始查找点
		for (int j = i; j < image.rows; j++)
		{
			if (j == image.rows - 1)
			{
				std::cout << "未能找到字符上边界" << std::endl;
				return -1;
			}
			//查找到上边界，跳出循环,上边界为从上到下第一个非零点 i <= start_y < image.rows - 1
			if (rowsum[j] > 0 && rowsum[j + 1] > (image.cols / 5))
			{
				start_y = j;
				break;
			}
		}
		//查找下边界
		for (int j = start_y + 1; j< image.rows; j++)
		{
			//查找第一个小于阈值的点
			if (j == image.rows - 1 || (rowsum[j] < 1 && rowsum[j + 1] < (image.cols / 5)))
			{
				end_y = j;
				break;
			}
		}

		//检查字符宽度
		if (end_y - start_y < image.rows / 2.5 || end_y - start_y < 7)
		{
			i = end_y;//若字符宽度不满足要求
		}
		else
		{
			break;
		}
	}

	start_y = start_y + int(dilate_y / 2);
	end_y = end_y - int(dilate_y / 2);

	//计算列和
	cv::Mat cut_img = image(cv::Rect(0, start_y, image.cols, end_y - start_y + 1));
	std::vector<int> colsum(cut_img.cols);
	count_colsum(cut_img, colsum);

	//确定字符串左右边界
	int start_x, end_x;
	for (int j = 0; j < image.cols; j++)
	{
		//未能找到字符左边界
		if (j == image.cols - 1)
		{
			std::cout << "未能找到字符串左边界" << std::endl;
			return -2;
		}
		if (colsum[j] > 0 && colsum[j + 1] > (image.rows / 10))//
		{
			start_x = (j - 1 > 0) ? j - 1 : 0;
			break;
		}
	}
	for (int j = image.cols - 1; j >= 0; j--)
	{
		//未能找到字符串右边界
		if (j == start_x + 1)
		{
			std::cout << "未能找到字符串右边界" << std::endl;
			return -3;
		}
		if (colsum[j] > 0 && colsum[j - 1] > (image.rows / 10))
		{
			end_x = (j + 1 < image.cols - 1) ? j + 1 : image.cols - 1;
			break;
		}
	}

	start_x = start_x + int(dilate_x / 2);
	end_x = end_x - int(dilate_x / 2);

	if (end_x - start_x <= 7)
	{
		return -3;
	}
	if (end_x - start_x <= 5)
	{
		return -3;
	}

	top = start_y;
	bottom = end_y;
	left = start_x;
	right = end_x;
	return 0;
}

//连通域分析分割法数字版
int connect_analysis_number(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point)
{
	cv::Mat contour_image = image.clone();
	std::vector<std::vector<cv::Point>> contours(100);//轮廓过多时可能出现bug，需要后续改写
	findContours(contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0)); //CV_RETR_EXTERNAL
	std::vector<std::vector<cv::Point> >::iterator iters;

	std::vector<cv::Rect> bboxSet;
	for (iters = contours.begin(); iters != contours.end(); iters++)
	{
		cv::Rect bbox = boundingRect(cv::Mat(*iters));
		bboxSet.push_back(bbox);
	}

	
	for (std::vector<cv::Rect> ::iterator it = bboxSet.begin(); it != bboxSet.end(); )
	{
		if ( it->area() <= 8 || (it->width <= 3 || it->height <= image.rows/4))
			it = bboxSet.erase(it);    //删除元素，返回值指向已删除元素的下一个位置    
		else
			++it;    //指向下一个位置
	}
	
	length = 21;
	int count = bboxSet.size();

	std::vector<int> ind(count);
	for (int i = 0; i < count; i++)
	{
		ind[i] = 0;
	}

	int flag = 0;

	while (1)
	{
		for (int i = 0; i < count; i++)
		{
			ind[i] = 0;
		}
		flag = 0;
		//合并包含的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if (bboxSet[j].contains(bboxSet[i].tl())
					&& bboxSet[j].contains(bboxSet[i].br()))
				{
					ind[i] = 1;//i包含于j,删去i
					flag = 1;
				}
			}
		}
		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}
			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			count--;
			continue;
		}

		cv::Rect rect;
		//合并交叉的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if (bboxSet[j].contains(bboxSet[i].tl())
					|| bboxSet[j].contains(bboxSet[i].br())
					|| bboxSet[j].contains(cv::Point(bboxSet[i].x + bboxSet[i].width, bboxSet[i].y))
					|| bboxSet[j].contains(cv::Point(bboxSet[i].x, bboxSet[i].y + bboxSet[i].height))
					)
				{
					if ((bboxSet[i].x + bboxSet[i].width - bboxSet[j].x > 2 && bboxSet[i].x < bboxSet[j].x)
						|| (bboxSet[j].x + bboxSet[j].width - bboxSet[i].x > 2 && bboxSet[j].x < bboxSet[i].x))
					{
						if (i != j)
						{
							ind[i] = 1;
							ind[j] = 1;
							rect = (bboxSet[j] | bboxSet[i]);
							flag = 1;
						}
					}

				}
			}
		}

		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}

			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			bboxSet.push_back(rect);
			count--;
			continue;
		}

		//合并上下关系的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if ((bboxSet[i].y + bboxSet[i].height < bboxSet[j].y || bboxSet[j].y + bboxSet[j].height < bboxSet[i].y)
					&& ((bboxSet[i].x < bboxSet[j].x + bboxSet[j].width && bboxSet[i].x > bboxSet[j].x)
						|| (bboxSet[i].x + bboxSet[i].width < bboxSet[j].x + bboxSet[j].width && bboxSet[i].x + bboxSet[i].width > bboxSet[j].x))
					)
				{
					if ((bboxSet[j] | bboxSet[i]).width < 1.5*length)
					{
						if (i != j)
						{
							ind[i] = 1;
							ind[j] = 1;

							int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
							int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

							int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
							int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
							rect = cv::Rect(x, y, xr - x, yr - y);
							flag = 1;
						}
					}

				}
			}
		}

		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}

			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			bboxSet.push_back(rect);
			count--;
			continue;
		}

		std::sort(bboxSet.begin(), bboxSet.end(), [](const cv::Rect& r1, const cv::Rect& r2) { return r1.x < r2.x; });
		
		for (int i = 0; i < bboxSet.size(); i++)
		{
			if (flag == 1)
			{
				break;
			}
			if (bboxSet[i].width < 0.5*length)
			{
				if ((i == 0 && bboxSet[i].width < 0.35*length) || (i != 0 && i != bboxSet.size() - 1 && bboxSet[i].x + bboxSet[i].width - bboxSet[i - 1].x >= bboxSet[i + 1].x + bboxSet[i + 1].width - bboxSet[i].x))
				{
					ind[i] = 1;
					int j = i + 1;
					ind[j] = 1;

					int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
					int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

					int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
					int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
					//rect = cv::Rect(x, y, xr - x, yr - y);
					rect = (bboxSet[j] | bboxSet[i]);

					flag = 1;

				}
				else if ((i == bboxSet.size() - 1) || (i != 0 && i != bboxSet.size() - 1 && bboxSet[i].x + bboxSet[i].width - bboxSet[i - 1].x < bboxSet[i + 1].x + bboxSet[i + 1].width - bboxSet[i].x))
				{
					ind[i] = 1;
					int j = i - 1;
					ind[j] = 1;

					int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
					int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

					int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
					int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
					//rect = cv::Rect(x, y, xr - x, yr - y);
					rect = (bboxSet[j] | bboxSet[i]);
					flag = 1;

				}
			}
		}
		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}

			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			bboxSet.push_back(rect);
			count--;
			continue;
		}
		
		break;
	}

	/*
	cv::Mat draw_image = 255 - image;
	for (int i = 0; i < bboxSet.size(); i++)
	{
		cv::rectangle(draw_image, bboxSet[i], cv::Scalar(0, 0, 255), 1, 1, 0);
	}
	imwrite("char\\rect.jpg", draw_image.clone());
	*/
	if (bboxSet.size() != char_num) 
	{
		//std::cout << bboxSet.size() << std::endl;
		//std::cout << "连通域分割失败" << std::endl;
		return -1;
	}

	for (int i = 0; i < bboxSet.size(); i++)
	{
		if (bboxSet[i].width > 1.4*length)
		{
			//std::cout << "连通域分割失败1" << std::endl;
			return -1;
		}
	}

	for (int j = 0; j < char_num; j++)
	{
		start_point[j] = bboxSet[j].x;
		end_point[j] = (bboxSet[j].x + bboxSet[j].width < image.cols) ? bboxSet[j].x + bboxSet[j].width : image.cols;
		//std::cout << "start:"<<start_point[j] ;
		//std::cout << " end:" << end_point[j] << std::endl;
	}
	return 0;
}

//连通域分析法
int connect_analysis(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point)
{
	cv::Mat contour_image = image.clone();
	std::vector<std::vector<cv::Point>> contours(100);//轮廓过多时可能出现bug，需要后续改写
	findContours(contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0)); //CV_RETR_EXTERNAL
	std::vector<std::vector<cv::Point> >::iterator iters;

	std::vector<cv::Rect> bboxSet;
	for (iters = contours.begin(); iters != contours.end(); iters++)
	{
		cv::Rect bbox = boundingRect(cv::Mat(*iters));
		bboxSet.push_back(bbox);
	}

	/*
	for (std::vector<cv::Rect> ::iterator it = bboxSet.begin(); it != bboxSet.end(); )
	{
		if (it->area() <= 8 || (it->width <= 3 && it->height <= 5))
			it = bboxSet.erase(it);    //删除元素，返回值指向已删除元素的下一个位置    
		else
			++it;    //指向下一个位置
	}
	*/

	int count = bboxSet.size();
	std::vector<int> ind(count);
	for (int i = 0; i < count; i++)
	{
		ind[i] = 0;
	}
	int flag = 0;

	while (1)
	{
		for (int i = 0; i < count; i++)
		{
			ind[i] = 0;
		}
		flag = 0;
		//合并包含的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if (bboxSet[j].contains(bboxSet[i].tl())
					&& bboxSet[j].contains(bboxSet[i].br()))
				{
					ind[i] = 1;//i包含于j,删去i
					flag = 1;
				}
			}
		}
		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}
			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			count--;
			continue;
		}

		cv::Rect rect;
		//合并交叉的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if (bboxSet[j].contains(bboxSet[i].tl())
					|| bboxSet[j].contains(bboxSet[i].br())
					|| bboxSet[j].contains(cv::Point(bboxSet[i].x + bboxSet[i].width, bboxSet[i].y))
					|| bboxSet[j].contains(cv::Point(bboxSet[i].x, bboxSet[i].y + bboxSet[i].height))
					)
				{
					if ((bboxSet[i].x + bboxSet[i].width - bboxSet[j].x > 2 && bboxSet[i].x < bboxSet[j].x)
						|| (bboxSet[j].x + bboxSet[j].width - bboxSet[i].x > 2 && bboxSet[j].x < bboxSet[i].x))
					{
						if (i != j)
						{
							ind[i] = 1;
							ind[j] = 1;
							rect = (bboxSet[j] | bboxSet[i]);
							flag = 1;
						}
					}

				}
			}
		}

		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}

			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			bboxSet.push_back(rect);
			count--;
			continue;
		}

		//合并上下关系的连通域
		for (int i = 0; i < count; i++)
		{
			if (flag == 1)
			{
				break;
			}
			for (int j = 0; j < count; j++)
			{
				if (flag == 1)
				{
					break;
				}
				if ((bboxSet[i].y + bboxSet[i].height < bboxSet[j].y || bboxSet[j].y + bboxSet[j].height < bboxSet[i].y)
					&& ((bboxSet[i].x < bboxSet[j].x + bboxSet[j].width && bboxSet[i].x > bboxSet[j].x)
						|| (bboxSet[i].x + bboxSet[i].width < bboxSet[j].x + bboxSet[j].width && bboxSet[i].x + bboxSet[i].width > bboxSet[j].x))
					)
				{
					if ((bboxSet[j] | bboxSet[i]).width < 1.5*length)
					{
						if (i != j)
						{
							ind[i] = 1;
							ind[j] = 1;

							int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
							int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

							int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
							int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
							rect = cv::Rect(x, y, xr - x, yr - y);
							flag = 1;
						}
					}

				}
			}
		}

		if (flag == 1)
		{
			std::vector<cv::Rect> sortedBbox;
			for (int i = 0; i < count; i++)
			{
				if (ind[i] == 0)
				{
					sortedBbox.push_back(bboxSet[i]);
				}
			}

			bboxSet.clear();
			bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
			bboxSet.push_back(rect);
			count--;
			continue;
		}

		std::sort(bboxSet.begin(), bboxSet.end(), [](const cv::Rect& r1, const cv::Rect& r2) { return r1.x < r2.x; });
		
		for (int i = 0; i < bboxSet.size(); i++)
		{
		if (flag == 1)
		{
		break;
		}
		if (bboxSet[i].width < 0.5*length)
		{
		if ((i == 0 && bboxSet[i].width < 0.35*length) || (i != 0 && i != bboxSet.size() - 1 && bboxSet[i].x + bboxSet[i].width - bboxSet[i - 1].x >= bboxSet[i + 1].x + bboxSet[i + 1].width - bboxSet[i].x))
		{
		ind[i] = 1;
		int j = i + 1;
		ind[j] = 1;

		int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
		int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

		int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
		int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
		//rect = cv::Rect(x, y, xr - x, yr - y);
		rect = (bboxSet[j] | bboxSet[i]);

		flag = 1;

		}
		else if ((i == bboxSet.size() - 1) || (i != 0 && i != bboxSet.size() - 1 && bboxSet[i].x + bboxSet[i].width - bboxSet[i - 1].x < bboxSet[i + 1].x + bboxSet[i + 1].width - bboxSet[i].x))
		{
		ind[i] = 1;
		int j = i - 1;
		ind[j] = 1;

		int x = (bboxSet[i].x < bboxSet[j].x) ? bboxSet[i].x : bboxSet[j].x;
		int xr = (bboxSet[i].x + bboxSet[i].width > bboxSet[j].x + bboxSet[j].width) ? bboxSet[i].x + bboxSet[i].width : bboxSet[j].x + bboxSet[j].width;

		int y = (bboxSet[i].y < bboxSet[j].y) ? bboxSet[i].y : bboxSet[j].y;
		int yr = (bboxSet[i].y + bboxSet[i].height> bboxSet[j].y + bboxSet[j].height) ? bboxSet[i].y + bboxSet[i].height : bboxSet[j].y + bboxSet[j].height;
		//rect = cv::Rect(x, y, xr - x, yr - y);
		rect = (bboxSet[j] | bboxSet[i]);
		flag = 1;

		}
		}
		}
		if (flag == 1)
		{
		std::vector<cv::Rect> sortedBbox;
		for (int i = 0; i < count; i++)
		{
		if (ind[i] == 0)
		{
		sortedBbox.push_back(bboxSet[i]);
		}
		}

		bboxSet.clear();
		bboxSet.assign(sortedBbox.begin(), sortedBbox.end());
		bboxSet.push_back(rect);
		count--;
		continue;
		}
		
		break;
	}

	/*
	cv::Mat draw_image = 255 - image;
	for (int i = 0; i < bboxSet.size(); i++)
	{
	cv::rectangle(draw_image, bboxSet[i], cv::Scalar(0, 0, 255), 1, 1, 0);
	}
	imwrite("char\\rect.jpg", draw_image.clone());
	*/
	if (bboxSet.size() != char_num)
	{
		//std::cout << bboxSet.size() << std::endl;
		//std::cout << "连通域分割失败" << std::endl;
		return -1;
	}

	for (int i = 0; i < bboxSet.size(); i++)
	{
		if (bboxSet[i].width > 1.4*length)
		{
			//std::cout << "连通域分割失败1" << std::endl;
			return -1;
		}
	}

	for (int j = 0; j < char_num; j++)
	{
		start_point[j] = bboxSet[j].x;
		end_point[j] = (bboxSet[j].x + bboxSet[j].width < image.cols) ? bboxSet[j].x + bboxSet[j].width : image.cols;
		//std::cout << "start:"<<start_point[j] ;
		//std::cout << " end:" << end_point[j] << std::endl;
	}
	return 0;
}

//投影法
int project_method(cv::Mat image, int dilate_x, int dilate_y, int length, int char_num, std::vector<int> &start_point, std::vector<int> &end_point)
{
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, 1));
	dilate(image, image, element);

	std::vector<int> rowsum(image.rows);
	std::vector<int> colsum(image.cols);
	count_colsum(image, colsum);

	guass_filter(colsum, image.cols);

	int start, end;
	start = 0;
	end = length;

	for (int i = 0; i < char_num; ++i)
	{
		end_point[i] = 0;
	}

	for (int j = 0; j < char_num; j++)
	{
		//查找字符左边界,从起始点开始找第一非零值
		for (int i = start; i < end; i++)
		{
			if (i == end - 1)
			{
				std::cout << "字符数目缺失" << std::endl;
				return -1;//找不到字符左边界，输入字符个数可能错误
			}

			if (colsum[i] > 0)
			{
				start_point[j] = i;
				break;
			}
		}
		//std::cout << "字符" <<  j + 1 << "左边界:" << start_point[j] <<  std::endl;

		//查找字符右边界,从左边界开始找第一为零值
		start = (start_point[j] + 1 < image.cols - 2) ? start_point[j] + 1 : image.cols - 2;
		end = (int(start_point[j] + 1.3*length) < image.cols - 1) ? int(start_point[j] + 1.3*length) : image.cols - 1;

		for (int i = start; i < end; i++)
		{
			if (colsum[i] == 0 && colsum[i + 1] <= 2 && (i - start) > 0.4*length) //连续两个值为0视为右边界
			{
				end_point[j] = i;
				break;
			}
		}

		//若未找到右边界，则选择区间内最小值作为右边界
		if (end_point[j] == 0)
		{
			if (j == char_num - 1)
			{
				end_point[j] = image.cols - 1;
			}
			else
			{
				start = (int(start_point[j] + 0.8*length) < image.cols - 1) ? int(start_point[j] + 0.8*length) : image.cols - 1;
				end = (int(start_point[j] + 1.2*length) < image.cols) ? int(start_point[j] + 1.2*length) : image.cols;

				int min = colsum[start];
				end_point[j] = start;

				for (int i = start; i < end; i++)
				{
					if (colsum[i] < min)
					{
						min = colsum[i];
						end_point[j] = i;
					}
				}
			}
		}
		//std::cout << "字符" << j + 1 << "右边界:" << end_point[j] << std::endl;

		start = (end_point[j] + 1 < image.cols - 1) ? end_point[j] + 1 : image.cols - 1;
		end = (end_point[j] + length < image.cols) ? end_point[j] + length : image.cols;
	}

	if (end_point[char_num - 1] - start_point[char_num - 1] + 1 < length/4)
	{
		//std::cout << "缺少最后一个字符" << std::endl;
		return -1;
	}
	return 0;
}

//分割非点阵的实体数字
int segmentation_number_vector(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, float thresh)
{
	cv::medianBlur(image, image, 3);

	//二值化
	cv::Mat binary_img;
	cv::threshold(image, binary_img, 0, 255, CV_THRESH_OTSU);
	binary_img = 255 - binary_img;

	//版面分析，包括字符串的倾斜校正以及确定字符串上下左右边界
	int top = 0; int bottom = binary_img.rows - 1; int left = 0; int right = binary_img.cols - 1;
	cv::Mat cut_img;
	if (layout_analysis_with_skewcorrect(binary_img, 0, 0, top, bottom, left, right) == 0)//对于实体数字，不需要进行膨胀操作，膨胀系数为0
	{
		cut_img = binary_img(cv::Rect(left, top, right - left + 1, bottom - top + 1)).clone();
	}
	else
	{
		return -3;//输入图像通道不正确
	}

	cv::Size Kernel(1, 1);
	float length = ((right - left) / char_num) < cut_img.rows * 0.7 ? ((right - left) / char_num) : cut_img.rows * 0.7;

	if(segment_solid_number(cut_img, Kernel, length, char_num, char_image) == 0)
	{
		for (int j = 0; j < char_num; j++)
		{
			if ((char_image[j].rows) / (char_image[j].cols) >= 2)
			{
				cv::resize(char_image[j], char_image[j], cv::Size(int(char_image[j].cols*1.2), char_image[j].rows));
			}
			resize_image_keepratio(char_image[j], 32, 32);
			fill_background(char_image[j]);
		}
		return 0;
	}

	return -1;
}

//分割非点阵的实体数字
int segmentation_number(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x, int dilate_y, float thresh)
{
	//检查输入的旋转角度
	switch (angle)
	{
	case 0:
		break;
	case 90:
		rotate90(image);
		break;
	case 180:
		rotate180(image);
		break;
	case 270:
		rotate270(image);
		break;
	default:
		return -1;//输入角度不正确
		break;
	}

	//输入图像必须为单通道图像，且两幅图像大小必须相等
	if (image.channels() != 1)
	{
		return -2;//输入图像通道不正确
	}

	cv::medianBlur(image, image, 3);

	cv::Mat source_img;
	cv::threshold(image, source_img, 0, 255, CV_THRESH_OTSU);
	source_img = 255 - source_img;

	//二值化
	cv::Mat binary_img;
	cv::threshold(image, binary_img, 0, 255, CV_THRESH_OTSU);
	binary_img = 255 - binary_img;

	//版面分析，包括字符串的倾斜校正以及确定字符串上下左右边界
	int top = 0; int bottom = binary_img.rows - 1; int left = 0; int right = binary_img.cols - 1;
	cv::Mat cut_img;	
	if (layout_analysis_with_skewcorrect(binary_img, 0, 0, top, bottom, left, right) == 0)//对于实体数字，不需要进行膨胀操作，膨胀系数为0
	{
		cut_img = binary_img(cv::Rect(left, top, right - left + 1, bottom - top + 1)).clone();
		imwrite("cutimg.jpg", cut_img);
	}
	else
	{
		return -3;//输入图像通道不正确
	}	

	//预测字符宽度
	float length = ((right - left) / char_num) < cut_img.rows * 0.7 ? ((right - left) / char_num) : cut_img.rows * 0.7;

	std::vector<int> start_point(char_num);
	std::vector<int> end_point(char_num);

	//用连通域分析法分割字符
	if (connect_analysis_number(cut_img, dilate_x, dilate_y, length, char_num, start_point, end_point) == 0)
	{
		for (int j = 0; j < char_num; j++)
		{
			if (end_point[j] > cut_img.cols)
			{
				return -4;//溢出保护
			}
			char_image[j] = cut_img(cv::Rect(start_point[j], 0, end_point[j] - start_point[j], cut_img.rows));

			if (char_image[j].rows > 30)
			{
				cv::medianBlur(char_image[j], char_image[j], 3);
			}

			std::vector<int> sum(char_image[j].rows);
			count_rowsum(char_image[j], sum);

			//首先查找字符区域上边界，i为起始查找点
			for (int m = 0; m < char_image[j].rows; ++m)
			{
				if (m == char_image[j].rows - 1)
				{
					//std::cout << "未能找到字符上边界" << std::endl;
					return -1;
				}
				//查找到上边界，跳出循环,上边界为从上到下第一个非零点 i <= start_y < image.rows - 1
				if ((sum[m] > 0 ))
				{
					top = m;
					break;
				}
			}

			//查找下边界
			for (int m = char_image[j].rows - 1; m > 0; --m)
			{
				//查找第一个小于阈值的点
				if ((sum[m] > 0))
				{
					bottom = m;
					break;
				}
			}

			char_image[j] = cut_img(cv::Rect(start_point[j], top, end_point[j] - start_point[j], bottom - top + 1));

			if ((bottom - top + 1) / (end_point[j] - start_point[j] + 1) >= 2)
			{
				cv::resize(char_image[j], char_image[j], cv::Size(int(char_image[j].cols*1.2), char_image[j].rows));
			}
			resize_image_keepratio(char_image[j], 32, 32);
			fill_background(char_image[j]);

		}
		return 0;
	}

	return -1;
}

//分割汉字，该算法分割汉字还是比较稳定的
int segmentation_chinese(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x, int dilate_y, float thresh)
{
	//检查输入的旋转角度
	switch (angle)
	{
	case 0:
		break;
	case 90:
		rotate90(image);
		break;
	case 180:
		rotate180(image);
		break;
	case 270:
		rotate270(image);
		break;
	default:
		return -1;//输入角度不正确
		break;
	}

	//输入图像必须为单通道图像，且两幅图像大小必须相等
	if (image.channels() != 1)
	{
		return -2;//输入图像通道不正确
	}

	//闭运算去除噪声,太小的字符不进行操作
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	//二值化
	cv::Mat binary_img;
	cv::threshold(image, binary_img, 0, 255, CV_THRESH_OTSU);
	binary_img = 255 - binary_img;

	//图像膨胀主要针对点阵字符发生的断裂
	cv::Mat dilate_img;
	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
	dilate(binary_img, dilate_img, element);

	//对膨胀后的图像进行版面分析
	int top; int bottom; int left; int right;
	cv::Mat cut_img;
	if (layout_analysis_with_skewcorrect(dilate_img, dilate_x, dilate_y, top, bottom, left, right) == 0)
	{
		cut_img = dilate_img(cv::Rect(left, top, right - left + 1, bottom - top + 1)).clone();
		//imwrite("cutimg.jpg", cut_img);
	}
	else
	{
		return -1;
	}

	//对膨胀后的图像进行投影法分割
	float length = (right - left) / char_num;
	std::vector<int> start_point(char_num);
	std::vector<int> end_point(char_num);

	//对二值图作投影法
	if (project_method(cut_img, dilate_x, dilate_y, length, char_num, start_point, end_point) == 0)
	{
		for (int j = 0; j < char_num; j++)
		{
			char_image[j] = cut_img(cv::Rect(start_point[j], 0, end_point[j] - start_point[j], cut_img.rows));//

			if ((bottom - top + 1) / (end_point[j] - start_point[j] + 1) >= 2)
			{
				cv::resize(char_image[j], char_image[j], cv::Size(int(char_image[j].cols*1.2), char_image[j].rows));
			}
			cv::resize(char_image[j], char_image[j], cv::Size(32, 32));
			//resize_image_keepratio(char_image[j], 32, 32);
			//fill_background(char_image[j]);

			element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
			dilate(char_image[j], char_image[j], element);

		}
		return 0;
	}
	else
	{
		return -1;
	}

}

//分割点阵数字
int segmentation_with_dilate(cv::Mat image, std::vector<cv::Mat> &char_image, int char_num, int angle, int dilate_x, int dilate_y, float thresh)
{
	//检查输入的旋转角度
	switch (angle)
	{
	case 0:
		break;
	case 90:
		rotate90(image);
		break;
	case 180:
		rotate180(image);
		break;
	case 270:
		rotate270(image);
		break;
	default:
		return -1;//输入角度不正确
		break;
	}

	//输入图像必须为单通道图像，且两幅图像大小必须相等
	if (image.channels() != 1)
	{
		return -2;//输入图像通道不正确
	}

	cv::Mat source_img;
	cv::threshold(image, source_img, 0, 255, CV_THRESH_OTSU);
	source_img = 255 - source_img;

	//闭运算去除噪声,太小的字符不进行操作
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	if (image.rows > 30)
	{
		morphologyEx(image, image, cv::MORPH_CLOSE, element);
	}

	//二值化
	cv::Mat binary_img;
	cv::threshold(image, binary_img, 0, 255, CV_THRESH_OTSU);
	binary_img = 255 - binary_img;

	//图像膨胀主要针对点阵字符发生的断裂
	cv::Mat dilate_img;
	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
	dilate(binary_img, dilate_img, element);

	//对膨胀后的图像进行版面分析
	int top; int bottom; int left; int right;
	cv::Mat cut_img;
	if (layout_analysis(dilate_img, dilate_x, dilate_y, top, bottom, left, right) == 0)
	{
		cut_img = dilate_img(cv::Rect(left, top, right - left + 1, bottom - top + 1)).clone();
	}
	else
	{
		return -1;
	}

	//对膨胀后的图像进行连通域分析分割法
	float length = (right - left) / char_num;

	std::vector<int> start_point(char_num);
	std::vector<int> end_point(char_num);

	if (connect_analysis(cut_img, dilate_x, dilate_y, length, char_num, start_point, end_point) == 0)
	{
		for (int j = 0; j < char_num; j++)
		{

			char_image[j] = source_img(cv::Rect(left + start_point[j], top, end_point[j] - start_point[j], bottom - top + 1));

			if (char_image[j].rows > 30)
			{
				cv::medianBlur(char_image[j], char_image[j], 3);
			}

			if ((bottom - top + 1)/(end_point[j] - start_point[j] + 1) >= 2 )
			{
				cv::resize(char_image[j], char_image[j], cv::Size(int(char_image[j].cols*1.2), char_image[j].rows));
			}
			resize_image_keepratio(char_image[j], 32, 32);
			fill_background(char_image[j]);

			element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
			dilate(char_image[j], char_image[j], element);
		}

		return 0;
	}
	else
	{
		cut_img = binary_img(cv::Rect(left, top, right - left + 1, bottom - top + 1)).clone();
		//对二值图作投影法
		if (project_method(cut_img, dilate_x, dilate_y, length, char_num, start_point, end_point) == 0)
		{
			for (int j = 0; j < char_num; j++)
			{
				char_image[j] = source_img(cv::Rect(left + start_point[j], top, end_point[j] - start_point[j], bottom - top + 1));//

				if (char_image[j].rows > 30)
				{
					cv::medianBlur(char_image[j], char_image[j], 3);
				}

				if ((bottom - top + 1) / (end_point[j] - start_point[j] + 1) >= 2)
				{
					cv::resize(char_image[j], char_image[j], cv::Size(int(char_image[j].cols*1.2), char_image[j].rows));
				}
				resize_image_keepratio(char_image[j], 32, 32);
				fill_background(char_image[j]);

				element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate_x, dilate_y));
				dilate(char_image[j], char_image[j], element);
			}
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

//旋转函数
void rotate90(cv::Mat& src)
{
	// 矩阵转置
	transpose(src, src);
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 1);
}

void rotate180(cv::Mat& src)
{
	// 矩阵转置
	flip(src, src, 0);
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 1);
}

void rotate270(cv::Mat& src)
{
	// 矩阵转置
	transpose(src, src);
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 0);
}

int rotate_correction(cv::Mat& src, int angle)
{
	//检查输入的旋转角度
	switch (angle)
	{
	case 0:
		break;
	case 90:
		rotate90(src);
		break;
	case 180:
		rotate180(src);
		break;
	case 270:
		rotate270(src);
		break;
	default:
		return -1;//输入角度不正确
		break;
	}
}

//旋转任意角度
void rotate(cv::Mat src, cv::Mat& dst, double angle)
{
	cv::Size src_sz = src.size();
	cv::Size dst_sz(src_sz.width, src_sz.height);

	cv::Point center = cv::Point(src.cols / 2, src.rows / 2);//旋转中心
	cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);//获得仿射变换矩阵

	cv::warpAffine(src, dst, rot_mat, dst_sz);
}

//基于投影法的倾斜校正，同时获得图像的上下边界,查找上下边界的阈值值得推敲
int skew_correction(cv::Mat &src, int &top, int &bottom)
{
	cv::Mat dst;
	int rot = 0;
	int height = src.rows + 100;

	int max = 0;
	int sum = 0;
	for (int i = -4; i < 5; ++i)
	{
		rotate(src, dst, double(i));
		sum = count_dif_rowsum(dst);
		if (sum >= max)
		{
			rot = i;
			max = sum;
		}
	}

	rotate(src, dst, double(rot));
	src = dst.clone();

	std::vector<int> rowsum(dst.rows);
	count_rowsum(dst, rowsum);

	int start_y = 0;
	int end_y = dst.rows - 1;

	//首先查找字符区域上边界，i为起始查找点
	for (int j = 0; j < dst.rows; ++j)
	{
		if (j == dst.rows - 1)
		{
			//std::cout << "未能找到字符上边界" << std::endl;
			return -1;
		}
			//查找到上边界，跳出循环,上边界为从上到下第一个非零点 i <= start_y < image.rows - 1
		if ((rowsum[j] > 0 && rowsum[j + 1] > (dst.cols / 18)))
		{
			start_y = j;
			break;
		}
	}

	//查找下边界
	for (int j = dst.rows - 1; j > 0; --j)
	{
		//查找第一个小于阈值的点
		//if ((rowsum[j] > 0 && rowsum[j - 1] > (dst.cols / 18)))
		if ((rowsum[j] > 0 ))
		{
			end_y = j;
			break;
		}
	}

	if (end_y - start_y <= height)
	{
		height = end_y - start_y;
		top = start_y;
		bottom = end_y;
		//std::cout << top << " " << std::endl;
	}

	return 0;
}

//等比例缩放图像
void resize_image_keepratio(cv::Mat& src, int max_width, int max_height)
{
	float ratio_w = float(max_width) / float(src.cols);
	float ratio_h = float(max_height) / float(src.rows);

	float ratio = (ratio_w < ratio_h) ? ratio_w : ratio_h;


	int new_width = (int(src.cols * ratio) < max_width) ? int(src.cols * ratio) : max_width;
	int new_height = (int(src.rows * ratio) < max_height) ? int(src.rows * ratio) : max_height;

	new_width = (new_width < 5) ? 5 : new_width;
	new_height = (new_width < 5) ? 5 : new_height;

	cv::resize(src, src, cv::Size(new_width, new_height));
}

//给图像填充背景将图像变成正方形
void fill_background(cv::Mat& src)
{
	int size = (src.rows > src.cols) ? src.rows+1 : src.cols+1;

	size = 32;

	cv::Mat char_image = cv::Mat::zeros(size, size, CV_8UC1);

	cv::Mat imageROI = char_image(cv::Rect((size - src.cols) / 2, (size - src.rows) / 2, src.cols, src.rows));

	src.copyTo(imageROI);

	src = char_image;
}



