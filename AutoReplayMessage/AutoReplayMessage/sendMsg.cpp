#include "pch.h"
#include"dllmain.h"
// ��Ϣ�ṹ��

#define sendMsgCallAddress 0x3E3B80 //������Ϣ����Ҫcall�ĺ�����ַ
#define sendMsgHookAddress 0x11EECB //������Ϣ��ַ

struct wxMessageStruct
{
	wchar_t* text;
	DWORD textLength;
	DWORD textLen;
	int full1 = 0;
	int full2 = 0;
};
VOID SendTextMessage(wchar_t* wxid, wchar_t* message)
{
	DWORD winAddress = (DWORD)getWeChatWin();
	DWORD sendCallAddress = winAddress + sendMsgCallAddress;
	wxMessageStruct wxWxid = { 0 };
	wxWxid.text = wxid;
	wxWxid.textLength = wcslen(wxid);
	wxWxid.textLen = wcslen(wxid) * 2;
	wxMessageStruct wxMessage = { 0 };
	wxMessage.text = message;
	wxMessage.textLength = wcslen(message);
	wxMessage.textLen = wcslen(message) * 2;
	char* pWxid = (char*)&wxWxid.text;
	char* pMsg = (char*)&wxMessage.text;
	char buff[0x798] = { 0 };
	__asm {
		mov edx, pWxid;
		push 0x1;
		push 0;
		mov ebx, pMsg;
		push ebx;
		lea ecx, buff;
		call sendCallAddress;
		add esp, 0xC;
	}
}