#pragma once

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

#include "Histogram1D.h"

class HandDetector
{
public:
	HandDetector(void);
	~HandDetector(void);

	// 预处理（阈值分割(基于直方图灰度级分割)和中值滤波处理），获取手形二值图
	void preProcess(const cv::Mat &input_src_gray,	// 输入灰度图
		cv::Mat &out_hand_binary);	// 输出含手形的二值图（可能还有其他的干扰块）

	// 提取手形轮廓，去除其他的干扰块，返回只含手形的二值图
	cv::Mat extractHandContour(const cv::Mat &input_hand_binary,	// 输入可能含干扰块的手形二值图 
		std::vector<cv::Point> &output_hand_contour);	// 输出手形轮廓上的像素点

	// 旋转校正，返回校正后的图形（即手势图垂直，手掌向上）
	cv::Mat rotationCorrect(const cv::Mat &input_hand_binary,	// 输入只含手形的手形二值图
		const std::vector<cv::Point> &input_hand_contour);	// 输入对应的手形轮廓

	// 使用距离变换获取掌心坐标，获取距离变换图中最大距离，获取距离变换后的图
	void extractPalmCenter(const cv::Mat &input_hand_binary,	// 输入旋转校正后的手形二值图
		cv::Point &output_palm_center,	//  输出掌心
		float &output_max_distance,	// 输出最大距离
		cv::Mat &output_dist_transformed);	// 输出距离变换后的图

	// 分割手掌（直接利用距离变换的最大距离和掌心坐标，默认掌心以下最大距离个像素点），返回手掌二值图
	cv::Mat segmentPalm(const cv::Mat &input_hand_binary,	// 输入旋转校正后的手形二值图
		const cv::Point &input_palm_center,	// 输入掌心坐标
		const float &input_max_distance);	// 输入掌心离最近边缘的距离

	// 分割手掌（直接利用距离变换后的图像，手掌和手臂为两个距离峰值，而手腕的距离值为峰谷，找到峰谷，直接分割），返回手掌二值图
	cv::Mat segmentPalm(const cv::Mat &input_hand_binary,	// 输入旋转校正后的手形二值图
		const cv::Mat input_dist_transformed);	// 输入距离变换后的图

	// 检测手掌
	cv::Mat detectHand(const cv::Mat &input_src_gray);	// 输入原始灰度图

private:
	Histogram1D hist;

	// const double *histogram表示直方图数组，int size_h表示直方图项的数目
	bool isBimodal(const std::vector<double> histogram, int size_h = 256);	// 判断是否是双峰形状

	// const double *histogram表示直方图数组，int size_h表示直方图项的数目
	int getMinimumThreshold(const std::vector<double> histogram, int size_h = 256);	// 取峰谷为阈值
};

