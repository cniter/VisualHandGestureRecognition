#include "VisualHandGestureRecognition.h"

VisualHandGestureRecognition::VisualHandGestureRecognition(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	theTimer.start(30);              // 开始计时，超过30ms则发出timeout()信号，即每30ms取一帧  
	kinect = new Kinect2();
	/*kinect->showDepthVideo();*/
	kinect->openKinect();
	initializeColorFrame();
	initializeDepthFrame();

	/*信号和槽*/  
	connect(&theTimer, SIGNAL(timeout()), this, SLOT(getFrame()));  // 时间到，读取当前摄像头信息  	
}

VisualHandGestureRecognition::~VisualHandGestureRecognition()
{
	kinect->closeKinect();

	delete kinect;
	kinect = 0;
}

void VisualHandGestureRecognition::initializeColorFrame()
{
	kinect->getColorFrameSource();
	kinect->getColorFrameDescription();
	kinect->getColorFrameReader();
}

void VisualHandGestureRecognition::initializeDepthFrame()
{
	kinect->getDepthFrameSource();
	kinect->getDepthFrameDescription();
	kinect->getDepthFrameReader();
}

void VisualHandGestureRecognition::showMatWithQtQlabel(const cv::Mat &img, QLabel *label)
{
	// [Qt中用QLabel显示OpenCV中Mat图像数据出现扭曲现象的解决](http://lovelittlebean.blog.163.com/blog/static/11658218620125208212189/)
	QImage q_img;  
	if(img.channels() == 3)    // RGB image  
	{  
		q_img = QImage((const uchar*)(img.data), img.cols, img.rows, img.cols*img.channels(), QImage::Format_RGB888).rgbSwapped();  
	}else if (img.channels() == 4)	// RGBA image  
	{
		q_img = QImage((const uchar*)(img.data), img.cols, img.rows, img.cols*img.channels(), QImage::Format_RGB32);  
	}else              // gray image  
	{  
		q_img = QImage((const uchar*)(img.data), img.cols, img.rows, img.cols*img.channels(), QImage::Format_Indexed8);  
	}  

	// -------------- 图片适应label -------------------
	QImage q_label_img = q_img.scaled(label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 图片自适应label大小	
	label->setPixmap(QPixmap::fromImage(q_label_img));  // 将图片显示到label上 

	// -------------- label适应图片 -------------------
	/*label->setPixmap(QPixmap::fromImage(q_img));  // 显示在label中
	label->resize(label->pixmap()->size());  // 改变label的尺寸以自适应图像
	label->show();  */
}

 void VisualHandGestureRecognition::getFrame()
 {
	 cv::Mat color;
	 kinect->getColorFrame();
	 color = kinect->getColorShowImg();
	 if (color.data)
	 {
		 showMatWithQtQlabel(color, ui.label_show_color);
	 }

	 cv::Mat depth;
	 kinect->getDepthFrame();
	 depth = kinect->getDepthShowImg();
	 if (depth.data)
	 {
		 showMatWithQtQlabel(depth, ui.label_show_depth);
	 }

	 cv::Mat hand_gesture;
	 hand_gesture = kinect->getHandGesture();
	 if (!hand_gesture.empty())
	 {
		 showMatWithQtQlabel(hand_gesture, ui.label_show_hand_gesture);

		 //设置字体
		 QFont font("Microsoft YaHei", 36, 75); //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75） 
		 ui.label_hand_gesture_num->setFont(font);

		 //设置颜色
		 QPalette pa;
		 pa.setColor(QPalette::WindowText, Qt::red);
		 ui.label_hand_gesture_num->setPalette(pa);

		 ui.label_hand_gesture_num->setText(QString::fromStdString(kinect->getSHandGesture()));
	 }
 }

