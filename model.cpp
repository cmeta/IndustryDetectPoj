/********************************************************
Copyright	BIAC
FileName:	model.cpp
Author:        Version :          Date:
Description:    // 识别模型的参数初始化和接口函数
Version:         // 版本信息
History:         // 历史修改记录
<author>  <time>   <version >      <desc>
David    18/7/16      1.0       build this moudle
********************************************************/

#include"model.h"
#include<iostream>
#include<time.h>

#include "tf_utils.hpp"
#include <array>
#include <fstream>

#include "tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace tiny_dnn::layers;

static void deallocate_tensor(void*, size_t, void*) {}

template <typename Activation>
double rescale(double x) {
	Activation a;
	return 100.0 * (x - a.scale().first) / (a.scale().second - a.scale().first);
}

RecognModel*RecognModel::recognmodel = NULL;

RecognModel * RecognModel::getInstance()
{
	if (recognmodel == NULL) {
		recognmodel = new RecognModel();
	}
	return recognmodel;
}

/*
Function:       init_model()
Description:	总初始化，包括tiny-cnn，zbar
Return:         初始化是否成功
Others:
*/
int RecognModel::init_model()
{
	init_zbar();

	init_tinycnn();
	/*
	if (init_tensorflow() != 0)
		return -1;
	*/
	if (load_list() != 0)
		return -1;
	
	return 0;
}

/*
Function:       init_tinycnn()
Description:	初始化init_tinycnn
*/
void RecognModel::init_tinycnn()
{
	//network<sequential> nn;
	adagrad optimizer;

	// connection table [Y.Lecun, 1998 Table.1]
#define O true
#define X false
	static const bool connection[] = {
		O, X, X, X, O, O, O, X, X, O, O, O, O, X, O, O,
		O, O, X, X, X, O, O, O, X, X, O, O, O, O, X, O,
		O, O, O, X, X, X, O, O, O, X, X, O, X, O, O, O,
		X, O, O, O, X, X, O, O, O, O, X, X, O, X, O, O,
		X, X, O, O, O, X, X, O, O, O, O, X, O, O, X, O,
		X, X, X, O, O, O, X, X, O, O, O, O, X, O, O, O
	};
#undef O
#undef X

	num_nn << convolutional_layer<tan_h>(
		32, 32, 5, 1, 6)  /* 32x32 in, 5x5 kernel, 1-6 fmaps conv */
		<< average_pooling_layer<tan_h>(
			28, 28, 6, 2)     /* 28x28 in, 6 fmaps, 2x2 subsampling */
		<< convolutional_layer<tan_h>(
			14, 14, 5, 6, 16, connection_table(connection, 6, 16))
		<< average_pooling_layer<tan_h>(10, 10, 16, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 16, 128)
		<< fully_connected_layer<tan_h>(128, 11);

	std::ifstream ifs_num("numplussign-weights");
	ifs_num >> num_nn;

	numplus_nn << convolutional_layer<tan_h>(
		32, 32, 5, 1, 6)  /* 32x32 in, 5x5 kernel, 1-6 fmaps conv */
		<< average_pooling_layer<tan_h>(
			28, 28, 6, 2)     /* 28x28 in, 6 fmaps, 2x2 subsampling */
		<< convolutional_layer<tan_h>(
			14, 14, 5, 6, 16, connection_table(connection, 6, 16))
		<< average_pooling_layer<tan_h>(10, 10, 16, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 16, 128)
		<< fully_connected_layer<tan_h>(128, 11);

	// load nets  
	//std::ifstream ifs("LeNet-weights");
	std::ifstream ifs("numplus-weights");
	ifs >> numplus_nn;

	chi_nn << convolutional_layer<tan_h>(
		32, 32, 5, 1, 8)  //32x32 in, 5x5 kernel, 1-6 fmaps conv 
		<< average_pooling_layer<tan_h>(
			28, 28, 8, 2)     // 28x28 in, 6 fmaps, 2x2 subsampling
		<< convolutional_layer<tan_h>(
			14, 14, 5, 8, 18)
		<< average_pooling_layer<tan_h>(10, 10, 18, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 18, 192)
		<< fully_connected_layer<tan_h>(192, 25);

	std::ifstream ifs_chi("chi-weights");
	ifs_chi >> chi_nn;

	letter_nn << convolutional_layer<tan_h>(
		32, 32, 5, 1, 8)  //32x32 in, 5x5 kernel, 1-6 fmaps conv 
		<< average_pooling_layer<tan_h>(
			28, 28, 8, 2)     // 28x28 in, 6 fmaps, 2x2 subsampling
		<< convolutional_layer<tan_h>(
			14, 14, 5, 8, 18)
		<< average_pooling_layer<tan_h>(10, 10, 18, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 18, 192)
		<< fully_connected_layer<tan_h>(192, 26);

	std::ifstream ifs_letter("letter-weights");
	ifs_letter >> letter_nn;

	numletter_nn << convolutional_layer<tan_h>(
		32, 32, 5, 1, 8)  //32x32 in, 5x5 kernel, 1-6 fmaps conv 
		<< average_pooling_layer<tan_h>(
			28, 28, 8, 2)     // 28x28 in, 6 fmaps, 2x2 subsampling
		<< convolutional_layer<tan_h>(
			14, 14, 5, 8, 18)
		<< average_pooling_layer<tan_h>(10, 10, 18, 2)
		<< convolutional_layer<tan_h>(5, 5, 5, 18, 224)
		<< fully_connected_layer<tan_h>(224, 35);

	std::ifstream ifs_numletter("numletter-weights");
	ifs_numletter >> numletter_nn;
}

