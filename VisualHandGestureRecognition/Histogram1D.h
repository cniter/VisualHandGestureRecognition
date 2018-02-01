#pragma once

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

class Histogram1D
{
public:
	Histogram1D(void);
	~Histogram1D(void);

	cv::MatND getHistogram(const cv::Mat &image);	// 计算一维直方图
	cv::Mat getHistogramImage(const cv::Mat &image);	// 计算1D直方图，并返回一幅直方图图像

	cv::Mat applyLookUp(const cv::Mat &image, const cv::Mat &lookup);	// 对图像应用查找表生成新图像,提高图像对比度
	cv::Mat stretch(const cv::Mat &image, int minValue = 0);	// 图像拉伸

	cv::Mat equalize(const cv::Mat &image);	// 直方图均衡化

private:
	int histSize[1];	// 项的数量
	float hranges[2];	// 像素的最小及最大值
	const float *ranges[1];
	int channels[1];	// 仅用到一个通道
};

