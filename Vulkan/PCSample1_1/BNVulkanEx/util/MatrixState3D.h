#ifndef MatrixState_hpp//��ֹ�ظ�����
#define MatrixState_hpp

#include "Matrix.h"//������Ҫ��ͷ�ļ�

class MatrixState3D
{
public:
	static float currMatrix[16];//��ǰ�任����
	static float mProjMatrix[16];//ͶӰ����
	static float mVMatrix[16];//���������
	static float mMVPMatrix[16];//�ܾ���
	static float vulkanClipMatrix[16];//���ÿռ���� X���� Y�÷� Z����
	static float mStack[10][16];//�����任�����ջ
	static int stackTop;//ջ��λ��

	static float cx, cy, cz;//�����λ��

	static void setInitStack();//��ʼ������

	static void pushMatrix();//�����任����

	static void popMatrix();//�ָ��任����

	static void translate(float x, float y, float z);//��x��y��z��ƽ��

	static void rotate(float angle, float x, float y, float z);//��ָ������ת

	static void scale(float x, float y, float z);//��������
	static void setCamera//���������
	(
		float cx,
		float cy,
		float cz,
		float tx,
		float ty,
		float tz,
		float upx,
		float upy,
		float upz
	);

	static void setProjectFrustum//����͸��ͶӰ����
	(
		float left,
		float right,
		float bottom,
		float top,
		float near,
		float far
	);

	static float* getFinalMatrix();//��ȡ���վ���

	static float* getMMatrix();//��ȡ��ǰ�任����
};


#endif