/*
Function:       call_tinycnn(cv::Mat image[], int num, int ocr_type, std::string& result)
Description:	调用tiny-cnn框架进行字符识别
Return:         识别是否成功
Others:			result为识别结果
*/
int RecognModel::call_tinycnn(cv::Mat image[], int num, int ocr_type, std::string& result)
{
	if (ocr_type == TYPE_CHAR)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = letter_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 26; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}

	}

	if (ocr_type == TYPE_NUMBER)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789L";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = numplus_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_NUMBER_NODOT)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789/";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = num_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_CHINESE)
	{
		double minv = -1.0;
		double maxv = 1.0;

		std::string list = "生产日期有效至品批号保质合格年月时间分秒天及赏味限";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = chi_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 25; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list.substr(int(scores[0].second * 2), 2);
		}
	}

	return 0;
}

int RecognModel::call_tinycnn(std::vector<cv::Mat> &image, int num, int ocr_type, std::string& result)
{
	if (ocr_type == TYPE_CHARANDNUMBER)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "ABCDEFGHJKLMNPQRSTUVWXYZ0123456789/";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = numletter_nn.predict(data);

			std::vector<std::pair<double, int> > scores;

			for (int j = 0; j < 35; j++)
			{
				for (int i = 0; i < whitelist_length; i++)
				{
					if(white_list[i]==j)
						scores.emplace_back(rescale<tan_h>(res[j]), j);
				}
			}
			
			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}

	}

	if (ocr_type == TYPE_CHAR)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = letter_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 26; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}

	}

	if (ocr_type == TYPE_NUMBER)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789L";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = numplus_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_NUMBER_NODOT)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789/";
		int index = 0;

		for (int n = 0; n < num; n++)
		{

			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = num_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_CHINESE)
	{
		double minv = -1.0;
		double maxv = 1.0;

		std::string list = "生产日期有效至品批号保质合格年月时间分秒天及赏味限";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = chi_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 25; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list.substr(int(scores[0].second * 2), 2);
		}
	}

	return 0;
}

