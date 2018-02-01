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

	void initializeColorFrame();	// ��ʼ����ɫ֡
	void initializeDepthFrame();	// ��ʼ�����֡

	void showMatWithQtQlabel(const cv::Mat &img, QLabel *label);	// ��Qlabel��ʾMat

private:
	Ui::VisualHandGestureRecognitionClass ui;

	Kinect2 *kinect;
	QTimer theTimer;  //��ʱ�����ڶ�ʱȡ֡

private slots:    //�����ۺ���  
	void getFrame(); //ʵ�ֶ�ʱ������ͷȡ֡����ʾ��label�ϵĹ���
};

#endif // VISUALHANDGESTURERECOGNITION_H
