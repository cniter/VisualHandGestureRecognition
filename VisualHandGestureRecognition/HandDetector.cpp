#include "HandDetector.h"


HandDetector::HandDetector(void)
{
}


HandDetector::~HandDetector(void)
{
}

void HandDetector::preProcess(const cv::Mat &input_src_gray, cv::Mat &out_hand_binary)
{
	out_hand_binary.create(input_src_gray.size(), CV_8UC1);
	//cv::imshow("Hist", hist.getHistogramImage(input_src_gray));

	cv::Mat stretched = hist.stretch(input_src_gray);	// 直方图拉伸
	//cv::imshow("Hist Stretched", stretched);
	//cv::imshow("Hist Image", hist.getHistogramImage(stretched));
	// 获取直方图
	cv::MatND histogram = hist.getHistogram(stretched);

	// 从像素值为1开始寻找直方图左边第一个非零个数像素值
	int bin = 0;	// 记录分割终点像素值
	for (int i = 1; i < 256; i++)
	{
		printf("%f\t", histogram.at<float>(i));
		if (histogram.at<float>(i) > 300)	// 手的像素至少有300个
		{
			//continue;
			bin = i;
			break;
		}
	}
	printf("\n%d\n", bin);
	cv::inRange(stretched, cv::Scalar(bin - 5), cv::Scalar(bin + 4), out_hand_binary);	// bin-5~bin+4 || 0.97*bin~1.03*bin 之间的像素值为255，其他为0
	//cv::imshow("Hand Bianry Without MedianBlur", out_hand_binary);
}

cv::Mat HandDetector::extractHandContour(const cv::Mat &input_hand_binary, std::vector<cv::Point> &output_hand_contour)
{
	std::vector<std::vector<cv::Point>> contours;	// 轮廓向量
	cv::findContours(input_hand_binary,
		contours,	// 轮廓的数组
		CV_RETR_EXTERNAL,	// 获取外轮廓，而参数CV_RETR_LIST可获取轮廓中洞组成的轮廓(内部轮廓)
		CV_CHAIN_APPROX_SIMPLE);	// 获取每个轮廓的末端像素（区域外轮廓简单拟合）

	// 找到最大的轮廓，即为手部轮廓  
	int index(0);    
	double area(0.), maxArea(0.);    
	for (int i = 0; i < contours.size(); i++)    
	{    
		area = cv::contourArea(cv::Mat(contours[i]));    
		if (area >= maxArea)    
		{    
			maxArea = area;    
			index = i;    
		}  
	}    

	output_hand_contour = contours[index];	// 获取手部轮廓

	// 在黑色图像上绘制轮廓
	cv::Mat hand_binary(input_hand_binary.size(), CV_8UC1, cv::Scalar(0));
	cv::drawContours(hand_binary, contours,
		index,	// 只绘制手部轮廓
		cv::Scalar(255),	// 颜色为白色
		CV_FILLED);	// 完全填充
	//cv::imshow("Hand Binary Without Noise", hand_binary);

	// 闭运算：先膨胀，后腐蚀；作用为填充白色前景物体中的黑色小洞
	//cv::Mat element(9, 9, CV_8U, cv::Scalar(1));	// 使用9x9的结构元素
	//cv::morphologyEx(hand_binary, hand_binary, cv::MORPH_CLOSE, element);	// 闭运算
	//cv::imshow("close", hand_binary);

	cv::medianBlur(hand_binary, hand_binary, 9);	// 应用中值滤波
	//cv::imshow("Smooth Hand Binary Without Noise", hand_binary);

	return hand_binary;
}

