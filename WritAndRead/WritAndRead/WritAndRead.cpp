#include "pch.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <string>

//΢��3.3
//ƫ��
	//΢���ǳƣ�0x1DDD55C
	//�ֻ��ţ�0x1DDD590
	//΢��ID��0x1DDD6C0
	//΢��UID��0x1DDD978
    //�ֻ��ͺţ�0x1DDD9FA
	//΢��ͷ��0x1DDD824


//���Ȼ�ȡWeChatWin.dll��ַ
DWORD getWechatWinAdd()
{
	HMODULE winApp = LoadLibrary("WeChatWin.dll");
	return (DWORD)winApp;
}
//��ȡ�ڴ�����
void readWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//΢��ID
	char wxid[0x100] = { 0 };
	sprintf_s(wxid, "%s", winApp + 0x1DDD6C0);
	SetDlgItemText(hwndDlg, TEXT_ID, wxid);

	//΢����
	char wxName[0x100] = { 0 };
	sprintf_s(wxName, "%s", winApp + 0x1DDD55C);
	SetDlgItemText(hwndDlg, TEXT_NAME, wxName);

	//�ֻ���
	char phone[0x100] = { 0 };
	sprintf_s(phone, "%s", winApp + 0x1DDD590);
	SetDlgItemText(hwndDlg, TEXT_PHONE, phone);

	//΢��ͷ��
	char wxPic[0x200] = { 0 };
	DWORD pPic = winApp + 0x1DDD824;
	sprintf_s(wxPic, "%s", *((DWORD *)pPic));
	SetDlgItemText(hwndDlg, TEXT_PIC, wxPic);
}

//д�ڴ�
void writeWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//΢��ID
	DWORD wxNameP = winApp + 0x1DDD55C;
	char wxName[0x100] = { 0 };
	GetDlgItemText(hwndDlg, TEXT_NAME, wxName,sizeof(wxName));
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)wxNameP, wxName, sizeof(wxName), NULL);

}