#ifndef VISUALHANDGESTURERECOGNITION_H
#define VISUALHANDGESTURERECOGNITION_H

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QFont>
#include "ui_VisualHandGestureRecognition.h"

#include "Kinect2.h"

class VisualHandGestureRecognition : public QMainWindow
{
	Q_OBJECT

public:
	VisualHandGestureRecognition(QWidget *parent = 0);
	~VisualHandGestureRecognition();

	void initializeColorFrame();	// 初始化彩色帧
	void initializeDepthFrame();	// 初始化深度帧

	void showMatWithQtQlabel(const cv::Mat &img, QLabel *label);	// 用Qlabel显示Mat

private:
	Ui::VisualHandGestureRecognitionClass ui;

	Kinect2 *kinect;
	QTimer theTimer;  //定时器用于定时取帧

private slots:    //声明槽函数  
	void getFrame(); //实现定时从摄像头取帧并显示在label上的功能
};

#endif // VISUALHANDGESTURERECOGNITION_H
