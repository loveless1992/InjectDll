#include "pch.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <string>

//΢������05BC1BAC = WeChatWin.dll + 1AD 1BAC
//΢���˺ţ�05BC1D10 = WeChatWin.dll + 1AD 1D10
//ͷ��05BC1E74 = = WeChatWin.dll + 1AD 1E74
//�ֻ��ţ�05BC1BE0 = WeChatWin.dll + 1AD 1BE0


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
	sprintf_s(wxid, "%s", winApp + 0x1AD1D10);
	SetDlgItemText(hwndDlg, TEXT_ID, wxid);

	//΢����
	char wxName[0x100] = { 0 };
	sprintf_s(wxName, "%s", winApp + 0x1AD1BAC);

	wxName[strlen(wxName)] = '\0';
	SetDlgItemText(hwndDlg, TEXT_NAME, wxName);
	MessageBox(NULL, wxName, "���", 0);

	//΢��ͷ��
	char wxPic[0x100] = { 0 };
	DWORD pPic = winApp + 0x1AD1E74;
	sprintf_s(wxPic, "%s", *((DWORD *)pPic));
	SetDlgItemText(hwndDlg, TEXT_PIC, wxPic);


}