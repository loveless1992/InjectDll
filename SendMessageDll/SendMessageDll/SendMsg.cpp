#include "pch.h"
#include "dllmain.h"

struct wxStr
{
	wchar_t* pStr;
	int straLen;
	int straPerLen;
};


//��ȡģ���ַ
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}


VOID sendTextMessage(wchar_t* wxid,wchar_t* msg)
{
	//������ϢCALL
	DWORD sendCall = getWeChatWin() + 0x1343BA;
	//��װ΢��ID����
	wxStr pWxID = { 0 };
	pWxID.pStr = wxid;
	pWxID.straLen = wcslen(wxid);//wcslen��ȡ���ַ�������
	pWxID.straPerLen = wcslen(wxid) * 2;
	//��װ�ı�����
	wxStr pWxMsg = { 0 };
	pWxMsg.pStr = msg;
	pWxMsg.straLen = wcslen(msg);//wcslen��ȡ���ַ�������
	pWxMsg.straPerLen = wcslen(msg) * 2;

	//��ֵ
	//mov ecx, dword ptr ds : [esi + 0xB60]
	//lea eax, dword ptr ss : [ebp - 0x24]
	//push eax
	//add ecx, 0x1528
	//call WeChatWi.78BFECB0

	char* asmWxID = (char*)&pWxID.pStr;
	char* asmWxMsg = (char*)&pWxMsg.pStr;
	_asm
	{
		mov ecx, asmWxID
		lea eax, dword ptr ss : [ebp - 0x24]
	}



}