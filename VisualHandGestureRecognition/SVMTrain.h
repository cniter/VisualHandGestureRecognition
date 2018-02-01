#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include <opencv2/core/core.hpp>  
#include <opencv2/ml/ml.hpp> 

#include "Utilities.h"

class SVMTrain
{
public:
	SVMTrain(void);
	~SVMTrain(void);

	SVMTrain(const char *hand_gesture_folder, const char *xml);

	void getHOGFeatures(const cv::Mat &image, cv::Mat &features);	// ��ȡHOG����
	void getTrainData();	// ��ȡѵ������
	void trainAuto();	// ��ʼѵ��ģ��
	void loadModel();	// �������е�ѵ���õ�ģ��
	std::string predictResult(const cv::Mat &image);	// Ԥ����
	void test(const cv::Mat &image);	// ����ѵ������ģ�ͣ�����ͼƬ��
	void test();	// ����ѵ������ģ��(�������ݼ�)

	const char *hand_gesture_folder_;
	const char *svm_xml_;

private:
	Utilities util;

	CvSVM svm_;
	cv::Mat training_data_;	// ѵ������
	cv::Mat classes_;	// ��𡢱�ǩ
};
