#include "FileUtil.h"
#include<fstream>
#include<string>
#include<iostream>
#include <assert.h>

//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
string FileUtil::loadAssetStr(string fname)
{
	ifstream infile;
	infile.open(fname.data());   //将文件流对象与文件连接起来 
	cout << "fname>" << fname << endl;
	assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
	string ss;
	string s;
	while (getline(infile, s))
	{
		ss += s;
		ss += '\n';
	}
	infile.close();             //关闭文件输入流 
	/*cout << ss << endl;*/
	return ss;
}