int RecognModel::call_tinycnn(std::vector<cv::Mat> &image, int num, int ocr_type, std::vector<bool> able,std::string& result)
{
	if (ocr_type == TYPE_CHAR)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			if (able[n] == false)
				continue;
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = letter_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 26; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}

	}

	if (ocr_type == TYPE_NUMBER)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789L";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			if (able[n] == false)
				continue;
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = numplus_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_NUMBER_NODOT)
	{
		double minv = -1.0;
		double maxv = 1.0;
		char* list;
		list = "0123456789/";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			if (able[n] == false)
				continue;
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = num_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 11; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list[scores[0].second];

		}
	}

	if (ocr_type == TYPE_CHINESE)
	{
		double minv = -1.0;
		double maxv = 1.0;

		std::string list = "生产日期有效至品批号保质合格年月时间分秒天及赏味限";
		int index = 0;

		for (int n = 0; n < num; n++)
		{
			if (able[n] == false)
				continue;
			vec_t data;
			cv::Mat_<uint8_t> resized;
			cv::resize(image[n], resized, cv::Size(32, 32));

			// mnist dataset is "white on black", so negate required  
			std::transform(resized.begin(), resized.end(), std::back_inserter(data),
				[=](uint8_t c) { return (255 - c) * (maxv - minv) / 255.0 + minv; });
			auto res = chi_nn.predict(data);

			std::vector<std::pair<double, int> > scores;
			for (int j = 0; j < 25; j++)
				scores.emplace_back(rescale<tan_h>(res[j]), j);

			std::sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());
			scores[0].second;

			result += list.substr(int(scores[0].second * 2), 2);
		}
	}

	return 0;
}

/*
Function:       init_zbar()
Description:	初始化zbar引擎
Return:         无
Others:         
*/
void RecognModel::init_zbar()
{
	scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
}

/*
Function:       call_zbar(cv::Mat image,std::string& decode,std::string& result)
Description:	调用zbar引擎
Return:         二维码解析是否成功
Others:			decode为码类型，result为解码结果
*/
bool RecognModel::call_zbar(cv::Mat image,std::string& decode,std::string& result)
{
	if (image.channels() != 1)
	{
		cv::cvtColor(image, image, CV_RGB2GRAY);
	}
	
	int width = image.cols;
	int height = image.rows;
	uchar *raw = (uchar *)image.data;

	// wrap image data
	zbar::Image img(width, height, "Y800", raw, width * height);

	int n = scanner.scan(img);

	if (img.symbol_begin() == img.symbol_end())
	{
		return false;
	}

	//解析的结果
	for (zbar::Image::SymbolIterator symbol = img.symbol_begin();
		symbol != img.symbol_end();
		++symbol) 
	{
		result = symbol->get_data();
		decode = symbol->get_type_name();
	}

	return true;
}

//以下是关于tensorflow和Tesseract的内容，现已不再使用，但接口代码仍保留
/*
Function:       init_tensorflow_graph()
Description:	the inplement of function init_tensorflow()

Return:         初始化是否成功
Others:			TensorGraph includes number letter and chinese
*/
int RecognModel::init_tensorflow_graph(TensorGraph &tensor_graph, const char* file)
{
	//load the .pb file
	tensor_graph.graph = LoadGraphDef(file);
	if (tensor_graph.graph == nullptr) {
		//std::cout << "Can't init graph" << std::endl;
		return 1;
	}

	//get the input operation 
	tensor_graph.input_op = { TF_GraphOperationByName(tensor_graph.graph, "input"), 0 };
	if (tensor_graph.input_op.oper == nullptr) {
		//std::cout << "Can't init input_op" << std::endl;
		return 2;
	}

	//get the output operation 
	tensor_graph.out_op = { TF_GraphOperationByName(tensor_graph.graph, "out"), 0 };
	if (tensor_graph.out_op.oper == nullptr) {
		//std::cout << "Can't init out_op" << std::endl;
		return 3;
	}

	//new a session
	tensor_graph.status = TF_NewStatus();
	TF_SessionOptions* options = TF_NewSessionOptions();
	tensor_graph.sess = TF_NewSession(tensor_graph.graph, options, tensor_graph.status);
	TF_DeleteSessionOptions(options);
	if (TF_GetCode(tensor_graph.status) != TF_OK) {
		TF_DeleteStatus(tensor_graph.status);
		return 4;
	}

	return 0;
}