cv::Mat HandDetector::rotationCorrect(const cv::Mat &input_hand_binary, const std::vector<cv::Point> &input_hand_contour)
{
	cv::Mat hand_corrected(input_hand_binary.size(), input_hand_binary.type(), cv::Scalar(125));
	input_hand_binary.copyTo(hand_corrected);

	// 使用椭圆拟合手形轮廓，得到旋转角度
	cv::RotatedRect rrect = cv::fitEllipse(cv::Mat(input_hand_contour));
	cv::ellipse(hand_corrected, rrect, cv::Scalar(125), 2);
	//cv::imshow("Rotation Corrected", hand_corrected);
	printf("\nangle:%f", rrect.angle);
	// 获取旋转矩阵
	if (rrect.angle > 90)	// 角度为长轴与垂直向上方向的夹角
	{
		rrect.angle = rrect.angle - 180;
	}
	else if (rrect.angle > 180)
	{
		//rrect.angle = rrect.angle + 180;
	}
	printf("\nchanged angle:%f", rrect.angle);
	cv::Mat rotmat = cv::getRotationMatrix2D(rrect.center, rrect.angle, 1);	// 获取旋转矩形

	// ----------- 使用仿射变换旋转图像------------------------
	cv::Mat img_rotated(input_hand_binary.size(), input_hand_binary.type(), cv::Scalar(125));
	cv::warpAffine(input_hand_binary, img_rotated, rotmat, input_hand_binary.size(), CV_INTER_LINEAR);	// 仿射变换（双线性差值）,差值之后变为非二值图
	cv::threshold(img_rotated, img_rotated, 0, 255, CV_THRESH_OTSU);	// 阈值化变为二值图
	medianBlur(img_rotated, img_rotated, 5);	// 平滑边缘
	//cv::imshow("Wrap Affine", img_rotated);
	hand_corrected = img_rotated;	// 获取旋转校正后的图像

	return hand_corrected;
}

void HandDetector::extractPalmCenter(const cv::Mat &input_hand_binary, cv::Point &output_palm_center, float &output_max_distance, cv::Mat &output_dist_transformed)
{
	output_dist_transformed.create(input_hand_binary.size(), CV_8UC1);

	cv::Mat disted(input_hand_binary.size(), CV_32FC1); //定义保存距离变换结果的Mat矩阵  
	cv::distanceTransform(input_hand_binary, disted, CV_DIST_L2, 3);  //距离变换（欧氏距离）  
	cv::Mat distShow = cv::Mat::zeros(input_hand_binary.size(), CV_8UC1); // 距离变换的显示图 
	float maxValue = 0.;  //定义距离变换矩阵中的最大值 
	for(int i = 0; i < disted.rows/* * 7 / 8*/; i++)  // 手掌不能太靠下
	{  
		for(int j = 0; j < disted.cols; j++)  
		{  
			distShow.at<uchar>(i,j) = static_cast<uchar>(disted.at<float>(i,j));  
			if(disted.at<float>(i,j) > maxValue && disted.at<float>(i,j) < 34)	// 距离不能太大，太大的话可能不是手掌  
			{  
				maxValue = disted.at<float>(i,j);  // 获取距离变换的极大值  
				output_palm_center = cv::Point(j, i);  // 掌心坐标  
			}  
		}  
	}  
	normalize(distShow, distShow, 0, 255, CV_MINMAX); //为了显示清晰，做了0~255归一化 
	//cv::imshow("Distance Transformed", distShow);
	output_dist_transformed = disted;	// 获取距离变换后的图
	output_max_distance = maxValue;	// 获取最大距离
	printf("\nmaxValue:%f", maxValue);

	cv::Mat palm;
	input_hand_binary.copyTo(palm);
	cvtColor(palm, palm, CV_GRAY2BGR);
	cv::circle(palm, output_palm_center, maxValue, cv::Scalar(0,0,255), 3);      
	cv::circle(palm, output_palm_center, 3, cv::Scalar(0,255,0), 3); 
	//cv::imshow("Palm", palm);
}

