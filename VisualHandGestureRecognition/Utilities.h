#pragma once

#include <io.h>  
#include <direct.h>  
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

using namespace std;

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);

	// 获取文件夹下指定格式所有文件名
	void getAllFormatFiles(string input_path,	// 输入文件夹路径 
		string input_format,	// 输入指定文件格式，若为*，则获取全部文件
		vector<string> &output_files);	// 输出完整文件名（包括路径）

	// 创建文件夹及子文件夹
	void makeDir(const string &input_path);

	// 中值滤波
	int medianBlurOption(const cv::Mat &img, int row, int col, int kernelSize);
	// 对0像素点应用中值滤波
	void applyMedianBlurOption(const cv::Mat &img, cv::Mat &out_img, int kernelSize);
};

