#include "Kinect2.h"

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

Kinect2::Kinect2(void)
{
	p_sensor = nullptr;
	is_open = false;

	p_depth_frame_source = nullptr;
	depth_width = 0;
	depth_height = 0;
	p_depth_frame_reader = nullptr;

	p_color_frame_source = nullptr;
	color_width = 0;
	color_height = 0;
	p_color_frame_reader = nullptr;

	p_body_frame_source = nullptr;
	body_data = nullptr;
	body_count = 0;
	p_coordinate_mapper = nullptr;
	p_body_frame_reader = nullptr;

	st = new SVMTrain("../data/train", "../data/hand_gesture_model_HOG_LINEAR_13068.xml");
	st->loadModel();
}


Kinect2::~Kinect2(void)
{
	if (nullptr != p_depth_frame_reader)
	{
		std::cout << "Release depth frame reader" << std::endl;
		p_depth_frame_reader->Release();
		p_depth_frame_reader = nullptr;
	}

	if (nullptr != p_depth_frame_source)
	{
		std::cout << "Release depth frame source" << std::endl;
		p_depth_frame_source->Release();
		p_depth_frame_source = nullptr;
	}

	if (nullptr != p_color_frame_reader)
	{
		std::cout << "Release color frame reader" << std::endl;
		p_color_frame_reader->Release();
		p_color_frame_reader = nullptr;
	}

	if (nullptr != p_color_frame_source)
	{
		std::cout << "Release color frame source" << std::endl;
		p_color_frame_source->Release();
		p_color_frame_source = nullptr;
	}

	if (nullptr != p_body_frame_reader)
	{
		std::cout << "Release body frame reader" << std::endl;
		p_body_frame_reader->Release();
		p_body_frame_reader = nullptr;
	}

	if (nullptr != p_coordinate_mapper)
	{
		std::cout << "Release coordinate mapper" << std::endl;
		p_coordinate_mapper->Release();
		p_coordinate_mapper = nullptr;
	}

	if (NULL != body_data)
	{
		delete [] body_data;
		body_data = NULL;
	}

	if (nullptr != p_body_frame_source)
	{
		std::cout << "Release body frame source" << std::endl;
		p_body_frame_source->Release();
		p_body_frame_source = nullptr;
	}

	if (nullptr != p_sensor)
	{
		std::cout << "Release sensor" << std::endl;
		p_sensor->Release();
		p_sensor = nullptr;
	}

	if (NULL != st)
	{
		delete st;
		st = NULL;
	}
}

void Kinect2::openKinect()
{
	// 获取默认的Kinect传感器
	std::cout << "Try to get default sensor" << std::endl;
	p_sensor = nullptr;
	if (GetDefaultKinectSensor(&p_sensor) != S_OK)
	{
		std::cerr << "Get Sensor failed" << std::endl;
		exit(-1);
	}

	// 打开传感器
	std::cout << "Try to open sensor" << std::endl;
	if (p_sensor->Open() != S_OK)
	{
		std::cerr << "Can't open sensor" << std::endl;
		exit(-1);
	}
	is_open = true;
}

void Kinect2::closeKinect()
{
	if (is_open)
	{
		std::cout << "close sensor" << std::endl;
		p_sensor->Close();
	}
}

void Kinect2::setOpencvData()
{
	// ---------------- 深度帧显示- ---------------------
	depth_src_img.create(depth_height, depth_width, CV_16UC1);
	depth_show_img.create(depth_height, depth_width, CV_8UC1);
	cv::namedWindow( "Depth Map" , CV_WINDOW_NORMAL);
	cv::resizeWindow("Depth Map", 640, 480);

	// --------------- 彩色帧显示 ------------------------------
	color_show_img.create(color_height, color_width, CV_8UC4);
	u_color_buffer_size = color_height * color_width * 4 * sizeof(BYTE);
	cv::namedWindow("Color Map", CV_WINDOW_NORMAL);
	cv::resizeWindow("Color Map", 640, 480);
}



