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

	cv::Mat stretched = hist.stretch(input_src_gray);	// ֱ��ͼ����
	//cv::imshow("Hist Stretched", stretched);
	//cv::imshow("Hist Image", hist.getHistogramImage(stretched));
	// ��ȡֱ��ͼ
	cv::MatND histogram = hist.getHistogram(stretched);

	// ������ֵΪ1��ʼѰ��ֱ��ͼ��ߵ�һ�������������ֵ
	int bin = 0;	// ��¼�ָ��յ�����ֵ
	for (int i = 1; i < 256; i++)
	{
		printf("%f\t", histogram.at<float>(i));
		if (histogram.at<float>(i) > 300)	// �ֵ�����������300��
		{
			//continue;
			bin = i;
			break;
		}
	}
	printf("\n%d\n", bin);
	cv::inRange(stretched, cv::Scalar(bin - 5), cv::Scalar(bin + 4), out_hand_binary);	// bin-5~bin+4 || 0.97*bin~1.03*bin ֮�������ֵΪ255������Ϊ0
	//cv::imshow("Hand Bianry Without MedianBlur", out_hand_binary);
}

cv::Mat HandDetector::extractHandContour(const cv::Mat &input_hand_binary, std::vector<cv::Point> &output_hand_contour)
{
	std::vector<std::vector<cv::Point>> contours;	// ��������
	cv::findContours(input_hand_binary,
		contours,	// ����������
		CV_RETR_EXTERNAL,	// ��ȡ��������������CV_RETR_LIST�ɻ�ȡ�����ж���ɵ�����(�ڲ�����)
		CV_CHAIN_APPROX_SIMPLE);	// ��ȡÿ��������ĩ�����أ���������������ϣ�

	// �ҵ�������������Ϊ�ֲ�����  
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

	output_hand_contour = contours[index];	// ��ȡ�ֲ�����

	// �ں�ɫͼ���ϻ�������
	cv::Mat hand_binary(input_hand_binary.size(), CV_8UC1, cv::Scalar(0));
	cv::drawContours(hand_binary, contours,
		index,	// ֻ�����ֲ�����
		cv::Scalar(255),	// ��ɫΪ��ɫ
		CV_FILLED);	// ��ȫ���
	//cv::imshow("Hand Binary Without Noise", hand_binary);

	// �����㣺�����ͣ���ʴ������Ϊ����ɫǰ�������еĺ�ɫС��
	//cv::Mat element(9, 9, CV_8U, cv::Scalar(1));	// ʹ��9x9�ĽṹԪ��
	//cv::morphologyEx(hand_binary, hand_binary, cv::MORPH_CLOSE, element);	// ������
	//cv::imshow("close", hand_binary);

	cv::medianBlur(hand_binary, hand_binary, 9);	// Ӧ����ֵ�˲�
	//cv::imshow("Smooth Hand Binary Without Noise", hand_binary);

	return hand_binary;
}

