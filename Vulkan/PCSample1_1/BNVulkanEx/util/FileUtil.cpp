#include "FileUtil.h"
#include<fstream>
#include<string>
#include<iostream>
#include <assert.h>

//����Assets�ļ����µ�ָ���ı������ļ�������Ϊ�ַ�������
string FileUtil::loadAssetStr(string fname)
{
	ifstream infile;
	infile.open(fname.data());   //���ļ����������ļ��������� 
	cout << "fname>" << fname << endl;
	assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
	string ss;
	string s;
	while (getline(infile, s))
	{
		ss += s;
		ss += '\n';
	}
	infile.close();             //�ر��ļ������� 
	/*cout << ss << endl;*/
	return ss;
}