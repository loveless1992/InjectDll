#include "pch.h"
#include "dllMain.h"

#define HOOOK_LEN 5

//ȫ�ֻ����������ڴ�ű�������
BYTE backCode[HOOOK_LEN] = { 0 };
HWND hDlg = 0;

//��ȡģ���ַ
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary("WeChatWin.dll");
}

VOID saveImg(DWORD qrCode)
{
	//��ȡ��ecx+4λ�ô洢�����ݣ�ΪͼƬ����
	DWORD picLen = qrCode + 0x4;
	size_t cpyLen = (size_t)*((LPVOID*)picLen);
	//ͼƬ������
	char picData[0xFFF] = { 0 };
	memcpy(picData, *((LPVOID*)qrCode), cpyLen);
	//�򿪻򴴽�һ��ͼƬ�ļ�
	FILE* pFile;
	fopen_s(&pFile, "qrcode.png", "wb");
	//������д���ļ�
	fwrite(picData, sizeof(char), sizeof(picData), pFile);
	fclose(pFile);
	//��ʾͼƬ
	CImage img;
	CRect rect;
	HWND PicHdl = GetDlgItem(hDlg, QR_CODE);
	GetClientRect(PicHdl, &rect);
	//����ͼƬ
	img.Load("qrcode.png");
	img.Draw(GetDC(PicHdl), rect);
}

DWORD pEax = 0;
DWORD pEcx = 0;
DWORD pEdx = 0;
DWORD pEbx = 0;
DWORD pEbp = 0;
DWORD pEsp = 0;
DWORD pEsi = 0;
DWORD pEdi = 0;
DWORD retAdd = 0;

//hook������  __declspec(naked)������Ϊһ���㺯��������Զ�ջ���в������㺯���ڲ��������κα���
VOID __declspec(naked) show()
{
	//���ݼĴ���
	__asm{
		mov pEax, eax
		mov pEcx, ecx
		mov pEdx, edx
		mov pEbx, ebx
		mov pEbp, ebp
		mov pEsp, esp
		mov pEsi, esi
		mov pEdi, edi
	}
	//��ecx�еĶ�ά�����ݴ���
	saveImg(pEcx);
	retAdd = getWeChatWin()+ 0x5CABEF;
	//�ָ��Ĵ���
	__asm{
		mov eax,pEax
		mov ecx,pEcx
		mov edx,pEdx
		mov ebx,pEbx
		mov ebp,pEbp
		mov esp,pEsp
		mov esi,pEsi
		mov edi,pEdi
	    jmp retAdd
	}
}



//΢�ŵ�½��ά��ƫ�� : WeChatWin.dll + 0x5CABEA
// hook���صĵ�ַ��: WeChatWin.dll + 0x5CABEF
//��ʼHOOK pram������Ҫhook�ĺ�����ƫ��   ��ת�ĺ���
bool startHook(DWORD hookA,LPVOID func, HWND hwndDlg)
{
	hDlg = hwndDlg;//��һ��������Ϊ�����㺯�����Բ������ڲ�����������Ҳ����ܲ���
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook�ĵ�ַ+ƫ��

    //��װ����
	BYTE jmpCode[HOOOK_LEN] = { 0 };
	//hook��һ�ֽ�Ϊ���ͣ�E9����д���jump��E8Ϊcall
	jmpCode[0] = 0xE9;
	//�ڶ��ֽڴ�������Ҫ�ĵ�ַ������Ϊ��Ҫ��ת�ĵ�ַ-hook��ַ-5
	*(DWORD *)&jmpCode[1] = (DWORD)func - hookAdd - HOOOK_LEN;//�����Ϊɶ������ָ�룡������
	
	//����ԭ���ݣ�ж��ʱ���滻���ݼ���
	//��ȡ�Լ��Ľ��̾ݾ��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (ReadProcessMemory(hProcess, (LPCVOID)hookAdd, backCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "�ڴ汸��ʧ��", "����", NULL);
		return false;
	}

	//д������
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, jmpCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "�ڴ�д��ʧ��", "����", NULL);
		return false;
	}
	return true;
}


//ж��HOOK
bool endHook(DWORD hookA)
{
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook�ĵ�ַ+ƫ��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, backCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "HOOKж�سɹ�", "��ʾ", NULL);
		return false;
	}
	return true;
}