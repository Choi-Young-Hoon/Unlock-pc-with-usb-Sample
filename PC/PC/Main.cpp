#include "Pipe.h"
#include"WinUp.h"
#include"Hook����.h"
#include<Dbt.h>
#include<strsafe.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DisableKeys(int nCode, WPARAM wparam, LPARAM lparam);
char FirstDriveFromMask(ULONG unitmask);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("USB Control");


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, NULL);
	WndClass.hIcon = LoadCursor(NULL, NULL);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW || CS_VREDRAW || CS_DBLCLKS;
	RegisterClass(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszClass, WS_POPUPWINDOW, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	while (GetMessage(&Message, 0, 0, 0))
	{

		TranslateMessage(&Message);
		DispatchMessage(&Message);

	}
	return Message.wParam;
}
//
HANDLE SHThread = NULL;

//�������� ����
HHOOK g_hHook = NULL;			// �� �ڵ�
bool g_bActiva = false;			// �� �����찡 ��Ŀ�� ������

HANDLE winUpThread = NULL;		//������ �׻� ������ ��Ÿ���� �� �������� �ڵ� ����


HWND passWordEditText = NULL;	// USB�ν� �� ȭ�鿡 ��ȣ �Է� ���

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//����̹��� ������ ����
	char szMsg[80];
	
	//����̹� �ν� ���� ����
	PDEV_BROADCAST_HDR Drive = (PDEV_BROADCAST_HDR)lParam;
	PDEV_BROADCAST_VOLUME Drive2 = (PDEV_BROADCAST_VOLUME)Drive;
	
	//������ ���ڳѱ� ����	
	PVOID threadARGS[3] = { 0, };

	//��� ���� ����
	HDC hdc;
	PAINTSTRUCT ps;
	HFONT hFont,OldFont;

	switch (iMessage)
	{
	case WM_CREATE:
		//passWordEditText = CreateWindow(TEXT("Edit"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 100, 100, 100, 20, hWnd, (HMENU)10, g_hInst, NULL);

		//���α׷��� �׻� ������ �����ϰ� ���ִ� ������ ����
		winUpThread = CreateThread(
			NULL,	//���� �ĺ��ڸ� NULL�� �Ѱ��.
			0,		//0���� �Ѱܼ� ����Ʈ ���� ũ��(1M)�� �����ߴ�.
			WindowUpProc,	// ������ ������ ������ ��Ʈ�� �Լ��� ���� �����Ϳ�
			(PVOID)hWnd,// �Ѱ��� �Ű������� �����Ѵ�. �Ű������� 5000 ���� �־���.
			0,		//������ ���ÿ� ����ǵ��� �����Ѵ�
			0	// dwThreadID ������ �������� ID�� ����Ǿ� ��ȯ�ȴ�.
			);


		// ���� ������ �� �� ����Ѵ�
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, DisableKeys, g_hInst, 0);
		return 0;

		//USB �ν�//////////////////////////////////////////////////////////////
	case WM_DEVICECHANGE:
		
		//USB�� ����Ǿ��� ���
		if (wParam == 0x8000 && wParam != 0x8004)//���� ������  ����������쿡�� 0x0x8004
		{
			sprintf(szMsg, "%c:\\", FirstDriveFromMask(Drive2->dbcv_unitmask));
			//MessageBoxA(NULL, szMsg, "����", MB_OK);
			
			//������� �Ѱ��� ����
			threadARGS[0] = (PVOID)hWnd;
			threadARGS[1] = (PVOID)szMsg;
			threadARGS[2] = (PVOID)winUpThread;
			//������ ����
			SHThread = CreateThread(
				NULL,	//���� �ĺ��ڸ� NULL�� �Ѱ��.b
				0,		//0���� �Ѱܼ� ����Ʈ ���� ũ��(1M)�� �����ߴ�.
				HideThreadProc,	// ������ ������ ������ ��Ʈ�� �Լ��� ���� �����Ϳ�
				threadARGS,// �Ѱ��� �Ű������� �����Ѵ�. 
				0,		//������ ���ÿ� ����ǵ��� �����Ѵ�
				NULL);
		}


		//USB�� ���� �Ǿ��� ���
		if (wParam != 0x8000 && wParam == 0x8004)//���� ������  ����������쿡�� 0x0x8004
		{
			sprintf(szMsg, "%c:\\", FirstDriveFromMask(Drive2->dbcv_unitmask));
			//MessageBoxA(NULL, "�����˴ϴ�!", "����", MB_OK);

			//������� �Ѱ��� ����
			threadARGS[0] = (PVOID)hWnd;
			threadARGS[1] = (PVOID)szMsg;
			threadARGS[2] = (PVOID)winUpThread;
			if (SHThread != NULL)
			{
				//������ ����
				CreateThread(
					NULL,	//���� �ĺ��ڸ� NULL�� �Ѱ��.
					0,		//0���� �Ѱܼ� ����Ʈ ���� ũ��(1M)�� �����ߴ�.
					ShowThreadProc,	// ������ ������ ������ ��Ʈ�� �Լ��� ���� �����Ϳ�
					threadARGS,// �Ѱ��� �Ű������� �����Ѵ�. 
					0,		//������ ���ÿ� ����ǵ��� �����Ѵ�
					NULL);
			}
		}
		return 0;
		///////////////////////////////////////////////////////////////////////

	case WM_ACTIVATEAPP:
		g_bActiva = (bool)wParam;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hFont = CreateFont(250, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, FF_ROMAN, TEXT("FONT"));
		
		OldFont = (HFONT)SelectObject(hdc, hFont);
		TextOutA(hdc, GetSystemMetrics(SM_CXSCREEN)/4-100, GetSystemMetrics(SM_CYSCREEN)/4, "PC ���", strlen("PC ���"));
		SelectObject(hdc, OldFont);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:   
		// ������ �� Ű��ŷ ����
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
		

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


//�޾ƿ� ������ � ����̺����� ���ؼ� ��ȯ
char FirstDriveFromMask(ULONG unitmask)
{
	char i;

	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask = unitmask >> 1;
	}

	return(i + 'A');
}

