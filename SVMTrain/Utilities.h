#pragma once

#include <io.h>  
#include <direct.h>  
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);

	// 获取文件夹下指定格式所有文件名
	void getAllFormatFiles(string input_path,	// 输入文件夹路径 
						   string input_format,	// 输入指定文件格式，若为*，则获取全部文件
						   vector<string> &output_files);	// 输出完整文件名（包括路径）
};

