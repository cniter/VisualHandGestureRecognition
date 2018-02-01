#include "SVMTrain.h"


SVMTrain::SVMTrain(void)
{
}


SVMTrain::~SVMTrain(void)
{
}

SVMTrain::SVMTrain(const char *hand_gesture_folder, const char *xml) : hand_gesture_folder_(hand_gesture_folder), svm_xml_(xml)
{
	assert(hand_gesture_folder);
	assert(xml);
}

void SVMTrain::getHOGFeatures(const cv::Mat &image, cv::Mat& features)
{
	cv::HOGDescriptor hog;
	hog.winSize = cvSize(image.cols, image.rows);// Size();// 窗口大小，整个图像的大小(130*140)
	hog.blockSize = cv::Size(30, 40);	// 块大小
	hog.blockStride = cv::Size(25, 25);	// 块滑动增量（1/4个block大小）
	hog.cellSize = cv::Size(15, 20);	// 胞元大小

	//hog.winSize = cvSize(image.cols, image.rows);// Size();// 窗口大小，整个图像的大小(130*140)
	//hog.blockSize = cv::Size(30, 40);	// 块大小
	//hog.blockStride = cv::Size(10, 10);	// 块滑动增量
	//hog.cellSize = cv::Size(15, 20);	// 胞元大小

	fprintf(stdout, ">> HOG Dimension: %d\n", hog.getDescriptorSize());	// 获取该参数下的维度

	std::vector<float> descriptors;

	cv::Mat gray;
	image.copyTo(gray);
	//cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	hog.compute(gray, descriptors, cv::Size(2, 2), cv::Size(0, 0));

	//hog计算得到的特征cols=1,rows=维数
	cv::Mat tmp(1, cv::Mat(descriptors).rows, CV_32FC1); //< used for transposition if needed
	//fprintf(stdout, ">> cv::Mat(descriptors).cols: %d\n", cv::Mat(descriptors).cols);

	transpose(cv::Mat(descriptors), tmp);	// 转置变换，将垂直的图像变为水平，即行变为对应列，列变为相应行
	//fprintf(stdout, ">> tmp.cols: %d\n", tmp.cols);
	tmp.copyTo(features);
}

void SVMTrain::get_train_data()
{
	std::cout << "Collecting train data..." << std::endl;

	std::vector<int> labels;
	std::vector<string> files;
	Utilities util;
	util.getAllFormatFiles(hand_gesture_folder_, "*", files);
	if (files.empty()) 
	{
		std::cout << "No file found in " << hand_gesture_folder_ << std::endl;
		exit(-1);
	}

	std::cout << "Collecting train data in " << hand_gesture_folder_ << std::endl;
	cv::Mat samples;	// 训练数据
	std::vector<int> responses;	// 标签，即类别
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image = cv::imread(files[i], 0);
		//std::cout << "FilePath： " << files[i] << std::endl;
		cv::Mat feature;
		getHOGFeatures(image, feature);
		samples.push_back(feature);
		responses.push_back(atoi(files[i].substr(files[i].find_last_of('/', files[i].find_last_of('/') - 1) + 2, files[i].find_last_of('/') - files[i].find_last_of('/', files[i].find_last_of('/') - 1) - 2).c_str()));	// 获取标签
		//printf(">> Label: %d\n", responses[i]);
	}
	//std::cout << "FilePath： " << files.size() << std::endl;
	samples.copyTo(this->training_data_);
	cv::Mat(responses).copyTo(this->classes_);

	std::cout << "Collecting train data done." << std::endl;
}

void SVMTrain::trainAuto()
{
	// need to be trained first
	CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	//SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;	// 线型，也就是无核
	SVM_params.kernel_type = CvSVM::RBF;  // CvSVM::RBF 径向基函数，也就是高斯核
	SVM_params.degree = 0.1;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 0.1;
	SVM_params.C = 1;
	SVM_params.nu = 0.1;
	SVM_params.p = 0.1;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 20000, 0.0001);

	std::cout << "Generating svm model file, please wait..." << std::endl;

	try 
	{
		svm_.train_auto(this->training_data_, this->classes_, cv::Mat(),
			cv::Mat(), SVM_params, 10,
			CvSVM::get_default_grid(CvSVM::C),
			CvSVM::get_default_grid(CvSVM::GAMMA),
			CvSVM::get_default_grid(CvSVM::P),
			CvSVM::get_default_grid(CvSVM::NU),
			CvSVM::get_default_grid(CvSVM::COEF),
			CvSVM::get_default_grid(CvSVM::DEGREE), false);
			fprintf(stdout, ">> Saving model file...\n");
	} catch (const cv::Exception& err) 
	{
		std::cout << err.what() << std::endl;
	}

	svm_.save(svm_xml_);
	fprintf(stdout, ">> Your SVM Model was saved to %s\n", svm_xml_);

	CvSVMParams params_re = svm_.get_params();
	float C = params_re.C;
	float gamma = params_re.gamma;
	printf("\n>> Optimal Params: C = %f, gamma = %f \n",C,gamma);

	test();
}

