#include "Utilities.h"


Utilities::Utilities(void)
{
}


Utilities::~Utilities(void)
{
}

void Utilities::getAllFormatFiles(string input_path, string input_format, vector<string> &out_files) 
{  
    //�ļ����    
    long   hFile   =   0;    
    //�ļ���Ϣ    
    struct _finddata_t fileinfo;    
    string pathName;    
    if((hFile = _findfirst(pathName.assign(input_path).append("/*." + input_format).c_str(),&fileinfo)) !=  -1)    
    {    
        do    
        {   
            //�����Ŀ¼,����֮  
            //�������,�����б�  
            if((fileinfo.attrib &  _A_SUBDIR))    
            {    
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)    
                {  
                    //files.push_back(p.assign(path).append("/").append(fileinfo.name) );  
                    getAllFormatFiles( pathName.assign(input_path).append("/").append(fileinfo.name), input_format, out_files);   
                }  
            }    
            else    
            {    
                out_files.push_back(pathName.assign(input_path).append("/").append(fileinfo.name) );    
            }    
        }while(_findnext(hFile, &fileinfo) == 0);    

        _findclose(hFile);   
    }   
}  

void Utilities::makeDir(const string &input_path)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = input_path.find('/', start)) != std::string::npos) 
	{
		if (end != start) 
		{
			tokens.push_back(input_path.substr(0, end));
		}
		start = end + 1;
	}
	if (end != start) 
	{
		tokens.push_back(input_path);
	}

	vector<string>::const_iterator itp = tokens.begin();
	while (itp != tokens.end())
	{
		if (access(itp->c_str(), 0) == -1)  // �ж��ļ����Ƿ����  
		{  
			//cout<<*itp<<" is not existing"<<endl;  
			//cout<<"now make it"<<endl;  
			if (mkdir(itp->c_str()) == 0)   // �������򴴽���ֻ��һ��һ���Ĵ���
			{
				//cout<<"make successfully"<<endl; 
			}
		}
		*itp++;
	}
}

int Utilities::medianBlurOption(const cv::Mat &img, int row, int col, int kernelSize)
{
	std::vector<uchar> pixels;
	for (int a = -kernelSize / 2; a <= kernelSize / 2; a++)
	{
		for (int b = -kernelSize / 2; b <= kernelSize / 2; b++)
		{
			pixels.push_back(img.at<uchar>(row + a, col + b));
		}
	}
	sort(pixels.begin(), pixels.end());

	int mid = pixels[kernelSize * kernelSize / 2];


	return mid;
}

void Utilities::applyMedianBlurOption(const cv::Mat &img, cv::Mat &out_img, int kernelSize)
{
	img.copyTo(out_img);
	// ��չͼ��ı߽�
	cv::Mat image;
	cv::copyMakeBorder(img, image, kernelSize / 2, kernelSize / 2, kernelSize / 2, kernelSize / 2, cv::BORDER_REFLECT);
	// ͼ��ѭ��
	for (int j = kernelSize / 2; j < image.rows - kernelSize / 2; j++)
	{
		for (int i = kernelSize / 2; i < image.cols - kernelSize / 2; i++)
		{
			if (img.at<uchar>(j-kernelSize / 2, i-kernelSize / 2) == 0)
			{
				out_img.at<uchar>(j-kernelSize / 2, i-kernelSize / 2) = medianBlurOption(image, j, i, kernelSize);
			} 
		}
	}
}
