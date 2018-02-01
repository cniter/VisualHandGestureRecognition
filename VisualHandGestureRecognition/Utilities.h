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

	// ��ȡ�ļ�����ָ����ʽ�����ļ���
	void getAllFormatFiles(string input_path,	// �����ļ���·�� 
		string input_format,	// ����ָ���ļ���ʽ����Ϊ*�����ȡȫ���ļ�
		vector<string> &output_files);	// ��������ļ���������·����

	// �����ļ��м����ļ���
	void makeDir(const string &input_path);

	// ��ֵ�˲�
	int medianBlurOption(const cv::Mat &img, int row, int col, int kernelSize);
	// ��0���ص�Ӧ����ֵ�˲�
	void applyMedianBlurOption(const cv::Mat &img, cv::Mat &out_img, int kernelSize);
};