void SVMTrain::loadModel()
{
	svm_.clear();	// 使用load函数之前需要先clear
	svm_.load(svm_xml_); 
}

std::string SVMTrain::predictResult(const cv::Mat &image)
{
	cv::Mat feature;
	getHOGFeatures(image, feature);
	std::cout << "Hand Gesture Predict：G" << svm_.predict(feature) << std::endl;
	char gesture[32];
	sprintf(gesture, "G%d", (int)svm_.predict(feature));	// float转int再转char[]
	return (std::string)gesture;
}

void SVMTrain::test(const cv::Mat &image)
{
	svm_.clear();	// 使用load函数之前需要先clear
	svm_.load(svm_xml_); 

	cv::Mat feature;
	getHOGFeatures(image, feature);
	std::cout << "Hand Gesture Predict：G" << svm_.predict(feature) << std::endl;
	char gesture[32];
	sprintf(gesture, "G%d", (int)svm_.predict(feature));	// float转int再转char[]
}

void SVMTrain::test()
{
	Utilities util;
	std::vector<string> files;
	util.getAllFormatFiles("./data/test", "*", files);

	std::cout << "Testing..." << std::endl;

	double count_all = files.size();
	double ptrue_rtrue[10] = {0};	// 预测手势和实际手势都为G的数量
	double ptrue_rfalse[10] = {0};	// 预测手势为G而实际不为G的数量
	double pfalse_rtrue[10] = {0};	// 实际手势为G而预测不为G的数量
	double pfalse_rfalse[10] = {0};	// 预测手势不是G实际也不是G的数量
	//int gesture = 1;	// 计算该手势的准确率和查全率

	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image = cv::imread(files[i], 0);
		//std::cout << "FilePath： " << files[i] << std::endl;
		cv::Mat feature;
		getHOGFeatures(image, feature);
		int predict = svm_.predict(feature);
		int real = atoi(files[i].substr(files[i].find_last_of('/', files[i].find_last_of('/') - 1) + 2, files[i].find_last_of('/') - files[i].find_last_of('/', files[i].find_last_of('/') - 1) - 2).c_str());	// 获取标签
		//std::cout << "Hand Gesture Predict：" << svm_.predict(feature) << "\t";
		for (int gesture = 0; gesture < 10; gesture++)
		{
			if (predict == gesture && real == gesture) ptrue_rtrue[gesture]++;
			if (predict == gesture && real != gesture) ptrue_rfalse[gesture]++;
			if (predict != gesture && real == gesture) pfalse_rtrue[gesture]++;
			if (predict != gesture && real != gesture) pfalse_rfalse[gesture]++;
		}
	}

	std::cout << "count_all: " << count_all << std::endl;
	for (int gesture = 0; gesture < 10; gesture++)
	{
		std::cout << "G" << gesture << " ptrue_rtrue: " << ptrue_rtrue[gesture] << std::endl;
		std::cout << "G" << gesture << " ptrue_rfalse: " << ptrue_rfalse[gesture] << std::endl;
		std::cout << "G" << gesture << " pfalse_rtrue: " << pfalse_rtrue[gesture] << std::endl;
		std::cout << "G" << gesture << " pfalse_rfalse: " << pfalse_rfalse[gesture] << std::endl;
	}


	for (int gesture = 0; gesture < 10; gesture++)
	{
		double precise = 0;	// 准确率
		if (ptrue_rtrue[gesture] + ptrue_rfalse[gesture] != 0) 
		{
			precise = ptrue_rtrue[gesture] / (ptrue_rtrue[gesture] + ptrue_rfalse[gesture]);
			std::cout << "G" << gesture << "precise: " << precise << std::endl;
		} else 
		{
			std::cout << "G" << gesture << "precise: " << "NA" << std::endl;
		}

		double recall = 0;	// 查全率
		if (ptrue_rtrue[gesture] + pfalse_rtrue[gesture] != 0) 
		{
			recall = ptrue_rtrue[gesture] / (ptrue_rtrue[gesture] + pfalse_rtrue[gesture]);
			std::cout << "G" << gesture << "recall: " << recall << std::endl;
		} else 
		{
			std::cout << "G" << gesture << "recall: " << "NA" << std::endl;
		}

		double Fsocre = 0;	// 得分
		if (precise + recall != 0) 
		{
			Fsocre = 2 * (precise * recall) / (precise + recall);
			std::cout << "G" << gesture << "Fsocre: " << Fsocre << std::endl;
		} else 
		{
			std::cout << "G" << gesture << "Fsocre: " << "NA" << std::endl;
		}
	}
}
