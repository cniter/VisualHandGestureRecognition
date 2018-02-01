#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp> 
#include <opencv2/objdetect/objdetect.hpp>	// HOGDescriptor

#include "Utilities.h"

class SVMTrain
{
public:
	SVMTrain(void);
	~SVMTrain(void);

	SVMTrain(const char *hand_gesture_folder, const char *xml);

	void getHOGFeatures(const cv::Mat &image, cv::Mat &features);	// 获取HOG特征
	void get_train_data();	// 获取训练数据

	void trainAuto();	// 开始训练模型
	void loadModel();	// 加载已有的训练好的模型

	std::string predictResult(const cv::Mat &image);	// 预测结果
	void test(const cv::Mat &image);	// 测试训练出的模型（单张图片）
	void test();	// 测试训练出的模型(测试数据集)

	const char *hand_gesture_folder_;	// 训练数据文件夹
	const char *svm_xml_;	// 训练出的模型文件

private:
	CvSVM svm_;
	cv::Mat training_data_;	// 训练数据
	cv::Mat classes_;	// 类别、标签
};

