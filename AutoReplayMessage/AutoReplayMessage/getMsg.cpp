#include "pch.h"
#include"dllmain.h"

#define HOOK_LEN 5 //HOOK�ĳ���
BYTE backCode[HOOK_LEN] = { 0 };//���ڷ�װhook�Ĵ���
DWORD WinAdd = 0;
DWORD retAdd = 0;//���صĵ�ַ
HWND hdl = 0;//����ģ����

//hook֮����Ҫ�Ĳ��� [[esp]]+0x70Ϊ��Ϣ���ݣ�[[esp]]+0x48Ϊ������ID  esp+0xc�����01�Ļ������Լ������Լ���
void getMsg(DWORD userData)
{
	wchar_t wxid[0x100] = { 0 };
	wchar_t msg[0x4096] = { 0 };
	DWORD espAddress = *(DWORD*)*(DWORD*)userData;
	if(*(int *)(userData +0xc) == 1)
	{
		return;
	}

	//��Ϣ�ռ�
	if (swprintf_s(wxid, L"%s", *((LPVOID*)(espAddress + 0x48))) != -1&& swprintf_s(msg, L"%s", *((LPVOID*)(espAddress + 0x70))) != -1)
	{
		//MessageBox(NULL, (LPCUWSTR)wxid, L"������", MB_OK);
		//MessageBox(NULL, (LPCUWSTR)msg, L"��Ϣ����", MB_OK);
		
		//�ٴ������÷��ͺ�������һЩ��Ϣ
		if(checkCanSendMsg(wxid))
		{
			wchar_t* retMsg = getReplayMsg(msg);
			SendTextMessage(wxid, retMsg);
		}
			
	}
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

//�洢�Ĵ����������Լ��ķ�����֮��ָ��Ĵ���
//�Ĵ�����Ҫcall eax+0x8��������һ��λ�ò��㣬������call������push֮���jmp����
VOID __declspec(naked) HookF()
{
	__asm {
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
	getMsg(cEsp);
	__asm {
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		call dword ptr ds : [eax + 0x8]
		push edi
		push ecx
		jmp retAdd
	}
}

//����1�������� ����2������hook��ƫ�ƣ�hookƫ�ƣ�0x4111DB    ����ƫ�ƣ�0x4111E0
void HookGetMseeage(HWND hwndDlg, DWORD HookAdd)
{
	hdl = hwndDlg;
	WinAdd = getWeChatWin();
	retAdd = WinAdd + 0x4111E0;
	StartHook(WinAdd + HookAdd, &HookF);
}

//ж��HOOK
bool endHook(DWORD hookA)
{
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook�ĵ�ַ+ƫ��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, L"HOOKж�سɹ�", L"��ʾ", NULL);
		return false;
	}
	return true;
}