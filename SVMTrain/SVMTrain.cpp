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
	hog.winSize = cvSize(image.cols, image.rows);// Size();// ���ڴ�С������ͼ��Ĵ�С(130*140)
	hog.blockSize = cv::Size(30, 40);	// ���С
	hog.blockStride = cv::Size(25, 25);	// �黬��������1/4��block��С��
	hog.cellSize = cv::Size(15, 20);	// ��Ԫ��С

	//hog.winSize = cvSize(image.cols, image.rows);// Size();// ���ڴ�С������ͼ��Ĵ�С(130*140)
	//hog.blockSize = cv::Size(30, 40);	// ���С
	//hog.blockStride = cv::Size(10, 10);	// �黬������
	//hog.cellSize = cv::Size(15, 20);	// ��Ԫ��С

	fprintf(stdout, ">> HOG Dimension: %d\n", hog.getDescriptorSize());	// ��ȡ�ò����µ�ά��

	std::vector<float> descriptors;

	cv::Mat gray;
	image.copyTo(gray);
	//cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	hog.compute(gray, descriptors, cv::Size(2, 2), cv::Size(0, 0));

	//hog����õ�������cols=1,rows=ά��
	cv::Mat tmp(1, cv::Mat(descriptors).rows, CV_32FC1); //< used for transposition if needed
	//fprintf(stdout, ">> cv::Mat(descriptors).cols: %d\n", cv::Mat(descriptors).cols);

	transpose(cv::Mat(descriptors), tmp);	// ת�ñ任������ֱ��ͼ���Ϊˮƽ�����б�Ϊ��Ӧ�У��б�Ϊ��Ӧ��
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
	cv::Mat samples;	// ѵ������
	std::vector<int> responses;	// ��ǩ�������
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image = cv::imread(files[i], 0);
		//std::cout << "FilePath�� " << files[i] << std::endl;
		cv::Mat feature;
		getHOGFeatures(image, feature);
		samples.push_back(feature);
		responses.push_back(atoi(files[i].substr(files[i].find_last_of('/', files[i].find_last_of('/') - 1) + 2, files[i].find_last_of('/') - files[i].find_last_of('/', files[i].find_last_of('/') - 1) - 2).c_str()));	// ��ȡ��ǩ
		//printf(">> Label: %d\n", responses[i]);
	}
	//std::cout << "FilePath�� " << files.size() << std::endl;
	samples.copyTo(this->training_data_);
	cv::Mat(responses).copyTo(this->classes_);

	std::cout << "Collecting train data done." << std::endl;
}

void SVMTrain::trainAuto()
{
	// need to be trained first
	CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	//SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;	// ���ͣ�Ҳ�����޺�
	SVM_params.kernel_type = CvSVM::RBF;  // CvSVM::RBF �����������Ҳ���Ǹ�˹��
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
	svm_.clear();	// ʹ��load����֮ǰ��Ҫ��clear
	svm_.load(svm_xml_); 
}

std::string SVMTrain::predictResult(const cv::Mat &image)
{
	cv::Mat feature;
	getHOGFeatures(image, feature);
	std::cout << "Hand Gesture Predict��G" << svm_.predict(feature) << std::endl;
	char gesture[32];
	sprintf(gesture, "G%d", (int)svm_.predict(feature));	// floatתint��תchar[]
	return (std::string)gesture;
}

void SVMTrain::test(const cv::Mat &image)
{
	svm_.clear();	// ʹ��load����֮ǰ��Ҫ��clear
	svm_.load(svm_xml_); 

	cv::Mat feature;
	getHOGFeatures(image, feature);
	std::cout << "Hand Gesture Predict��G" << svm_.predict(feature) << std::endl;
	char gesture[32];
	sprintf(gesture, "G%d", (int)svm_.predict(feature));	// floatתint��תchar[]
}

void SVMTrain::test()
{
	Utilities util;
	std::vector<string> files;
	util.getAllFormatFiles("./data/test", "*", files);

	std::cout << "Testing..." << std::endl;

	double count_all = files.size();
	double ptrue_rtrue[10] = {0};	// Ԥ�����ƺ�ʵ�����ƶ�ΪG������
	double ptrue_rfalse[10] = {0};	// Ԥ������ΪG��ʵ�ʲ�ΪG������
	double pfalse_rtrue[10] = {0};	// ʵ������ΪG��Ԥ�ⲻΪG������
	double pfalse_rfalse[10] = {0};	// Ԥ�����Ʋ���Gʵ��Ҳ����G������
	//int gesture = 1;	// ��������Ƶ�׼ȷ�ʺͲ�ȫ��

	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat image = cv::imread(files[i], 0);
		//std::cout << "FilePath�� " << files[i] << std::endl;
		cv::Mat feature;
		getHOGFeatures(image, feature);
		int predict = svm_.predict(feature);
		int real = atoi(files[i].substr(files[i].find_last_of('/', files[i].find_last_of('/') - 1) + 2, files[i].find_last_of('/') - files[i].find_last_of('/', files[i].find_last_of('/') - 1) - 2).c_str());	// ��ȡ��ǩ
		//std::cout << "Hand Gesture Predict��" << svm_.predict(feature) << "\t";
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
		double precise = 0;	// ׼ȷ��
		if (ptrue_rtrue[gesture] + ptrue_rfalse[gesture] != 0) 
		{
			precise = ptrue_rtrue[gesture] / (ptrue_rtrue[gesture] + ptrue_rfalse[gesture]);
			std::cout << "G" << gesture << "precise: " << precise << std::endl;
		} else 
		{
			std::cout << "G" << gesture << "precise: " << "NA" << std::endl;
		}

		double recall = 0;	// ��ȫ��
		if (ptrue_rtrue[gesture] + pfalse_rtrue[gesture] != 0) 
		{
			recall = ptrue_rtrue[gesture] / (ptrue_rtrue[gesture] + pfalse_rtrue[gesture]);
			std::cout << "G" << gesture << "recall: " << recall << std::endl;
		} else 
		{
			std::cout << "G" << gesture << "recall: " << "NA" << std::endl;
		}

		double Fsocre = 0;	// �÷�
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