cv::Mat HandDetector::rotationCorrect(const cv::Mat &input_hand_binary, const std::vector<cv::Point> &input_hand_contour)
{
	cv::Mat hand_corrected(input_hand_binary.size(), input_hand_binary.type(), cv::Scalar(125));
	input_hand_binary.copyTo(hand_corrected);

	// ʹ����Բ��������������õ���ת�Ƕ�
	cv::RotatedRect rrect = cv::fitEllipse(cv::Mat(input_hand_contour));
	cv::ellipse(hand_corrected, rrect, cv::Scalar(125), 2);
	//cv::imshow("Rotation Corrected", hand_corrected);
	printf("\nangle:%f", rrect.angle);
	// ��ȡ��ת����
	if (rrect.angle > 90)	// �Ƕ�Ϊ�����봹ֱ���Ϸ���ļн�
	{
		rrect.angle = rrect.angle - 180;
	}
	else if (rrect.angle > 180)
	{
		//rrect.angle = rrect.angle + 180;
	}
	printf("\nchanged angle:%f", rrect.angle);
	cv::Mat rotmat = cv::getRotationMatrix2D(rrect.center, rrect.angle, 1);	// ��ȡ��ת����

	// ----------- ʹ�÷���任��תͼ��------------------------
	cv::Mat img_rotated(input_hand_binary.size(), input_hand_binary.type(), cv::Scalar(125));
	cv::warpAffine(input_hand_binary, img_rotated, rotmat, input_hand_binary.size(), CV_INTER_LINEAR);	// ����任��˫���Բ�ֵ��,��ֵ֮���Ϊ�Ƕ�ֵͼ
	cv::threshold(img_rotated, img_rotated, 0, 255, CV_THRESH_OTSU);	// ��ֵ����Ϊ��ֵͼ
	medianBlur(img_rotated, img_rotated, 5);	// ƽ����Ե
	//cv::imshow("Wrap Affine", img_rotated);
	hand_corrected = img_rotated;	// ��ȡ��תУ�����ͼ��

	return hand_corrected;
}

void HandDetector::extractPalmCenter(const cv::Mat &input_hand_binary, cv::Point &output_palm_center, float &output_max_distance, cv::Mat &output_dist_transformed)
{
	output_dist_transformed.create(input_hand_binary.size(), CV_8UC1);

	cv::Mat disted(input_hand_binary.size(), CV_32FC1); //���屣�����任�����Mat����  
	cv::distanceTransform(input_hand_binary, disted, CV_DIST_L2, 3);  //����任��ŷ�Ͼ��룩  
	cv::Mat distShow = cv::Mat::zeros(input_hand_binary.size(), CV_8UC1); // ����任����ʾͼ 
	float maxValue = 0.;  //�������任�����е����ֵ 
	for(int i = 0; i < disted.rows/* * 7 / 8*/; i++)  // ���Ʋ���̫����
	{  
		for(int j = 0; j < disted.cols; j++)  
		{  
			distShow.at<uchar>(i,j) = static_cast<uchar>(disted.at<float>(i,j));  
			if(disted.at<float>(i,j) > maxValue && disted.at<float>(i,j) < 34)	// ���벻��̫��̫��Ļ����ܲ�������  
			{  
				maxValue = disted.at<float>(i,j);  // ��ȡ����任�ļ���ֵ  
				output_palm_center = cv::Point(j, i);  // ��������  
			}  
		}  
	}  
	normalize(distShow, distShow, 0, 255, CV_MINMAX); //Ϊ����ʾ����������0~255��һ�� 
	//cv::imshow("Distance Transformed", distShow);
	output_dist_transformed = disted;	// ��ȡ����任���ͼ
	output_max_distance = maxValue;	// ��ȡ������
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
	int split_row = input_palm_center.y + input_max_distance + 5;	// �ָ���
	split_row = split_row < input_hand_binary.rows ? split_row : input_hand_binary.rows;
	for (int i = 0; i < split_row; i++)
	{
		if (cv::countNonZero(input_hand_binary.row(i)))	// ��������з�0Ԫ��
		{
			input_hand_binary.row(i).copyTo(palm.row(i));
		}
	}
	//cv::imshow("Pure Palm", palm);
	cv::Mat temp_palm;
	palm.copyTo(temp_palm);
	if (cv::countNonZero(temp_palm) <= 0)	// û�з������ص㣬��û�м�⵽��
	{
		printf("\nhand detect error!\n");
		//exit(-1);
		temp_palm = cv::imread("../data/1.png", 0);
	}

	// -----------�ָ�����----------------
	std::vector<std::vector<cv::Point>> contours;	// ��������
	cv::findContours(temp_palm,
		contours,	// ����������
		CV_RETR_EXTERNAL,	// ��ȡ��������������CV_RETR_LIST�ɻ�ȡ�����ж���ɵ�����(�ڲ�����)
		CV_CHAIN_APPROX_SIMPLE);	// ��ȡÿ��������ĩ�����أ���������������ϣ�

	// �ü�����
	cv::Rect palm_rect = cv::boundingRect(contours[0]);	// ��С��Χ����
	//cv::rectangle(palm, r0, cv::Scalar(125), 1);
	//cv::imshow("Pure Palm Rect", palm);
	printf("\n%d,%d,%d,%d", palm_rect.x, palm_rect.y, palm_rect.size().width, palm_rect.size().height);
	cv::Mat palm_roi = palm(palm_rect);	// ��ȡ���Ƹ���Ȥ����
	//cv::imshow("Palm ROI", palm_roi);

	// ��С��һ��Ϊ130*140��W*H��(cols*rows)
	cv::resize(palm_roi, palm_roi, cv::Size(130, 140));
	printf("\n%d,%d", palm_roi.size());
	//cv::imshow("Resize Palm ROI", palm_roi);

	return palm_roi;
}

