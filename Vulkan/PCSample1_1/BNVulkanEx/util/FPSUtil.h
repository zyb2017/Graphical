#ifndef VULKANEXBASE_FPSUTIL_H
#define VULKANEXBASE_FPSUTIL_H


class FPSUtil
{
public:
	//���ڼ���FPS�ĸ�������
	static int FPSCount;
	static long long startTime;
	static float currFPS;

	//���ڿ���֡���ʵĸ�������
	static long long beforeTime;

	//����FPS��ط���
	static void init();
	static void calFPS();

	//����֡������ط���
	static void before();
	static void after(int dstFPS);
};


#endif //VULKANEXBASE_FPSUTIL_H
