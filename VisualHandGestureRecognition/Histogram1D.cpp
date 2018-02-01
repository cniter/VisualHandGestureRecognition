#include "Histogram1D.h"
#include <iostream>

Histogram1D::Histogram1D(void)
{
	// 初始化一维（1D）直方图的参数
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0;	// 默认情况，考察0号通道
}


Histogram1D::~Histogram1D(void)
{
}

cv::MatND Histogram1D::getHistogram(const cv::Mat &image)
{
	cv::MatND hist;

	// 计算直方图
	cv::calcHist(&image,
		1,	// 计算单张图像的直方图
		channels,	// 通道数量
		cv::Mat(),	// 不使用图像作为掩码
		hist,	// 返回的直方图
		1,	// 这是一维（1D）的直方图
		histSize,	// 项的数量
		ranges	// 像素值的范围
		);

	return hist;
}

cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image)
{
	// 首先计算直方图
	cv::MatND hist = getHistogram(image);

	// 获取最大值和最小值
	double maxVal = 0.0;
	double minVal = 0.0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);

	// 显示直方图的图像
	cv::Mat histImage(histSize[0], histSize[0], CV_8U, cv::Scalar(255));
	// 设置最高点为nbins的90%
	int hpt = static_cast<int>(0.9 * histSize[0]);
	// 每个条目都绘制一条直线
	for (int h = 0; h < histSize[0]; h++)
	{
		float binVal = hist.at<float>(h);	// 取第h+1条的值
		std::cout << binVal << "\t";
		int intensity = static_cast<int>(binVal * hpt / maxVal);
		// 两点之间绘制一条线
		cv::line(histImage, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));
	}

	return histImage;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat &image, const cv::Mat &lookup)
{
	cv::Mat result;	// 输出图像
	cv::LUT(image, lookup, result);	// 应用查找表

	return result;
}

cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue)
{
	// 首先计算直方图
	cv::MatND hist = getHistogram(image);

	// 寻找直方图的左端
	int imin = 0;
	for (; imin < histSize[0]; imin++)
	{
		std::cout << hist.at<float>(imin) << std::endl;
		if (hist.at<float>(imin) > minValue)
		{
			break;
		}
	}

	// 寻找直方图的右端
	int imax = histSize[0] - 1;
	for (; imax >= 0; imax--)
	{
		std::cout << hist.at<float>(imax) << std::endl;
		if (hist.at<float>(imax) > minValue)
		{
			break;
		}
	}

	// 创建查找表
	int dim(256);
	cv::Mat lookup(1,	// 1 dimension
		&dim,	// 256 entries
		CV_8U	// uchar
		);
	// 填充查找表
	for (int i = 0; i < 256; i++)
	{
		// 确保数值位于imin和imax之间
		if (i < imin) lookup.at<uchar>(i) = 0;
		else if (i > imax) lookup.at<uchar>(i) = 255;
		// 线性映射
		else lookup.at<uchar>(i) = static_cast<uchar>(255.0 * (i-imin) / (imax-imin));
	}

	// 应用查找表
	cv::Mat result = applyLookUp(image, lookup);

	return result;
}

cv::Mat Histogram1D::equalize(const cv::Mat &image)
{
	cv::Mat result;
	cv::equalizeHist(image, result);	// 直方图均衡化，使直方图分布更均匀
	return result;
}