cv::Mat HandDetector::segmentPalm(const cv::Mat &input_hand_binary, const cv::Point &input_palm_center, const float &input_max_distance)
{
	cv::Mat palm(input_hand_binary.size(), input_hand_binary.type(), cv::Scalar(0));
	int split_row = input_palm_center.y + input_max_distance + 5;	// 分割行
	split_row = split_row < input_hand_binary.rows ? split_row : input_hand_binary.rows;
	for (int i = 0; i < split_row; i++)
	{
		if (cv::countNonZero(input_hand_binary.row(i)))	// 如果该行有非0元素
		{
			input_hand_binary.row(i).copyTo(palm.row(i));
		}
	}
	//cv::imshow("Pure Palm", palm);
	cv::Mat temp_palm;
	palm.copyTo(temp_palm);
	if (cv::countNonZero(temp_palm) <= 0)	// 没有非零像素点，即没有检测到手
	{
		printf("\nhand detect error!\n");
		//exit(-1);
		temp_palm = cv::imread("../data/1.png", 0);
	}

	// -----------分割手掌----------------
	std::vector<std::vector<cv::Point>> contours;	// 轮廓向量
	cv::findContours(temp_palm,
		contours,	// 轮廓的数组
		CV_RETR_EXTERNAL,	// 获取外轮廓，而参数CV_RETR_LIST可获取轮廓中洞组成的轮廓(内部轮廓)
		CV_CHAIN_APPROX_SIMPLE);	// 获取每个轮廓的末端像素（区域外轮廓简单拟合）

	// 裁剪手掌
	cv::Rect palm_rect = cv::boundingRect(contours[0]);	// 最小包围矩形
	//cv::rectangle(palm, r0, cv::Scalar(125), 1);
	//cv::imshow("Pure Palm Rect", palm);
	printf("\n%d,%d,%d,%d", palm_rect.x, palm_rect.y, palm_rect.size().width, palm_rect.size().height);
	cv::Mat palm_roi = palm(palm_rect);	// 截取手掌感兴趣区域
	//cv::imshow("Palm ROI", palm_roi);

	// 大小归一化为130*140（W*H）(cols*rows)
	cv::resize(palm_roi, palm_roi, cv::Size(130, 140));
	printf("\n%d,%d", palm_roi.size());
	//cv::imshow("Resize Palm ROI", palm_roi);

	return palm_roi;
}

bool HandDetector::isBimodal(const std::vector<double> histogram, int size_h)
{
	// 对直方图的峰进行计数，只有峰数位2才为双峰 
	int count = 0;
	for (int y = 1; y < size_h - 1; y++)
	{
		if ((histogram[y - 1] < histogram[y] && histogram[y + 1] < histogram[y]))	// 三邻域内峰值最大
		{
			count++;
			if (count > 2) return false;
		}
	}
	if (count == 2)
		return true;
	else
		return false;
}

int HandDetector::getMinimumThreshold(const std::vector<double> histogram, int size_h)
{
	int y, iter = 0;
	std::vector<double> histogram_copy(size_h);           // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
	std::vector<double> histogram_copy_copy(size_h);          // 求均值的过程会破坏前面的数据，因此需要两份数据
	for (y = 0; y < size_h; y++)
	{
		histogram_copy[y] = histogram[y];
		histogram_copy_copy[y] = histogram[y];
	}

	// 通过三点求均值来平滑直方图
	while (isBimodal(histogram_copy_copy) == false)                                        // 判断是否已经是双峰的图像了      
	{
		histogram_copy_copy[0] = (histogram_copy[0] + histogram_copy[0] + histogram_copy[1]) / 3;                 // 第一点
		for (y = 1; y < size_h - 1; y++)
			histogram_copy_copy[y] = (histogram_copy[y - 1] + histogram_copy[y] + histogram_copy[y + 1]) / 3;     // 中间的点
		histogram_copy_copy[size_h-1] = (histogram_copy[size_h-2] + histogram_copy[size_h-1] + histogram_copy[size_h-1]) / 3;         // 最后一点
		histogram_copy_copy.swap(histogram_copy);	// 将histogram_copy_copy复制给histogram_copy
		iter++;
		if (iter >= 1000) return -1;                                                   // 直方图无法平滑为双峰的，返回错误代码
	}
	// 阈值极为两峰之间的最小值 
	bool peak_found = false;
	for (y = 1; y < size_h-1; y++)
	{
		if (histogram_copy_copy[y - 1] < histogram_copy_copy[y] && histogram_copy_copy[y + 1] < histogram_copy_copy[y]) peak_found = true;
		if (peak_found == true && histogram_copy_copy[y - 1] >= histogram_copy_copy[y] && histogram_copy_copy[y + 1] >= histogram_copy_copy[y])
			return y;
	}
	return -1;
}

