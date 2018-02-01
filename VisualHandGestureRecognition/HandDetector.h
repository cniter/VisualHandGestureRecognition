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

	// Ԥ������ֵ�ָ�(����ֱ��ͼ�Ҷȼ��ָ�)����ֵ�˲���������ȡ���ζ�ֵͼ
	void preProcess(const cv::Mat &input_src_gray,	// ����Ҷ�ͼ
		cv::Mat &out_hand_binary);	// ��������εĶ�ֵͼ�����ܻ��������ĸ��ſ飩

	// ��ȡ����������ȥ�������ĸ��ſ飬����ֻ�����εĶ�ֵͼ
	cv::Mat extractHandContour(const cv::Mat &input_hand_binary,	// ������ܺ����ſ�����ζ�ֵͼ 
		std::vector<cv::Point> &output_hand_contour);	// ������������ϵ����ص�

	// ��תУ��������У�����ͼ�Σ�������ͼ��ֱ���������ϣ�
	cv::Mat rotationCorrect(const cv::Mat &input_hand_binary,	// ����ֻ�����ε����ζ�ֵͼ
		const std::vector<cv::Point> &input_hand_contour);	// �����Ӧ����������

	// ʹ�þ���任��ȡ�������꣬��ȡ����任ͼ�������룬��ȡ����任���ͼ
	void extractPalmCenter(const cv::Mat &input_hand_binary,	// ������תУ��������ζ�ֵͼ
		cv::Point &output_palm_center,	//  �������
		float &output_max_distance,	// ���������
		cv::Mat &output_dist_transformed);	// �������任���ͼ

	// �ָ����ƣ�ֱ�����þ���任����������������꣬Ĭ��������������������ص㣩���������ƶ�ֵͼ
	cv::Mat segmentPalm(const cv::Mat &input_hand_binary,	// ������תУ��������ζ�ֵͼ
		const cv::Point &input_palm_center,	// ������������
		const float &input_max_distance);	// ���������������Ե�ľ���

	// �ָ����ƣ�ֱ�����þ���任���ͼ�����ƺ��ֱ�Ϊ���������ֵ��������ľ���ֵΪ��ȣ��ҵ���ȣ�ֱ�ӷָ���������ƶ�ֵͼ
	cv::Mat segmentPalm(const cv::Mat &input_hand_binary,	// ������תУ��������ζ�ֵͼ
		const cv::Mat input_dist_transformed);	// �������任���ͼ

	// �������
	cv::Mat detectHand(const cv::Mat &input_src_gray);	// ����ԭʼ�Ҷ�ͼ

private:
	Histogram1D hist;

	// const double *histogram��ʾֱ��ͼ���飬int size_h��ʾֱ��ͼ�����Ŀ
	bool isBimodal(const std::vector<double> histogram, int size_h = 256);	// �ж��Ƿ���˫����״

	// const double *histogram��ʾֱ��ͼ���飬int size_h��ʾֱ��ͼ�����Ŀ
	int getMinimumThreshold(const std::vector<double> histogram, int size_h = 256);	// ȡ���Ϊ��ֵ
};

