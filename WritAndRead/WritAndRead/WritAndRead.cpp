#include "pch.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <string>

//3.x�汾
//΢������05BC1BAC = WeChatWin.dll + 1AD 1BAC
//΢���˺ţ�05BC1D10 = WeChatWin.dll + 1AD 1D10
//ͷ��05BC1E74 = = WeChatWin.dll + 1AD 1E74
//�ֻ��ţ�05BC1BE0 = WeChatWin.dll + 1AD 1BE0
//΢��2.x�汾
//���̣�weChatStore.exe
//ƫ�ƣ�
//΢������05BC1BAC = WeChatWin.dll + 104F52C
//΢���˺ţ�05BC1D10 = WeChatWin.dll+104F690
//ͷ��05BC1E74 = = WeChatWin.dll + 104F7F4
//�ֻ��ţ�05BC1BE0 = WeChatWin.dll + 104F560


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
	sprintf_s(wxid, "%s", winApp + 0x104F690);
	SetDlgItemText(hwndDlg, TEXT_ID, wxid);

	//΢����
	char wxName[0x100] = { 0 };
	sprintf_s(wxName, "%s", winApp + 0x104F52C);
	SetDlgItemText(hwndDlg, TEXT_NAME, wxName);

	//�ֻ���
	char phone[0x100] = { 0 };
	sprintf_s(phone, "%s", winApp + 0x104F560);
	SetDlgItemText(hwndDlg, TEXT_PHONE, phone);

	//΢��ͷ��
	char wxPic[0x200] = { 0 };
	DWORD pPic = winApp + 0x104F7F4;
	sprintf_s(wxPic, "%s", *((DWORD *)pPic));
	SetDlgItemText(hwndDlg, TEXT_PIC, wxPic);
}

//д�ڴ�
void writeWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//΢��ID
	DWORD wxIdP = winApp + 0x1AD1D10;
	char wxid[0x100] = { 0 };
	GetDlgItemText(hwndDlg, TEXT_ID, wxid,sizeof(wxid));
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)wxIdP, wxid, sizeof(wxid), NULL);

}