/*
Function:       init_tensorflow()
Description:	初始化tensorflow引擎

Return:         初始化是否成功
Others:			TensorGraph includes number letter and chinese
*/
int RecognModel::init_tensorflow()
{
	std::cout << " init num_normal_model graph" << std::endl;
	if (init_tensorflow_graph(graph_num, "num_normal_model.pb") != 0)
		return -1;
	std::cout << " init letter_model graph" << std::endl;
	if (init_tensorflow_graph(graph_letter, "letter_model.pb") != 0)
		return -2;
	std::cout << " init chi_model graph" << std::endl;
	if (init_tensorflow_graph(graph_chi, "chi_model.pb") != 0)
		return -3;

	return 0;
}

/*
Function:       load_list()
Description:	加载字符列表

Return:         加载是否成功
Others:
*/
int RecognModel::load_list()
{
	std::ifstream infile;

	std::string num_list = "ABCDEFGHJKLMNPQRSTUVWXYZ0123456789/";

	//for (int i = 0; i < 3755; i++)
	//{
	//	std::string s = chi_list.substr(int(i * 2), 2);
	//	chi_hash[s] = i;
	//}

	infile.open("white_list.txt");
	if (!infile.is_open())
	{
		std::cout << "txt文件打开失败" << std::endl;
		return -1;
	}

	int j = 0;

	while (!infile.eof())
	{
		std::string s;
		getline(infile, s);
		whitelist_length++;
	}
	white_list = new int[whitelist_length];

	infile.seekg(std::ios::beg);
	while (!infile.eof())
	{
		std::string s;
		getline(infile, s);
		white_list[j] = num_list.find(s);
		//std::cout << white_list[j] << std::endl;
		j++;
	}

	return 0;
}

/*
Function:       call_tensorflow()
Description:	call tensorflow engine

Inputs:			cv::Mat image[]: images
				int num: the number of images
				ocr_type: number,letter or chinese

Return:         result
Others:			
*/
int RecognModel::call_tensorflow(cv::Mat image[], int num, int ocr_type, std::string& result)
{
	//input values
	std::vector<float> input_vals;
	//input dimensions
	std::array<int64_t, 4> input_dims = { num, 32, 32, 1 };

	//get the input values
	for (int n = 0; n < num; n++)
	{
		for (int i = 0; i < image[n].rows; ++i) {
			//获取第 i 行首像素指针
			uchar * p = image[n].ptr<uchar>(i);
			//对第 i 行的每个像素(byte)操作
			for (int j = 0; j < image[n].cols; ++j) {
				float v = p[j] / 255.0;
				input_vals.push_back(v);
			}
		}
	}

	//get the input_tensor
	TF_Tensor* input_tensor = TF_NewTensor(TF_FLOAT,
		input_dims.data(), input_dims.size(),
		input_vals.data(), input_vals.size() * sizeof(float),
		deallocate_tensor, nullptr);
	//get the output_tensor
	TF_Tensor* output_tensor = nullptr;

	//run the session
	int classes;
	switch (ocr_type)
	{
	case TYPE_NUMBER:
		classes = 10;
		TF_SessionRun(graph_num.sess,
			nullptr, // Run options.
			&graph_num.input_op, &input_tensor, 1, // Input tensors, input tensor values, number of inputs.
			&graph_num.out_op, &output_tensor, 1, // Output tensors, output tensor values, number of outputs.
			nullptr, 0, // Target operations, number of targets.
			nullptr, // Run metadata.
			graph_num.status // Output status.
		);
		if (TF_GetCode(graph_num.status) != TF_OK) {
			return -1;
		}

		break;

	case TYPE_CHAR:
		classes = 26;
		TF_SessionRun(graph_letter.sess,
			nullptr, // Run options.
			&graph_letter.input_op, &input_tensor, 1, // Input tensors, input tensor values, number of inputs.
			&graph_letter.out_op, &output_tensor, 1, // Output tensors, output tensor values, number of outputs.
			nullptr, 0, // Target operations, number of targets.
			nullptr, // Run metadata.
			graph_letter.status // Output status.
		);
		break;
	case TYPE_CHINESE:
		classes = 3755;
		TF_SessionRun(graph_chi.sess,
			nullptr, // Run options.
			&graph_chi.input_op, &input_tensor, 1, // Input tensors, input tensor values, number of inputs.
			&graph_chi.out_op, &output_tensor, 1, // Output tensors, output tensor values, number of outputs.
			nullptr, 0, // Target operations, number of targets.
			nullptr, // Run metadata.
			graph_chi.status // Output status.
		);
		break;
	default:
		break;
	}

	auto data = static_cast<float*>(TF_TensorData(output_tensor));

	for (int i = 0; i < whitelist_length; i++)
	{
		//std::cout << white_list[i] << std::endl;
	}

	if (ocr_type == TYPE_CHINESE)
	{
		int index = 0;
		for (int j = 0; j < num; j++)
		{
			index = white_list[0];
			float max = data[j * classes + white_list[0]];
			for (int i = 0; i < whitelist_length; i++)
			{
				if (data[j * classes + white_list[i]] > max)
				{
					index = white_list[i];
					max = data[j * classes + white_list[i]];
				}
			}
			std::cout << index << std::endl;
			result += chi_list.substr(int(index * 2), 2);
		}

	}

	else 
	{
		for (int j = 0; j < num; j++)
		{
			float max = data[j*classes];
			int index = 0;
			for (int i = j*classes; i < (j + 1)*classes; i++)
			{
				if (data[i] > max)
				{
					index = i%classes;
					max = data[i];
				}
			}
			char* list;

			switch (ocr_type)
			{
			case TYPE_NUMBER:
				list = "0123456789";
				result += list[index];
				break;
			case  TYPE_CHAR:
				list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
				result += list[index];
				break;
			case  TYPE_CHINESE:
				result += chi_list.substr(int(index* 2), 2);
				break;
			}

		}
	}

	//std::cout << "output vals: " << index << std::endl;
	//std::cout << result << std::endl;

	TF_DeleteTensor(input_tensor);
	TF_DeleteTensor(output_tensor);



	return 0;
}

