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

	// ��ȡ�ļ�����ָ����ʽ�����ļ���
	void getAllFormatFiles(string input_path,	// �����ļ���·�� 
						   string input_format,	// ����ָ���ļ���ʽ����Ϊ*�����ȡȫ���ļ�
						   vector<string> &output_files);	// ��������ļ���������·����
};