cv::Mat HandDetector::segmentPalm(const cv::Mat &input_hand_binary, const cv::Mat input_dist_transformed)
{
	// ------------- 获取每行距离最大值 --------------------------------------------
	std::vector<double> dist_row_max_value(input_dist_transformed.rows);	// 每行的距离最大值
	for (int j = 0; j < input_dist_transformed.rows; j++)
	{
		int max_pixel_value = 0;
		const float *data_in = input_dist_transformed.ptr<float>(j);	// 得到输入图像第j行的首地址
		if (cv::countNonZero(input_dist_transformed.row(j)))	// 如果该行有非0元素
		{
			for (int i = 0; i < input_dist_transformed.cols; i++)	// 统计第j行的最大值
			{
				if (data_in[i] > max_pixel_value)
				{
					max_pixel_value = data_in[i];
				}
			}
			dist_row_max_value[j] = max_pixel_value;
		}
		else
		{
			dist_row_max_value[j] = 0;
		}
	}

	// 显示直方图的图像
	//cv::Mat histImage(dist_row_max_value.size(), dist_row_max_value.size(), CV_8UC1, cv::Scalar(255));
	//// 设置最高点为nbins的90%
	//int hpt = static_cast<int>(0.9 * dist_row_max_value.size());
	//// 每个条目都绘制一条直线
	//for (int h = 0; h < dist_row_max_value.size(); h++)
	//{
	//	float binVal = dist_row_max_value[h];	// 取第h+1条的值
	//	//std::cout << binVal << "\t";
	//	int intensity = static_cast<int>(binVal * hpt / dist_row_max_value.size());
	//	// 两点之间绘制一条线
	//	cv::line(histImage, cv::Point(h, dist_row_max_value.size()), cv::Point(h, dist_row_max_value.size() - intensity), cv::Scalar::all(0));
	//}

	// 获取手腕所在行，即峰谷。分割行
	int split_row = getMinimumThreshold(dist_row_max_value, input_dist_transformed.rows);
	split_row = split_row > 0 ? split_row : 0; 

	// 分割手掌（手腕以下的像素全置为0）
	cv::Mat palm;
	input_hand_binary.copyTo(palm);
	cv::Mat zero_mat = cv::Mat::zeros(1, input_dist_transformed.cols, input_dist_transformed.type());
	for (int i = split_row; i < input_dist_transformed.rows; i++)
	{
		if (cv::countNonZero(palm.row(i)))	// 如果该行有非0元素
		{
			zero_mat.copyTo(palm.row(i));	// 按行置为0
		}
	}
	cv::imshow("Palm", palm);
	cv::Mat temp_palm;
	palm.copyTo(temp_palm);
	if (cv::countNonZero(temp_palm) <= 0)	// 没有非零像素点，即没有检测到手
	{
		printf("\nhand detect error!\n");
		exit(-1);
	}

	// -----------分割手掌----------------
	std::vector<std::vector<cv::Point>> contours;	// 轮廓向量
	cv::findContours(temp_palm,
		contours,	// 轮廓的数组
		CV_RETR_EXTERNAL,	// 获取外轮廓，而参数CV_RETR_LIST可获取轮廓中洞组成的轮廓(内部轮廓)
		CV_CHAIN_APPROX_SIMPLE);	// 获取每个轮廓的末端像素（区域外轮廓简单拟合）

	// 裁剪手掌
	cv::Rect palm_rect = cv::boundingRect(contours[0]);	// 最小包围矩形
	//cv::rectangle(palm, r0, cv::Scalar(125), 1);
	//cv::imshow("Pure Palm Rect", palm);
	printf("\n%d,%d,%d,%d", palm_rect.x, palm_rect.y, palm_rect.size().width, palm_rect.size().height);
	cv::Mat palm_roi = palm(palm_rect);	// 截取手掌感兴趣区域
	//cv::imshow("Palm ROI", palm_roi);

	// 大小归一化为130*140（W*H）(cols*rows)
	cv::resize(palm_roi, palm_roi, cv::Size(130, 140));
	printf("\n%d,%d", palm_roi.size());
	//cv::imshow("Resize Palm ROI", palm_roi);

	return palm_roi;
}

cv::Mat HandDetector::detectHand(const cv::Mat &input_src_gray)
{
	cv::Mat hand_binary;	// 可能含干扰块的手部二值图
	preProcess(input_src_gray, hand_binary);
	std::vector<cv::Point> hand_contour;	// 手部轮廓
	hand_binary = extractHandContour(hand_binary, hand_contour);	// 不含干扰块的手部二值图
	hand_binary = rotationCorrect(hand_binary, hand_contour);	// 旋转校正后的手部二值图

	cv::Point palm_center;	// 掌心坐标
	cv::Mat dist_transformed;	// 距离变换后的图
	float max_distance;	// 距离变化中最大距离
	extractPalmCenter(hand_binary, palm_center, max_distance, dist_transformed);
	return segmentPalm(hand_binary, palm_center, max_distance);	//segmentPalm(hand_binary, dist_transformed);
}
