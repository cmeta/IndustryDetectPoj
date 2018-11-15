/*************************************************
Copyright BIAC
File name:   model.h
Author:chenjing     Version:1.0      Date:20180717 
Description:  识别模型的参数初始化和接口函数
Others:
History:
*************************************************/


#ifndef _MODEL_H_
#define _MODEL_H_
#include <zbar.h>
//#include <baseapi.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "c_api.h"
#include <unordered_map> 
#include "tiny_dnn/tiny_dnn.h"

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace tiny_dnn::layers;

//识别种类
typedef enum OCRType
{
	TYPE_UNKNOWN = 0,	//混合识别
	TYPE_NUMBER,		//识别数字
	TYPE_NUMBER_NODOT,
	TYPE_CHINESE,		//识别中文
	TYPE_CHAR,			//识别字母
	TYPE_CHARANDNUMBER,			//识别字母和数字
	TYPE_QRCODE,		//识别二维码
}OCRType;

class TensorGraph
{
public:
	TF_Graph* graph;
	TF_Output input_op;
	TF_Output out_op;
	TF_Session* sess;
	TF_Status* status;
};

class RecognModel
{
private:

	RecognModel();
	~RecognModel();
	RecognModel(const RecognModel&);
	RecognModel&operator=(const RecognModel&);

	static RecognModel* recognmodel;

public:

	static RecognModel *getInstance();

	//bool init_tesseract();
	//std::string ocr_tesseract(cv::Mat image, int ocr_type);   //Tesseract识别引擎

	void init_zbar();
	bool call_zbar(cv::Mat image,std::string& decode,std::string& result);
	
	int init_tensorflow_graph(TensorGraph &tensor_graph, const char* file);
	int init_tensorflow();
	//int call_tensorflow(cv::Mat image, int ocr_type);

	int call_tensorflow(cv::Mat image[], int num, int ocr_type, std::string& result);

	void init_tinycnn();
	int call_tinycnn(cv::Mat image[], int num, int ocr_type, std::string& result);
	int call_tinycnn(std::vector<cv::Mat> &image, int num, int ocr_type, std::string& result);
	int call_tinycnn(std::vector<cv::Mat> &image, int num, int ocr_type, std::vector<bool> able, std::string& result);

	int init_model();
	int load_list();

	class delInstance {
		~delInstance() {
			if (recognmodel != NULL) {
				delete recognmodel;
			}
		}
	};
	static delInstance dl;

private:
	//tesseract::TessBaseAPI tess_eng;
	//tesseract::TessBaseAPI tess_chi;

	zbar::ImageScanner scanner;
	network<sequential> num_nn;
	network<sequential> numplus_nn;
	network<sequential> chi_nn;
	network<sequential> letter_nn;
	network<sequential> numletter_nn;

	TensorGraph graph_num;
	TensorGraph graph_letter;
	TensorGraph graph_chi;

	std::string chi_list;
	std::unordered_map<std::string, int> chi_hash;
	int *white_list = NULL;
	int whitelist_length = 0;
};



#endif