void Kinect2::getDepthFrameSource()
{
	std::cout << "Try to get depth frame source" << std::endl;
	p_depth_frame_source = nullptr;
	if (p_sensor->get_DepthFrameSource(&p_depth_frame_source) != S_OK)
	{
		std::cerr << "Can't get depth frame source" << std::endl;
		exit(-1);
	}

	// 获取深度值范围
	depth_min = 0, depth_max = 0;
	p_depth_frame_source->get_DepthMinReliableDistance(&depth_min);
	p_depth_frame_source->get_DepthMaxReliableDistance(&depth_max);
	std::cout << "Reliable Distance: " << depth_min << " - " << depth_max << std::endl;
}

void Kinect2::getDepthFrameDescription()
{
	std::cout << "get depth frame description" << std::endl;
	depth_width = 0;
	depth_height = 0;
	IFrameDescription *p_depth_frame_description = nullptr;
	if (p_depth_frame_source->get_FrameDescription(&p_depth_frame_description) == S_OK)
	{
		p_depth_frame_description->get_Width(&depth_width);
		p_depth_frame_description->get_Height(&depth_height);
		std::cout << "depth_width: " << depth_width << " \tdepth_height: " << depth_height << std::endl;
	}
	p_depth_frame_description->Release();
	p_depth_frame_description = nullptr;

	// ---------------- 深度帧显示- ---------------------
	depth_src_img.create(depth_height, depth_width, CV_16UC1);
	depth_show_img.create(depth_height, depth_width, CV_8UC1);
	/*cv::namedWindow( "Depth Map" , CV_WINDOW_NORMAL);
	cv::resizeWindow("Depth Map", 640, 480);*/
}

void Kinect2::getDepthFrameReader()
{
	std::cout << "Try to get depth frame reader" << std::endl;
	p_depth_frame_reader = nullptr;
	if (p_depth_frame_source->OpenReader(&p_depth_frame_reader) != S_OK)
	{
		std::cerr << "Can't get depth frame reader" << std::endl;
		exit(-1);
	}
}
int i = 0;
void Kinect2::getDepthFrame()
{
	IDepthFrame *p_depth_frame = nullptr;
	if (p_depth_frame_reader->AcquireLatestFrame(&p_depth_frame) == S_OK)
	{
		// 4c. copy the depth map to image
		if (p_depth_frame->CopyFrameDataToArray(depth_width * depth_height, reinterpret_cast<UINT16 *>(depth_src_img.data)) == S_OK)
		{
			// 4d. convert from 16bit to 8bit
			depth_src_img.convertTo(depth_show_img, CV_8UC1, 255.0f / depth_max);
			//cv::imshow("Depth Map", depth_show_img);
			cv::Mat hand = hd.detectHand(depth_show_img);

			// ---------- 采集数据 --------------
			/*std::string s = std::to_string(i);
			i++;
			std::string img_name = "./data/" + s + ".png";
			cv::imwrite(img_name, hand);*/

			hand.copyTo(hand_gesture);	// 获取检测出的手势
			cv::cvtColor(depth_show_img, depth_show_img, CV_GRAY2BGR);
			s_hand_gesture = st->predictResult(hand);	// 获取预测出的手势
			cv::putText(depth_show_img, s_hand_gesture, cv::Point(100, 100), cv::FONT_HERSHEY_COMPLEX , 1.0, cv::Scalar(0, 0, 255));	// 在图像上输出字符串
			//cv::imshow("Result", depth_show_img);
		}
		else
		{
			std::cerr << "Data copy error" << std::endl;
		}

		// 4e. release frame
		p_depth_frame->Release();
		p_depth_frame = nullptr;
	}
}

cv::Mat Kinect2::getDepthShowImg()
{
	return depth_show_img;
}

cv::Mat Kinect2::getHandGesture()
{
	return hand_gesture;
}

std::string Kinect2::getSHandGesture()
{
	return s_hand_gesture;
}

