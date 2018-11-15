#include "segment_char.h"
/*
Img: ����ͼ��
Kernel: ��̬ѧ�˲���
length: �ַ����
charNum: �ַ���
charImageSet: �ָ����ַ�ͼ��
*/
int segment_solid_number(cv::Mat& Img, cv::Size& Kernel, const int& length, const int& charNum, std::vector<cv::Mat>& charImageSet)
{
	//bbox������ֵ
	int minArea = 9, minWidth = 2, minHeight = Img.rows / 4 + 1;
	cv::Mat contourImage = Img.clone();
	std::vector<std::vector<cv::Point>> contours;
	findContours(contourImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	cv::Rect bbox;
	std::vector<cv::Rect> bboxSet;
	//ɸѡ�ߴ�����bbox
	for (std::vector<std::vector<cv::Point> >::iterator iters = contours.begin(); iters != contours.end(); iters++)
	{
		bbox = boundingRect(cv::Mat(*iters));
		if (bbox.area() < minArea || bbox.width < minWidth || bbox.height < minHeight)
			continue;
		bboxSet.push_back(bbox);
	}

	std::sort(bboxSet.begin(), bboxSet.end(), [](const cv::Rect& r1, const cv::Rect& r2) { return r1.x < r2.x; });

	int size = bboxSet.size(), interval = 4;
	std::vector<int> flag(size, 0);

	/*for (int i = 0, j = 0; i < size; i++)
	{
		if (flag[i] == 0)
		{
			imwrite(cv::format("char1\\char1_%d.jpg", j), Img(bboxSet[i]));
			j++;
		}
	}*/

	for (int i = 0; i < size; i++)
	{
		if (flag[i] == 1)
			continue;
		for (int j = i - 1; j >= 0 && j > i - interval; j--)
		{
			if (!flag[j] && mergeUpDownBbox(bboxSet[i], bboxSet[j], 6, 2))			//�ϲ�������ͨ��
				flag[j] = 1;
			else if (!flag[j] && mergeLeftRightBbox(bboxSet[i], bboxSet[j], 21))	//�ϲ�������ͨ��
				flag[j] = 1;
		}
		for (int j = i + 1; j < size && j < i + interval; j++)
		{
			if (!flag[j] && mergeUpDownBbox(bboxSet[i], bboxSet[j], 6, 2))
				flag[j] = 1;
			else if (!flag[j] && mergeLeftRightBbox(bboxSet[i], bboxSet[j], 21))
				flag[j] = 1;
		}
	}

	/*for (int i = 0, j = 0; i < size; i++)
	{
		if (flag[i] == 0)
		{
			imwrite(cv::format("char1\\char2_%d.jpg", j), Img(bboxSet[i]));
			j++;
		}
	}*/

	//�˳�����Ͱ�����ͨ��
	for (int i = 0, j = 0; i < size; i++)
	{
		if (flag[i] == 1)
			continue;
		cv::Mat charImage = Img(bboxSet[i]).clone();
		//imwrite(cv::format("char1\\char1_%d.jpg", j), charImage);
		if (filterCharImage(charImage) != 0)
		{
			return -1;
		}
		charImageSet.push_back(charImage);
		//imwrite(cv::format("char1\\char2_%d.jpg", j), charImage);
		j++;
		//std::cout << "Size: " << charImage.rows << " " << charImage.cols << std::endl;
	}
	//std::cout << "number of char: " << charImageSet.size() << std::endl;
	if (std::abs((int)charImageSet.size() - charNum) > 0)
	{
		return -1;
	}
	return 0;
}

int filterCharImage(cv::Mat& charImage)
{
	int imageArea = (charImage.rows / 2) * (charImage.cols / 2);
	cv::Mat contourImage = charImage.clone();
	std::vector<std::vector<cv::Point>> contours;
	findContours(contourImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	cv::Rect bbox, maxBbox;
	std::vector<cv::Rect> bboxSet;
	//�˳�����Ͱ�����ͨ��
	int i = 0, index = 0, maxBboxArea = 0;
	for (std::vector<std::vector<cv::Point> >::iterator iters = contours.begin(); iters != contours.end(); iters++)
	{
		bbox = boundingRect(cv::Mat(*iters));
		bboxSet.push_back(bbox);
		if (bbox.area() > maxBboxArea)
		{
			maxBboxArea = bbox.area();
			index = i;
		}
		i++;
	}
	if (bboxSet.size() == 0)
	{
		return -1;
	}
	maxBbox = bboxSet[index];
	for (int i = 0; i < bboxSet.size(); i++)
	{
		//���bbox�����С����ֵ����λ�����maxBbox���棬�򽫸�bbox������0
		if(i != index && bbox.area() < imageArea && ((bboxSet[i] & maxBbox) == bboxSet[i]))
			charImage(bboxSet[i]).setTo(0);
	}
	return 0;
}

bool mergeUpDownBbox(cv::Rect& rect1, cv::Rect& rect2, int threshX = 6, int threshY = 2)
{
	//�ϲ�����������bbox���±߽�������bbox���ϱ߽�֮��ľ���С����ֵthreshY����bbox�����ĺ�����֮��Ĳ�ֵС����ֵthreshX
	int centerX1 = rect1.x + cvRound(rect1.width / 2.0);
	int centerX2 = rect2.x + cvRound(rect2.width / 2.0);
	if (std::min(rect1.br().y, rect2.br().y) - std::max(rect1.y, rect2.y) <= threshY &&
		std::abs(centerX1 - centerX2) <= threshX)
	{
		int x = std::min(rect1.x, rect2.x);
		int y = std::min(rect1.y, rect2.y);
		int width = std::max(rect1.br().x, rect2.br().x) - x;
		int height = std::max(rect1.br().y, rect2.br().y) - y;
		rect1 = cv::Rect(x, y, width, height);
		return true;
	}
	return false;
}

bool mergeLeftRightBbox(cv::Rect& rect1, cv::Rect& rect2, int thresh = 21)
{
	//�������bbox�ϲ���Ŀ��С�ڸ�����ֵ��ϲ�
	if (std::max(rect1.br().x, rect2.br().x) - std::min(rect1.x, rect2.x) < thresh)
	{
		int x = std::min(rect1.x, rect2.x);
		int y = std::min(rect1.y, rect2.y);
		int width = std::max(rect1.br().x, rect2.br().x) - x;
		int height = std::max(rect1.br().y, rect2.br().y) - y;
		rect1 = cv::Rect(x, y, width, height);
		return true;
	}
	return false;
}