#include "pch.h"
#include"dllmain.h"

#define GET_USER_LIST_HOOK_LEN 5 //HOOK�ĳ���
BYTE getUserListBackCode[GET_USER_LIST_HOOK_LEN] = { 0 };//���ڷ�װhook�Ĵ���
HWND getUserListHDlg = 0;//hoook����ľ��
DWORD getUserListRetCallAdd = 0;//�������hook��������Ҫcall�ĺ���
DWORD getUserListRetAdd = 0;//���صĵ�ַ
HWND getUserListGHwndList = 0;

char* list[10000];//��΢���˺�
char* nameList[10000];//��΢����
int listLen = 0;


char* ignoreList[10000];//��ӽ������б������
int ignoreLen = 0;


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

	if(*((LPVOID*)wxidAdd) == NULL)
	{
		return;
	}
	if (*((LPVOID*)wxNameAdd) == NULL)
	{
		return;
	}
	if (*((LPVOID*)userNameAdd) == NULL)
	{
		return;
	}

	//ȥ��
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		list[listLen] = UnicodeToUTF8(wxid);
		nameList[listLen] = UnicodeToUTF8(userName);
		listLen++;

		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPWSTR)wxid;
		ListView_InsertItem(getUserListGHwndList, &item);

		item.iSubItem = 1;
		item.pszText = (LPWSTR)wxName;
		ListView_SetItem(getUserListGHwndList, &item);

		item.iSubItem = 2;
		item.pszText = (LPWSTR)userName;
		ListView_SetItem(getUserListGHwndList, &item);
	}
}

//��ʼhook
/**
 * ����һ hookAdd ��Ҫhook�ĵ�ַ
 * ������ jmpAdd hook���ȥ�ĵ�ַ
**/
VOID GetUserListStartHook(DWORD hookAdd, LPVOID jmpAdd)
{
	BYTE JmpCode[GET_USER_LIST_HOOK_LEN] = { 0 };
	//������Ҫ���һ������������
	// E9 11051111(��������ת�ĵط�����ط�����һ�������ַ ���Ǹ���hook��ַ����ת�Ĵ����ַ�ľ�����������)
	JmpCode[0] = 0xE9;
	//������ת�ľ��빫ʽ�ǹ̶���
	//���㹫ʽΪ ��ת�ĵ�ַ(Ҳ�������Ǻ����ĵ�ַ) - hook�ĵ�ַ - hook���ֽڳ���
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - GET_USER_LIST_HOOK_LEN;

	//hook�ڶ��� �ȱ��ݽ�Ҫ�����Ǹ��ǵ�ַ������ ����Ϊ����hook�ĳ��� HOOK_LEN 5���ֽ�

	//��ȡ���̾��
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

	//��������
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, getUserListBackCode, GET_USER_LIST_HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook��ַ�����ݶ�ȡʧ��", L"��ȡʧ��", MB_OK);
		return;
	}

	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, GET_USER_LIST_HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hookд��ʧ�ܣ������滻ʧ��", L"����", MB_OK);
		return;
	}

}

DWORD cgEax = 0;
DWORD cgEcx = 0;
DWORD cgEdx = 0;
DWORD cgEbx = 0;
DWORD cgEsp = 0;
DWORD cgEbp = 0;
DWORD cgEsi = 0;
DWORD cgEdi = 0;

//�洢�Ĵ����������Լ��ķ�����֮��ָ��Ĵ���
VOID __declspec(naked) GetUserListHookF()
{
	__asm {
		call getUserListRetCallAdd
		mov cgEax, eax
		mov cgEcx, ecx
		mov cgEdx, edx
		mov cgEbx, ebx
		mov cgEsp, esp
		mov cgEbp, ebp
		mov cgEsi, esi
		mov cgEdi, edi
	}
	//Ȼ����ת�������Լ��Ĵ����� ��������
	insertUserLists(cgEax);
	__asm {
		mov eax, cgEax
		mov ecx, cgEcx
		mov edx, cgEdx
		mov ebx, cgEbx
		mov esp, cgEsp
		mov ebp, cgEbp
		mov esi, cgEsi
		mov edi, cgEdi
		jmp getUserListRetAdd
	}
}

VOID GetUserListHookStart(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	//������������
	wchar_t*  specialWxID = (wchar_t *)"q315319188";
	ignoreList[listLen] = UnicodeToUTF8(specialWxID);
	ignoreLen++;

	getUserListGHwndList = hwndList;
	DWORD WinAdd = getWeChatWin();
	getUserListHDlg = hwndDlg;
	getUserListRetCallAdd = WinAdd + 0x10D000;
	getUserListRetAdd = WinAdd + 0x55DEA2;
	GetUserListStartHook(WinAdd + HookAdd, &GetUserListHookF);
}
//ж��HOOK
bool endGetUserListHook(DWORD hookA)
{
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook�ĵ�ַ+ƫ��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, getUserListBackCode, GET_USER_LIST_HOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, L"HOOKж�سɹ�", L"��ʾ", NULL);
		return false;
	}
	return true;
}


//�ⲿ���ã��������Ƿ���Է�����Ϣ
BOOL checkCanSendMsg(wchar_t * wxid)
{
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if(!flag)
	{
		return flag;
	}

	for (int i = 0; i < ignoreLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = false;
			break;
		}
	}
	return flag;
}