void Kinect2::showDepthVideo()
{
	openKinect();
	getDepthFrameSource();
	getDepthFrameDescription();
	getDepthFrameReader();
	//setOpencvData();
	st->loadModel();
	while (true)
	{

		getDepthFrame();
		if (cv::waitKey(30) == VK_ESCAPE)
		{
			break;
		}
		/*if (cv::waitKey(30) == VK_SPACE)
		{
			getchar();
		}*/
	}
	closeKinect();
}



void Kinect2::getColorFrameSource()
{
	std::cout << "Try to get color frame source" << std::endl;
	p_color_frame_source = nullptr;
	if (p_sensor->get_ColorFrameSource(&p_color_frame_source) != S_OK)
	{
		std::cerr << "Can't get color frame source" << std::endl;
		exit(-1);
	}
}

void Kinect2::getColorFrameDescription()
{
	std::cout << "get color frame description" << std::endl;
	color_width = 0;
	color_height = 0;
	IFrameDescription *p_color_frame_description = nullptr;
	if (p_color_frame_source->get_FrameDescription(&p_color_frame_description) == S_OK)
	{
		p_color_frame_description->get_Width(&color_width);
		p_color_frame_description->get_Height(&color_height);
		std::cout << "color_width: " << color_width << " \tcolor_height: " << color_height << std::endl;
	}
	p_color_frame_description->Release();
	p_color_frame_description = nullptr;

	// --------------- 彩色帧显示 ------------------------------
	color_show_img.create(color_height, color_width, CV_8UC4);
	u_color_buffer_size = color_height * color_width * 4 * sizeof(BYTE);
	/*cv::namedWindow("Color Map", CV_WINDOW_NORMAL);
	cv::resizeWindow("Color Map", 640, 480);*/
}

void Kinect2::getColorFrameReader()
{
	std::cout << "Try to get color frame reader" << std::endl;
	p_color_frame_reader = nullptr;
	if (p_color_frame_source->OpenReader(&p_color_frame_reader) != S_OK)
	{
		std::cerr << "Can't get color frame reader" << std::endl;
		exit(-1);
	}
}

void Kinect2::getColorFrame()
{
	IColorFrame *p_color_frame = nullptr;
	if (p_color_frame_reader->AcquireLatestFrame(&p_color_frame) == S_OK)
	{
		// 4c. copy the color map to OpenCV image
		/*ColorImageFormat color_frame_format;
		p_color_frame->get_RawColorImageFormat(&color_frame_format);
		std::cout << "Color Frame Format:" << color_frame_format << std::endl;*/
		if (p_color_frame->CopyConvertedFrameDataToArray(u_color_buffer_size, reinterpret_cast<BYTE *>(color_show_img.data), ColorImageFormat_Bgra) == S_OK)
		{
			//cv::imshow("Color Map", color_show_img);
			//color_show_img.copyTo(output_color);
		}
		else
		{
			std::cerr << "Data copy error" << std::endl;
		}

		// 4e. release frame
		p_color_frame->Release();
		p_color_frame = nullptr;
	}
}

cv::Mat Kinect2::getColorShowImg()
{
	return color_show_img;
}

void Kinect2::showColorVideo()
{
	openKinect();
	getColorFrameSource();
	getColorFrameDescription();
	getColorFrameReader();
	//setOpencvData();
	while (true)
	{
		getColorFrame();
		if (cv::waitKey(30) == VK_ESCAPE)
		{
			break;
		}
		/*if (cv::waitKey(30) == VK_SPACE)
		{
			getchar();
		}*/
	}
	closeKinect();
}



void Kinect2::getBodyFrameSource()
{
	std::cout << "Try to get body frame source" << std::endl;
	p_body_frame_source = nullptr;
	if (p_sensor->get_BodyFrameSource(&p_body_frame_source) != S_OK)
	{
		std::cerr << "Can't get body frame source" << std::endl;
		exit(-1);
	}



	if (p_body_frame_source->get_BodyCount(&body_count) != S_OK)
	{
		std::cerr << "Can't get body count" << std::endl;
		exit(-1);
	}
	std::cout << " > Can trace " << body_count << " bodies" << std::endl;
	body_data = new IBody *[body_count];
	for (int i = 0; i < body_count; ++i)
		body_data[i] = nullptr;
}