/*
std::string UTF8ToGB(const char* str)
{
std::string result;
WCHAR *strSrc;
//TCHAR *szRes;
LPSTR szRes;

//获得临时变量的大小
int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
strSrc = new WCHAR[i + 1];
MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

//获得临时变量的大小
i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
szRes = new CHAR[i + 1];
WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

result = szRes;
delete[]strSrc;
delete[]szRes;

return result;
}
*/

/*
Function:       init_tesseract()
Description:	初始化Tesseract引擎

Return:         初始化是否成功
Others:         Tesseract4.0不支持白名单
*/
/*
bool RecognModel::init_tesseract()
{
	if (tess_eng.Init(NULL, "eng", tesseract::OEM_DEFAULT) != 0)
		return false;
	if (tess_chi.Init(NULL, "chi_sim", tesseract::OEM_DEFAULT) != 0)
		return false;

	tess_eng.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	tess_chi.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	return true;
}
*/
/*
Function:       init_tesseract()
Description:	初始化Tesseract引擎

Return:         初始化是否成功
Others:         Tesseract4.0不支持白名单
*/
/*
std::string RecognModel::ocr_tesseract(cv::Mat image, int ocr_type)
{
	clock_t start, ends;
	start = clock();

	const char *text;
	std::string stext;

	switch (ocr_type)
	{
	case TYPE_UNKNOWN:

	case TYPE_NUMBER:

	case TYPE_CHAR:
		text = tess_eng.TesseractRect((uchar*)image.data, image.channels(), image.step1(), 0, 0, image.cols, image.rows);
		stext = text;
		break;

	case TYPE_CHINESE:
		text = tess_chi.TesseractRect((uchar*)image.data, image.channels(), image.step1(), 0, 0, image.cols, image.rows);
		stext = UTF8ToGB(text);
		break;

	default:
		break;
	}

	std::cout << stext << std::endl;
	ends = clock();
	std::cout << ends - start << std::endl;

	return stext;
}
*/
RecognModel::RecognModel()
{
}

RecognModel::~RecognModel()
{
}