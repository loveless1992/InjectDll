#pragma once
#include "pch.h"
#include"resource.h"
#include <Windows.h>
#include <stdio.h>
#include "commctrl.h"
#include <TlHelp32.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <map>
#include <iostream>

DWORD ThreadProc(HMODULE hModule);
VOID HookWechatQrcode(HWND hwndDlg, DWORD HookAdd);
INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);