void Kinect2::getBodyFrameReader()
{
	std::cout << "Try to get body frame reader" << std::endl;
	p_body_frame_reader = nullptr;
	if (p_body_frame_source->OpenReader(&p_body_frame_reader) != S_OK)
	{
		std::cerr << "Can't get color frame reader" << std::endl;
		exit(-1);
	}
}

void Kinect2::getCoordinateMapper()
{
	p_coordinate_mapper = nullptr;
	if (p_sensor->get_CoordinateMapper(&p_coordinate_mapper) != S_OK)
	{
		std::cout << "Can't get coordinate mapper" << std::endl;
		exit(-1);
	}
}

void Kinect2::drawLineOnColor(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper)
{
	if (rJ1.TrackingState == TrackingState_NotTracked || rJ2.TrackingState == TrackingState_NotTracked)
		exit(-1);

	ColorSpacePoint ptJ1, ptJ2;	// 彩色坐标系的点
	pCMapper->MapCameraPointToColorSpace(rJ1.Position, &ptJ1);	// 摄像头坐标系的点转彩色坐标系
	pCMapper->MapCameraPointToColorSpace(rJ2.Position, &ptJ2);

	cv::circle(src_img, cv::Point(ptJ1.X, ptJ1.Y), 10, cv::Scalar(255, 0, 0), 10, CV_FILLED);	// 实心圆表示关节
	cv::circle(src_img, cv::Point(ptJ2.X, ptJ2.Y), 10, cv::Scalar(255, 0, 0), 10, CV_FILLED);	// 实心圆表示关节
	cv::line(src_img, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0, 0, 255), 5);	// 连接两关节
}

void Kinect2::drawLineOnDepth(cv::Mat &src_img, const Joint &rJ1, const Joint &rJ2, ICoordinateMapper *pCMapper)
{
	if (rJ1.TrackingState == TrackingState_NotTracked || rJ2.TrackingState == TrackingState_NotTracked)
		exit(-1);

	DepthSpacePoint ptJ1, ptJ2;	// 深度坐标系的点
	pCMapper->MapCameraPointToDepthSpace(rJ1.Position, &ptJ1);	// 摄像头坐标系的点转深度坐标系
	pCMapper->MapCameraPointToDepthSpace(rJ2.Position, &ptJ2);

	cv::circle(src_img, cv::Point(ptJ1.X, ptJ1.Y), 5, cv::Scalar(255, 0, 0), 5, CV_FILLED);	// 实心圆表示关节
	cv::circle(src_img, cv::Point(ptJ2.X, ptJ2.Y), 5, cv::Scalar(255, 0, 0), 5, CV_FILLED);	// 实心圆表示关节
	cv::line(src_img, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0, 0, 255), 2);	// 连接两关节
}

