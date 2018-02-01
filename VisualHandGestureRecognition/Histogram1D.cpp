#include "Histogram1D.h"
#include <iostream>

Histogram1D::Histogram1D(void)
{
	// ��ʼ��һά��1D��ֱ��ͼ�Ĳ���
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0;	// Ĭ�����������0��ͨ��
}


Histogram1D::~Histogram1D(void)
{
}

cv::MatND Histogram1D::getHistogram(const cv::Mat &image)
{
	cv::MatND hist;

	// ����ֱ��ͼ
	cv::calcHist(&image,
		1,	// ���㵥��ͼ���ֱ��ͼ
		channels,	// ͨ������
		cv::Mat(),	// ��ʹ��ͼ����Ϊ����
		hist,	// ���ص�ֱ��ͼ
		1,	// ����һά��1D����ֱ��ͼ
		histSize,	// �������
		ranges	// ����ֵ�ķ�Χ
		);

	return hist;
}

cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image)
{
	// ���ȼ���ֱ��ͼ
	cv::MatND hist = getHistogram(image);

	// ��ȡ���ֵ����Сֵ
	double maxVal = 0.0;
	double minVal = 0.0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);

	// ��ʾֱ��ͼ��ͼ��
	cv::Mat histImage(histSize[0], histSize[0], CV_8U, cv::Scalar(255));
	// ������ߵ�Ϊnbins��90%
	int hpt = static_cast<int>(0.9 * histSize[0]);
	// ÿ����Ŀ������һ��ֱ��
	for (int h = 0; h < histSize[0]; h++)
	{
		float binVal = hist.at<float>(h);	// ȡ��h+1����ֵ
		std::cout << binVal << "\t";
		int intensity = static_cast<int>(binVal * hpt / maxVal);
		// ����֮�����һ����
		cv::line(histImage, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));
	}

	return histImage;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat &image, const cv::Mat &lookup)
{
	cv::Mat result;	// ���ͼ��
	cv::LUT(image, lookup, result);	// Ӧ�ò��ұ�

	return result;
}

cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue)
{
	// ���ȼ���ֱ��ͼ
	cv::MatND hist = getHistogram(image);

	// Ѱ��ֱ��ͼ�����
	int imin = 0;
	for (; imin < histSize[0]; imin++)
	{
		std::cout << hist.at<float>(imin) << std::endl;
		if (hist.at<float>(imin) > minValue)
		{
			break;
		}
	}

	// Ѱ��ֱ��ͼ���Ҷ�
	int imax = histSize[0] - 1;
	for (; imax >= 0; imax--)
	{
		std::cout << hist.at<float>(imax) << std::endl;
		if (hist.at<float>(imax) > minValue)
		{
			break;
		}
	}

	// �������ұ�
	int dim(256);
	cv::Mat lookup(1,	// 1 dimension
		&dim,	// 256 entries
		CV_8U	// uchar
		);
	// �����ұ�
	for (int i = 0; i < 256; i++)
	{
		// ȷ����ֵλ��imin��imax֮��
		if (i < imin) lookup.at<uchar>(i) = 0;
		else if (i > imax) lookup.at<uchar>(i) = 255;
		// ����ӳ��
		else lookup.at<uchar>(i) = static_cast<uchar>(255.0 * (i-imin) / (imax-imin));
	}

	// Ӧ�ò��ұ�
	cv::Mat result = applyLookUp(image, lookup);

	return result;
}

cv::Mat Histogram1D::equalize(const cv::Mat &image)
{
	cv::Mat result;
	cv::equalizeHist(image, result);	// ֱ��ͼ���⻯��ʹֱ��ͼ�ֲ�������
	return result;
}
