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

char* list[10000];//��΢���˺�
char* nameList[10000];//��΢����
int listLen = 0;

//��ȡģ���ַ
DWORD getWechatWin()
{
	return (DWORD)LoadLibrary("WeChatWin.dll");
}



CHAR* UnicodeToUTF8(const WCHAR* wideStr)
{
	char* utf8Str = NULL;
	int charLen = -1;
	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	utf8Str = (char*)malloc(charLen);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);
	return utf8Str;
}

char* UnicodeToANSI(const wchar_t* str)
{
	char* result;
	int textlen = 0;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char*)malloc((textlen + 1) * sizeof(char));
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}
//��ʾ�����б�
VOID insertUserLists(DWORD userData)
{
	//΢��ID
	wchar_t wxid[0x100] = { 0 };
	DWORD wxidAdd = userData + 0x38;
	if (swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd)) == -1)
	{
		return;
	}

	//΢�ź�
	wchar_t wxName[0x100] = { 0 };
	DWORD wxNameAdd = userData + 0x4C;
	if (swprintf_s(wxName, L"%s", *((LPVOID*)wxNameAdd)) == -1)
	{
		return;
	}

	//�û��ǳ�
	wchar_t userName[0x100] = { 0 };
	DWORD userNameAdd = userData + 0x94;
	if (swprintf_s(userName, L"%s", *((LPVOID*)userNameAdd)) == -1)
	{
		return;
	}

	//ȥ��
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToANSI(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		list[listLen] = UnicodeToANSI(wxid);
		nameList[listLen] = UnicodeToANSI(userName);
		listLen++;


		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPSTR)UnicodeToANSI(wxid);
		ListView_InsertItem(gHwndList, &item);

		item.iSubItem = 1;
		item.pszText = UnicodeToANSI(wxName);
		ListView_SetItem(gHwndList, &item);

		item.iSubItem = 2;
		item.pszText = UnicodeToANSI(userName);
		ListView_SetItem(gHwndList, &item);
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
		MessageBox(NULL, "hook��ַ�����ݶ�ȡʧ��", "��ȡʧ��", MB_OK);
		return;
	}

	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, "hookд��ʧ�ܣ������滻ʧ��", "����", MB_OK);
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

VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	gHwndList = hwndList;
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	retCallAdd = WinAdd + 0x10D000;
	retAdd = WinAdd + 0x55DEA2;
	MessageBox(NULL, "HOOK��ʼ", "��ʾ", MB_OK);
	StartHook(WinAdd + HookAdd, &HookF);
}

//ж��HOOK
bool endHook(DWORD hookA)
{
	DWORD winAdd = getWechatWin();
	DWORD hookAdd = winAdd + hookA;//hook�ĵ�ַ+ƫ��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "HOOKж�سɹ�", "��ʾ", NULL);
		return false;
	}
	return true;
}