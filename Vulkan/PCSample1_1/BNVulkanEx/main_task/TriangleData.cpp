#include "TriangleData.h"
#include <vector>
#include <math.h>
#include <string.h>

float* TriangleData::vdata;//���������׵�ַָ��
int TriangleData::dataByteCount;//������ռ���ֽ�����
int TriangleData::vCount;//��������
void  TriangleData::genVertexData() {//�����������ɷ���
	vCount = 3;//��������
	dataByteCount = vCount * 6 * sizeof(float);//������ռ�ڴ����ֽ���
	vdata = new float[vCount * 6]{ //��������
		0, 75, 0, 1, 0, 0,//ÿһ��ǰ�����Ƕ�������
		-45, 0, 0, 0, 1, 0,//ÿһ�к���������ɫRGBֵ
		45, 0, 0, 0, 0, 1
	};
}