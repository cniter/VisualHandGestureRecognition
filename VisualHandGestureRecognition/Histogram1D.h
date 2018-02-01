#pragma once

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

class Histogram1D
{
public:
	Histogram1D(void);
	~Histogram1D(void);

	cv::MatND getHistogram(const cv::Mat &image);	// ����һάֱ��ͼ
	cv::Mat getHistogramImage(const cv::Mat &image);	// ����1Dֱ��ͼ��������һ��ֱ��ͼͼ��

	cv::Mat applyLookUp(const cv::Mat &image, const cv::Mat &lookup);	// ��ͼ��Ӧ�ò��ұ�������ͼ��,���ͼ��Աȶ�
	cv::Mat stretch(const cv::Mat &image, int minValue = 0);	// ͼ������

	cv::Mat equalize(const cv::Mat &image);	// ֱ��ͼ���⻯

private:
	int histSize[1];	// �������
	float hranges[2];	// ���ص���С�����ֵ
	const float *ranges[1];
	int channels[1];	// ���õ�һ��ͨ��
};