bool HandDetector::isBimodal(const std::vector<double> histogram, int size_h)
{
	// ��ֱ��ͼ�ķ���м�����ֻ�з���λ2��Ϊ˫�� 
	int count = 0;
	for (int y = 1; y < size_h - 1; y++)
	{
		if ((histogram[y - 1] < histogram[y] && histogram[y + 1] < histogram[y]))	// �������ڷ�ֵ���
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
	std::vector<double> histogram_copy(size_h);           // ���ھ������⣬һ��Ҫ�ø���������������ò�����ȷ�Ľ��
	std::vector<double> histogram_copy_copy(size_h);          // ���ֵ�Ĺ��̻��ƻ�ǰ������ݣ������Ҫ��������
	for (y = 0; y < size_h; y++)
	{
		histogram_copy[y] = histogram[y];
		histogram_copy_copy[y] = histogram[y];
	}

	// ͨ���������ֵ��ƽ��ֱ��ͼ
	while (isBimodal(histogram_copy_copy) == false)                                        // �ж��Ƿ��Ѿ���˫���ͼ����      
	{
		histogram_copy_copy[0] = (histogram_copy[0] + histogram_copy[0] + histogram_copy[1]) / 3;                 // ��һ��
		for (y = 1; y < size_h - 1; y++)
			histogram_copy_copy[y] = (histogram_copy[y - 1] + histogram_copy[y] + histogram_copy[y + 1]) / 3;     // �м�ĵ�
		histogram_copy_copy[size_h-1] = (histogram_copy[size_h-2] + histogram_copy[size_h-1] + histogram_copy[size_h-1]) / 3;         // ���һ��
		histogram_copy_copy.swap(histogram_copy);	// ��histogram_copy_copy���Ƹ�histogram_copy
		iter++;
		if (iter >= 1000) return -1;                                                   // ֱ��ͼ�޷�ƽ��Ϊ˫��ģ����ش������
	}
	// ��ֵ��Ϊ����֮�����Сֵ 
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
	// ------------- ��ȡÿ�о������ֵ --------------------------------------------
	std::vector<double> dist_row_max_value(input_dist_transformed.rows);	// ÿ�еľ������ֵ
	for (int j = 0; j < input_dist_transformed.rows; j++)
	{
		int max_pixel_value = 0;
		const float *data_in = input_dist_transformed.ptr<float>(j);	// �õ�����ͼ���j�е��׵�ַ
		if (cv::countNonZero(input_dist_transformed.row(j)))	// ��������з�0Ԫ��
		{
			for (int i = 0; i < input_dist_transformed.cols; i++)	// ͳ�Ƶ�j�е����ֵ
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

	// ��ʾֱ��ͼ��ͼ��
	//cv::Mat histImage(dist_row_max_value.size(), dist_row_max_value.size(), CV_8UC1, cv::Scalar(255));
	//// ������ߵ�Ϊnbins��90%
	//int hpt = static_cast<int>(0.9 * dist_row_max_value.size());
	//// ÿ����Ŀ������һ��ֱ��
	//for (int h = 0; h < dist_row_max_value.size(); h++)
	//{
	//	float binVal = dist_row_max_value[h];	// ȡ��h+1����ֵ
	//	//std::cout << binVal << "\t";
	//	int intensity = static_cast<int>(binVal * hpt / dist_row_max_value.size());
	//	// ����֮�����һ����
	//	cv::line(histImage, cv::Point(h, dist_row_max_value.size()), cv::Point(h, dist_row_max_value.size() - intensity), cv::Scalar::all(0));
	//}

	// ��ȡ���������У�����ȡ��ָ���
	int split_row = getMinimumThreshold(dist_row_max_value, input_dist_transformed.rows);
	split_row = split_row > 0 ? split_row : 0; 

	// �ָ����ƣ��������µ�����ȫ��Ϊ0��
	cv::Mat palm;
	input_hand_binary.copyTo(palm);
	cv::Mat zero_mat = cv::Mat::zeros(1, input_dist_transformed.cols, input_dist_transformed.type());
	for (int i = split_row; i < input_dist_transformed.rows; i++)
	{
		if (cv::countNonZero(palm.row(i)))	// ��������з�0Ԫ��
		{
			zero_mat.copyTo(palm.row(i));	// ������Ϊ0
		}
	}
	cv::imshow("Palm", palm);
	cv::Mat temp_palm;
	palm.copyTo(temp_palm);
	if (cv::countNonZero(temp_palm) <= 0)	// û�з������ص㣬��û�м�⵽��
	{
		printf("\nhand detect error!\n");
		exit(-1);
	}

	// -----------�ָ�����----------------
	std::vector<std::vector<cv::Point>> contours;	// ��������
	cv::findContours(temp_palm,
		contours,	// ����������
		CV_RETR_EXTERNAL,	// ��ȡ��������������CV_RETR_LIST�ɻ�ȡ�����ж���ɵ�����(�ڲ�����)
		CV_CHAIN_APPROX_SIMPLE);	// ��ȡÿ��������ĩ�����أ���������������ϣ�

	// �ü�����
	cv::Rect palm_rect = cv::boundingRect(contours[0]);	// ��С��Χ����
	//cv::rectangle(palm, r0, cv::Scalar(125), 1);
	//cv::imshow("Pure Palm Rect", palm);
	printf("\n%d,%d,%d,%d", palm_rect.x, palm_rect.y, palm_rect.size().width, palm_rect.size().height);
	cv::Mat palm_roi = palm(palm_rect);	// ��ȡ���Ƹ���Ȥ����
	//cv::imshow("Palm ROI", palm_roi);

	// ��С��һ��Ϊ130*140��W*H��(cols*rows)
	cv::resize(palm_roi, palm_roi, cv::Size(130, 140));
	printf("\n%d,%d", palm_roi.size());
	//cv::imshow("Resize Palm ROI", palm_roi);

	return palm_roi;
}

cv::Mat HandDetector::detectHand(const cv::Mat &input_src_gray)
{
	cv::Mat hand_binary;	// ���ܺ����ſ���ֲ���ֵͼ
	preProcess(input_src_gray, hand_binary);
	std::vector<cv::Point> hand_contour;	// �ֲ�����
	hand_binary = extractHandContour(hand_binary, hand_contour);	// �������ſ���ֲ���ֵͼ
	hand_binary = rotationCorrect(hand_binary, hand_contour);	// ��תУ������ֲ���ֵͼ

	cv::Point palm_center;	// ��������
	cv::Mat dist_transformed;	// ����任���ͼ
	float max_distance;	// ����仯��������
	extractPalmCenter(hand_binary, palm_center, max_distance, dist_transformed);
	return segmentPalm(hand_binary, palm_center, max_distance);	//segmentPalm(hand_binary, dist_transformed);
}
