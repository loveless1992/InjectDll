#include "pch.h"
#include"dllmain.h"

#define HOOK_LEN 5 //HOOK�ĳ���
BYTE backCode[HOOK_LEN] = { 0 };//���ڷ�װhook�Ĵ���
HWND hDlg = 0;//hoook����ľ��
DWORD hookData = 0;
DWORD WinAdd = 0;
DWORD retCallAdd = 0;//�������hook��������Ҫcall�ĺ���
DWORD retAdd = 0;//���صĵ�ַ
HWND gHwndList = 0;

wchar_t wxid[0x100] = { 0 };
//��ȡģ���ַ
DWORD getWechatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}
//��ʾ�����б�
VOID insertUserLists(DWORD userData)
{
	/*
	eax + 0x10 wxid Ⱥ
	eax + 0x30 wxid Ⱥ
	eax + 0x44 ΢�ź�
	eax + 0x58 V1����
	eax + 0x8C �ǳ�
	eax + 0x11C Сͷ��
	eax + 0x130 ��ͷ��
	eax + 0x144 δ֪md5����
	eax + 0x1C8 ����
	eax + 0x1DC ʡ��
	eax + 0x1F0 ����
	eax + 0x204 �����Դ
	eax + 0x294 ����Ȧ��ֽ
	*/

	DWORD wxidAdd = userData + 0xc;
	swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd));

	/*MessageBox(NULL, wxid, L"wxid", NULL);
	MessageBox(NULL, wxid, L"�����ˣ�������", NULL);*/
}



//��ʼhook
/**
 * ����һ hookAdd ��Ҫhook�ĵ�ַ
 * ������ jmpAdd hook���ȥ�ĵ�ַ
**/
VOID StartHook(DWORD hookAdd, LPVOID jmpAdd)
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//������Ҫ���һ������������
	// E9 11051111(��������ת�ĵط�����ط�����һ�������ַ ���Ǹ���hook��ַ����ת�Ĵ����ַ�ľ�����������)
	JmpCode[0] = 0xE9;
	//������ת�ľ��빫ʽ�ǹ̶���
	//���㹫ʽΪ ��ת�ĵ�ַ(Ҳ�������Ǻ����ĵ�ַ) - hook�ĵ�ַ - hook���ֽڳ���
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook�ڶ��� �ȱ��ݽ�Ҫ�����Ǹ��ǵ�ַ������ ����Ϊ����hook�ĳ��� HOOK_LEN 5���ֽ�

	//��ȡ���̾��
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

	//��������
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook��ַ�����ݶ�ȡʧ��", L"��ȡʧ��", MB_OK);
		return;
	}

	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hookд��ʧ�ܣ������滻ʧ��", L"����", MB_OK);
		return;
	}

}

DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;
DWORD userData = 0;

//�洢�Ĵ����������Լ��ķ�����֮��ָ��Ĵ���
VOID __declspec(naked) HookF()
{
	__asm {
		call retCallAdd
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
	}
	//Ȼ����ת�������Լ��Ĵ����� ��������
	insertUserLists(cEax);
	__asm {
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		jmp retAdd
	}
}

VOID HookWechatQrcode(HWND hwndDlg, DWORD HookAdd)
{
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	retCallAdd = WinAdd + 0x10D000;
	retAdd = WinAdd + 0x55DEA2;
	StartHook(WinAdd + HookAdd, &HookF);
}