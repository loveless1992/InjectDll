#include "pch.h"
#include"dllmain.h"

/*
 * ��ģ�����Ⱪ¶һ���ӿڣ��ýӿڿ��Զ�ʱ������Ϣ�������Զ�������ز���
 */

void* show(void* data)
{
	time_t startTime = time(NULL);
	
	while (true)
	{
		time_t nowTime = time(NULL);
		if (nowTime  - startTime >= 5)
		{
			SendTextMessage((wchar_t*)L"filehelper", (wchar_t*)L"666666666");
			startTime = nowTime;
		}

	}
	return NULL;
}

bool alreadyCreatThread = false;
//������֮���˭��ʲô��Ϣ
void startThreadToDoSth(int time,wchar_t msg,wchar_t wxid)
{
	if(alreadyCreatThread == false)
	{
		pthread_t th;
		//�����߳�
		pthread_create(&th, 0, show, NULL);
		//pthread_join��������ǰ�߳�   pthread_detach����������ǰ�߳�
		//pthread_join(th, NULL);
		pthread_detach(th);
		alreadyCreatThread = true;
	}
}