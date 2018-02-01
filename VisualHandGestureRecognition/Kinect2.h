#pragma once

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

#include <Kinect.h>	// 使用Kinect

#include <iostream>

#include "HandDetector.h"
#include "SVMTrain.h"

class Kinect2
{
public:
	Kinect2(void);
	~Kinect2(void);

	void openKinect();	// 打开Kinect传感器
	void closeKinect();	// 关闭Kinect传感器

	void setOpencvData();	// 设置opencv显示数据，在获取帧宽高之后，显示视频之前调用

	// -------------------------------- 获取深度图像 ------------------------------------------
	void getDepthFrameSource();	// 获取深度帧资源
	void getDepthFrameDescription();	// 获取深度帧描述子，即深度帧宽、高
	void getDepthFrameReader();	// 获取深度帧读取器
	void getDepthFrame();	// 获取深度帧
	cv::Mat getDepthShowImg();	// 获取最终显示的深度图（CV_8UC1）
	cv::Mat getHandGesture();	// 获取检测出的手势
	std::string getSHandGesture();	// 获取预测出的手势
	void showDepthVideo();	// 显示深度视频

	// ---------------------------- 获取彩色图像 --------------------------------------
	void getColorFrameSource();	// 获取彩色帧资源
	void getColorFrameDescription();	// 获取彩色帧描述子，即彩色帧宽、高
	void getColorFrameReader();	// 获取彩色帧读取器
	void getColorFrame();	// 获取彩色帧
	cv::Mat getColorShowImg();	// 获取最终显示的彩色图（CV_8UC4）
	void showColorVideo();	// 显示彩色视频

	// -------------------------- 获取人体骨架（Skeleton） ---------------------------------------
	void getBodyFrameSource();	// 获取骨架帧资源
	void getBodyFrameReader();	// 获取骨架帧读取器
	void getCoordinateMapper(); // 获取坐标映射表
	void getBodyFrame();	// 获取骨架帧
	void showBodyVideoOnColor();	// 在彩色图中显示骨架视频
	void showBodyVideoOnDepth();	// 在深度图中显示骨架视频

private:
	// 在彩色图像上绘制骨架图，由于骨架关节点是摄像机坐标系（3D坐标），所以需要进行坐标变换
	void drawLineOnColor(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper);
	// 在深度图像上绘制骨架图，由于骨架关节点是摄像机坐标系（3D坐标），所以需要进行坐标变换
	void drawLineOnDepth(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper);

private:
	HandDetector hd;
	SVMTrain *st;

	IKinectSensor *p_sensor;	// 默认Kinect传感器
	bool is_open;	// 判断Kinect是否打开

	// ---------------------- 深度帧参数 --------------------------------------
	IDepthFrameSource *p_depth_frame_source;	// 深度帧资源
	int depth_width;	// 深度帧宽（512）
	int depth_height;	// 深度帧高（424）
	UINT16 depth_min, depth_max;	// 深度值范围（最小—最大）
	cv::Mat depth_src_img;	// 原始深度图（CV_16UC1）
	cv::Mat depth_show_img;	// 最终显示的深度图（CV_8UC1）
	IDepthFrameReader *p_depth_frame_reader;	// 深度帧读取器
	cv::Mat hand_gesture;	// 检测出的手势
	std::string s_hand_gesture;	// 预测出的手势

	// ------------------- 彩色帧参数 ---------------------------------------------
	IColorFrameSource *p_color_frame_source;	// 彩色帧资源
	int color_width;	// 彩色帧宽（1920）
	int color_height;	// 彩色帧高（1080）
	cv::Mat color_show_img;	// 最终显示的彩色图（CV_8UC4）
	UINT u_color_buffer_size;	// 彩色图像所占内存大小
	IColorFrameReader *p_color_frame_reader;	// 彩色帧读取器

	// --------------------- 骨架帧参数 --------------------------------------------
	IBodyFrameSource *p_body_frame_source;	// 骨架帧资源
	IBody **body_data;	// 人体骨架数据
	INT32 body_count;	// 可追踪人体骨架数量（6个）
	ICoordinateMapper *p_coordinate_mapper; // 坐标转换时的坐标映射表
	IBodyFrameReader *p_body_frame_reader;	// 骨架帧读取器
};


