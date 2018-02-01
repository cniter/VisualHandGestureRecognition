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
