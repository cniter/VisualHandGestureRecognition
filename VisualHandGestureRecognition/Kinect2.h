#pragma once

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

#include <Kinect.h>	// ʹ��Kinect

#include <iostream>

#include "HandDetector.h"
#include "SVMTrain.h"

class Kinect2
{
public:
	Kinect2(void);
	~Kinect2(void);

	void openKinect();	// ��Kinect������
	void closeKinect();	// �ر�Kinect������

	void setOpencvData();	// ����opencv��ʾ���ݣ��ڻ�ȡ֡���֮����ʾ��Ƶ֮ǰ����

	// -------------------------------- ��ȡ���ͼ�� ------------------------------------------
	void getDepthFrameSource();	// ��ȡ���֡��Դ
	void getDepthFrameDescription();	// ��ȡ���֡�����ӣ������֡����
	void getDepthFrameReader();	// ��ȡ���֡��ȡ��
	void getDepthFrame();	// ��ȡ���֡
	cv::Mat getDepthShowImg();	// ��ȡ������ʾ�����ͼ��CV_8UC1��
	cv::Mat getHandGesture();	// ��ȡ����������
	std::string getSHandGesture();	// ��ȡԤ���������
	void showDepthVideo();	// ��ʾ�����Ƶ

	// ---------------------------- ��ȡ��ɫͼ�� --------------------------------------
	void getColorFrameSource();	// ��ȡ��ɫ֡��Դ
	void getColorFrameDescription();	// ��ȡ��ɫ֡�����ӣ�����ɫ֡����
	void getColorFrameReader();	// ��ȡ��ɫ֡��ȡ��
	void getColorFrame();	// ��ȡ��ɫ֡
	cv::Mat getColorShowImg();	// ��ȡ������ʾ�Ĳ�ɫͼ��CV_8UC4��
	void showColorVideo();	// ��ʾ��ɫ��Ƶ

	// -------------------------- ��ȡ����Ǽܣ�Skeleton�� ---------------------------------------
	void getBodyFrameSource();	// ��ȡ�Ǽ�֡��Դ
	void getBodyFrameReader();	// ��ȡ�Ǽ�֡��ȡ��
	void getCoordinateMapper(); // ��ȡ����ӳ���
	void getBodyFrame();	// ��ȡ�Ǽ�֡
	void showBodyVideoOnColor();	// �ڲ�ɫͼ����ʾ�Ǽ���Ƶ
	void showBodyVideoOnDepth();	// �����ͼ����ʾ�Ǽ���Ƶ

private:
	// �ڲ�ɫͼ���ϻ��ƹǼ�ͼ�����ڹǼܹؽڵ������������ϵ��3D���꣩��������Ҫ��������任
	void drawLineOnColor(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper);
	// �����ͼ���ϻ��ƹǼ�ͼ�����ڹǼܹؽڵ������������ϵ��3D���꣩��������Ҫ��������任
	void drawLineOnDepth(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper);

private:
	HandDetector hd;
	SVMTrain *st;

	IKinectSensor *p_sensor;	// Ĭ��Kinect������
	bool is_open;	// �ж�Kinect�Ƿ��

	// ---------------------- ���֡���� --------------------------------------
	IDepthFrameSource *p_depth_frame_source;	// ���֡��Դ
	int depth_width;	// ���֡��512��
	int depth_height;	// ���֡�ߣ�424��
	UINT16 depth_min, depth_max;	// ���ֵ��Χ����С�����
	cv::Mat depth_src_img;	// ԭʼ���ͼ��CV_16UC1��
	cv::Mat depth_show_img;	// ������ʾ�����ͼ��CV_8UC1��
	IDepthFrameReader *p_depth_frame_reader;	// ���֡��ȡ��
	cv::Mat hand_gesture;	// ����������
	std::string s_hand_gesture;	// Ԥ���������

	// ------------------- ��ɫ֡���� ---------------------------------------------
	IColorFrameSource *p_color_frame_source;	// ��ɫ֡��Դ
	int color_width;	// ��ɫ֡��1920��
	int color_height;	// ��ɫ֡�ߣ�1080��
	cv::Mat color_show_img;	// ������ʾ�Ĳ�ɫͼ��CV_8UC4��
	UINT u_color_buffer_size;	// ��ɫͼ����ռ�ڴ��С
	IColorFrameReader *p_color_frame_reader;	// ��ɫ֡��ȡ��

	// --------------------- �Ǽ�֡���� --------------------------------------------
	IBodyFrameSource *p_body_frame_source;	// �Ǽ�֡��Դ
	IBody **body_data;	// ����Ǽ�����
	INT32 body_count;	// ��׷������Ǽ�������6����
	ICoordinateMapper *p_coordinate_mapper; // ����ת��ʱ������ӳ���
	IBodyFrameReader *p_body_frame_reader;	// �Ǽ�֡��ȡ��
};


