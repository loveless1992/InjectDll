#include "pch.h"
#include"Inject.h"

VOID setWindow(HWND thisWindow)
{
	HWND wechatWindow = FindWindow("WeChatMainWndForPC", NULL);
	//�ϣ�20 �£�620 ��10 �ң�720
	//MoveWindow(wechatWindow, 10, 20, 100, 600, TRUE);

	RECT wechatHandle = { 0 };
	GetWindowRect(wechatWindow, &wechatHandle);
	LONG width = wechatHandle.right - wechatHandle.left;
	LONG height = wechatHandle.bottom - wechatHandle.top;
	MoveWindow(thisWindow, wechatHandle.left - 230, wechatHandle.top, 240, height, TRUE);
	SetWindowPos(thisWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	char buff[0x100] = {};
	sprintf_s(buff, "�ϣ�%d �£�%d ��%d �ң�%d\r\n", wechatHandle.top, wechatHandle.bottom, wechatHandle.left, wechatHandle.right);
	OutputDebugString(buff);
}

//ͨ��΢�Ž������ҵ�PID��ͨ��PID�򿪽��̻�ȡ�����̾��,����΢��3.2.X������ӽ��̣���������ֱ�Ӳ��񸸽���PID
DWORD ProcessNameToFindPID(char* ProcessName)
{
	//��ȡ������ϵͳ���̿���,��Ҫ#include<TlHelp32.h>
	HANDLE processAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	//�����ԱȽ�������
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	//���ｫ���еĽ���ID�Լ�����ID��������
	int processIDArr[10] = { -1 };
	int processParentIDArr[10] = { -1 };
	int num = 0;
	while (Process32Next(processAll, &processInfo))
	{
		if (strcmp((const char*)processInfo.szExeFile, ProcessName) == 0)
		{
			processIDArr[num] = processInfo.th32ProcessID;
			processParentIDArr[num] = processInfo.th32ProcessID;
			num++;
		}
	}
	//���������������Ӿ�������д��ڸ��������ô�˸������������Ҫ�ľ��
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			if (processParentIDArr[j] == processIDArr[i])
			{
				return  processParentIDArr[j];
			}
		}
	}
	return -999;
}

//ע��dll
VOID injectDll(char* dllPath ,char* ProcessName)
{
	char buff[0x100] = { 0 };
	//��ȡĿ�����PID
	DWORD PID = ProcessNameToFindPID(ProcessName);
	if (PID == 0) {
		MessageBox(NULL, "û���ҵ��ý��̣�����Ϊ���������", "û���ҵ�", MB_OK);
		return;
	}
	else {
		//�ҵ�pid���Ǿʹ򿪽���
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
		if (NULL == hProcess) {
			MessageBox(NULL, "���̴�ʧ��", "����", MB_OK);
			return;
		}
		else {
			DWORD strSize = (DWORD)(_tcslen(dllPath) + 1) * sizeof(TCHAR);
			//���̴򿪺����ǰ����ǵ�dll·�����ȥ
			//��������һƬ�ڴ����ڴ���dll·��
			LPVOID allocRes = VirtualAllocEx(hProcess, NULL, strSize, MEM_COMMIT, PAGE_READWRITE);
			if (NULL == allocRes) {
				MessageBox(NULL, "�ڴ�����ʧ��", "����", MB_OK);
				return;
			}

			//����ú� ����д��·����Ŀ���ڴ浱��
			if (WriteProcessMemory(hProcess, allocRes, (LPVOID)dllPath, strSize, NULL) == 0) {
				MessageBox(NULL, "DLL·��д��ʧ��", "����", MB_OK);
				return;
			}
			//·��д�� �ɹ����������ڻ�ȡLoadLibraryW ��ַ
			//LoadLibraryW ��Kernel32.dll���� ���������Ȼ�ȡ���dll�Ļ�ַ
			HMODULE hModule = GetModuleHandle("Kernel32.dll");
			LPVOID address = GetProcAddress(hModule, "LoadLibraryA");
			sprintf_s(buff, "loadLibrary=%p path=%p", address, allocRes);
			OutputDebugString(buff);
			//ͨ��Զ���߳�ִ��������� �������� ����dll�ĵ�ַ
			//��ʼע��dll
			HANDLE hRemote = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, allocRes, 0, NULL);
			if (NULL == hRemote) {
				MessageBox(NULL, "Զ��ִ��ʧ��", "����", MB_OK);
				return;
			}
		}
	}
}
//ж��DLL
BOOL UnInjectDll(DWORD dwPID, LPCTSTR szDllPath, int& nError)
{
	nError = 0;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;  // �洢��Ŀ�����������ڴ��ַ  
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);  // �洢DLL�ļ�·��������ڴ�ռ��С  
	LPTHREAD_START_ROUTINE pThreadProc;

	WCHAR* pDllName = (WCHAR*)wcsrchr((wchar_t*)szDllPath, '\\');
	pDllName = pDllName + 1;
	//�������̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	MODULEENTRY32 ME32 = { 0 };
	ME32.dwSize = sizeof(MODULEENTRY32);
	BOOL isNext = Module32First(hSnap, &ME32);
	BOOL flag = FALSE;
	while (isNext)
	{
		wchar_t ws[100];
		swprintf(ws, 100, L"%hs", ME32.szModule);

		if (wcscmp((wchar_t const*)ws, (wchar_t const*)pDllName) == 0)
		{
			flag = TRUE;
			break;
		}
		isNext = Module32Next(hSnap, &ME32);
	}
	if (flag == FALSE)
	{
		nError = 1;
		return FALSE;

	}

	// ��ȡĿ����̾��
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwPID);
	if (NULL == hProcess)
	{
		return false;
	}

	pThreadProc = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "FreeLibrary");
	if (pThreadProc == NULL)
	{
		nError = 2;
		return FALSE;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, ME32.modBaseAddr, 0, NULL);
	if (hThread == NULL)
	{
		nError = 3;
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}

//����΢��
//CreateProcess ����Ŀ����� ����ʱ������ý���.
//Ȼ��ע��
//Ȼ����ResumeThread ��Ŀ���������
VOID runWechat(TCHAR* dllPath, TCHAR* wechatPath)
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;//SW_SHOW
	CreateProcess(NULL, wechatPath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	LPVOID Param = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	TCHAR add[0x100] = { 0 };
	if (WriteProcessMemory(pi.hProcess, Param, dllPath, strlen(dllPath) * 2 + sizeof(char), NULL) == 0) 
	{
		MessageBox(NULL, "DLL·��д��ʧ��", "����", MB_OK);
		return;
	}
	TCHAR buff[0x100] = { 0 };
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	LPVOID address = GetProcAddress(hModule, "LoadLibraryA");
	//ͨ��Զ���߳�ִ��������� �������� ����dll�ĵ�ַ
	//��ʼע��dll
	HANDLE hRemote = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, Param, 0, NULL);
	if (NULL == hRemote) {
		MessageBox(NULL, "Զ��ִ��ʧ��", "����", MB_OK);
		return;
	}
	DWORD TIME = ResumeThread(pi.hThread);
}