void Kinect2::getBodyFrame()
{
	cv::Mat mImg = color_show_img.clone();
	cv::Mat depth_img = depth_show_img.clone();
	//cvtColor(depth_img, depth_img, CV_GRAY2BGR);
	IBodyFrame* p_body_frame = nullptr;
	if (p_body_frame_reader->AcquireLatestFrame(&p_body_frame) == S_OK)
	{
		// 4b. get Body data
		if (p_body_frame->GetAndRefreshBodyData(body_count, body_data) == S_OK)
		{
			// 4c. for each body
			for (int i = 0; i < body_count; ++i)
			{
				IBody* pBody = body_data[i];

				// check if is tracked
				BOOLEAN bTracked = false;
				if ((pBody->get_IsTracked(&bTracked) == S_OK) && bTracked)
				{
					// get joint position
					Joint aJoints[JointType::JointType_Count];
					if (pBody->GetJoints(JointType::JointType_Count, aJoints) == S_OK)
					{
						// 在彩色帧上绘制骨架
						drawLineOnColor(mImg, aJoints[JointType_SpineBase], aJoints[JointType_SpineMid], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_Neck], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_Neck], aJoints[JointType_Head], p_coordinate_mapper);

						drawLineOnColor(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_WristLeft], aJoints[JointType_HandLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_HandLeft], aJoints[JointType_HandTipLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_HandLeft], aJoints[JointType_ThumbLeft], p_coordinate_mapper);

						drawLineOnColor(mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_ElbowRight], aJoints[JointType_WristRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_WristRight], aJoints[JointType_HandRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_HandRight], aJoints[JointType_HandTipRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_HandRight], aJoints[JointType_ThumbRight], p_coordinate_mapper);

						drawLineOnColor(mImg, aJoints[JointType_SpineBase], aJoints[JointType_HipLeft], p_coordinate_mapper);
						/*drawLineOnColor(mImg, aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_AnkleLeft], aJoints[JointType_FootLeft], p_coordinate_mapper);*/

						drawLineOnColor(mImg, aJoints[JointType_SpineBase], aJoints[JointType_HipRight], p_coordinate_mapper);
						/*drawLineOnColor(mImg, aJoints[JointType_HipRight], aJoints[JointType_KneeRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight], p_coordinate_mapper);
						drawLineOnColor(mImg, aJoints[JointType_AnkleRight], aJoints[JointType_FootRight], p_coordinate_mapper);*/



						// 在深度帧上绘制骨架
						drawLineOnDepth(depth_img, aJoints[JointType_SpineBase], aJoints[JointType_SpineMid], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_SpineShoulder], aJoints[JointType_Neck], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_Neck], aJoints[JointType_Head], p_coordinate_mapper);

						drawLineOnDepth(depth_img, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_WristLeft], aJoints[JointType_HandLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_HandLeft], aJoints[JointType_HandTipLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_HandLeft], aJoints[JointType_ThumbLeft], p_coordinate_mapper);

						drawLineOnDepth(depth_img, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_ElbowRight], aJoints[JointType_WristRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_WristRight], aJoints[JointType_HandRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_HandRight], aJoints[JointType_HandTipRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_HandRight], aJoints[JointType_ThumbRight], p_coordinate_mapper);

						drawLineOnDepth(depth_img, aJoints[JointType_SpineBase], aJoints[JointType_HipLeft], p_coordinate_mapper);
						/*drawLineOnDepth(depth_img, aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_AnkleLeft], aJoints[JointType_FootLeft], p_coordinate_mapper);*/

						drawLineOnDepth(depth_img, aJoints[JointType_SpineBase], aJoints[JointType_HipRight], p_coordinate_mapper);
						/*drawLineOnDepth(depth_img, aJoints[JointType_HipRight], aJoints[JointType_KneeRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight], p_coordinate_mapper);
						drawLineOnDepth(depth_img, aJoints[JointType_AnkleRight], aJoints[JointType_FootRight], p_coordinate_mapper);*/
					}
				}
			}
		}
		else
		{
			std::cerr << "Can't read body data" << std::endl;
		}

		// 4e. release frame
		p_body_frame->Release();
		p_body_frame = nullptr;
	}

	cv::namedWindow("Body Image", CV_WINDOW_NORMAL);
	cv::resizeWindow("Body Image", 640, 480);
	cv::imshow("Body Image",mImg);
	cv::imshow("Body Image2",depth_img);
}

void Kinect2::showBodyVideoOnColor()
{
	openKinect();

	getColorFrameSource();
	getColorFrameDescription();
	getColorFrameReader();

	getDepthFrameSource();
	getDepthFrameDescription();
	getDepthFrameReader();
	//st->loadModel();

	getBodyFrameSource();
	getBodyFrameReader();
	getCoordinateMapper();

	while (true)
	{
		getColorFrame();
		getDepthFrame();
		getBodyFrame();
		if (cv::waitKey(30) == VK_ESCAPE)
		{
			break;
		}
		/*if (cv::waitKey(30) == VK_SPACE)
		{
			getchar();
		}*/
	}
	closeKinect();
}
