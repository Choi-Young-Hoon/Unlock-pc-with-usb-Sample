#include "Pipe.h"
#include"WinUp.h"
#include"Hook변수.h"
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

//전역변수 선언
HHOOK g_hHook = NULL;			// 훅 핸들
bool g_bActiva = false;			// 내 윈도우가 포커스 중인지

HANDLE winUpThread = NULL;		//윈도우 항상 맨위에 나타나게 할 스레드의 핸들 저장


HWND passWordEditText = NULL;	// USB인식 시 화면에 암호 입력 출력

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//드라이버명 저장할 변수
	char szMsg[80];
	
	//드라이버 인식 관련 변수
	PDEV_BROADCAST_HDR Drive = (PDEV_BROADCAST_HDR)lParam;
	PDEV_BROADCAST_VOLUME Drive2 = (PDEV_BROADCAST_VOLUME)Drive;
	
	//쓰레드 인자넘길 변수	
	PVOID threadARGS[3] = { 0, };

	//출력 관련 변수
	HDC hdc;
	PAINTSTRUCT ps;
	HFONT hFont,OldFont;

	switch (iMessage)
	{
	case WM_CREATE:
		//passWordEditText = CreateWindow(TEXT("Edit"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 100, 100, 100, 20, hWnd, (HMENU)10, g_hInst, NULL);

		//프로그램이 항상 맨위에 존재하게 해주는 쓰레드 실행
		winUpThread = CreateThread(
			NULL,	//보안 식별자를 NULL로 넘겼다.
			0,		//0으로 넘겨서 디폴트 스택 크기(1M)를 지정했다.
			WindowUpProc,	// 유저가 정의한 스레드 엔트리 함수에 대한 포인터와
			(PVOID)hWnd,// 넘겨줄 매개변수를 지정한다. 매개변수는 5000 값을 주었다.
			0,		//생성과 동시에 실행되도록 설정한다
			0	// dwThreadID 변수에 스레드의 ID가 저장되어 반환된다.
			);


		// 윈도 생성할 때 훅 등록한다
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, DisableKeys, g_hInst, 0);
		return 0;

		//USB 인식//////////////////////////////////////////////////////////////
	case WM_DEVICECHANGE:
		
		//USB가 연결되었을 경우
		if (wParam == 0x8000 && wParam != 0x8004)//연결 됬을때  해제됬을경우에는 0x0x8004
		{
			sprintf(szMsg, "%c:\\", FirstDriveFromMask(Drive2->dbcv_unitmask));
			//MessageBoxA(NULL, szMsg, "연결", MB_OK);
			
			//스레드로 넘겨줄 인자
			threadARGS[0] = (PVOID)hWnd;
			threadARGS[1] = (PVOID)szMsg;
			threadARGS[2] = (PVOID)winUpThread;
			//스레드 생성
			SHThread = CreateThread(
				NULL,	//보안 식별자를 NULL로 넘겼다.b
				0,		//0으로 넘겨서 디폴트 스택 크기(1M)를 지정했다.
				HideThreadProc,	// 유저가 정의한 스레드 엔트리 함수에 대한 포인터와
				threadARGS,// 넘겨줄 매개변수를 지정한다. 
				0,		//생성과 동시에 실행되도록 설정한다
				NULL);
		}


		//USB가 해제 되었을 경우
		if (wParam != 0x8000 && wParam == 0x8004)//연결 됬을때  해제됬을경우에는 0x0x8004
		{
			sprintf(szMsg, "%c:\\", FirstDriveFromMask(Drive2->dbcv_unitmask));
			//MessageBoxA(NULL, "해제됩니다!", "해제", MB_OK);

			//스레드로 넘겨줄 인자
			threadARGS[0] = (PVOID)hWnd;
			threadARGS[1] = (PVOID)szMsg;
			threadARGS[2] = (PVOID)winUpThread;
			if (SHThread != NULL)
			{
				//스레드 생성
				CreateThread(
					NULL,	//보안 식별자를 NULL로 넘겼다.
					0,		//0으로 넘겨서 디폴트 스택 크기(1M)를 지정했다.
					ShowThreadProc,	// 유저가 정의한 스레드 엔트리 함수에 대한 포인터와
					threadARGS,// 넘겨줄 매개변수를 지정한다. 
					0,		//생성과 동시에 실행되도록 설정한다
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
		TextOutA(hdc, GetSystemMetrics(SM_CXSCREEN)/4-100, GetSystemMetrics(SM_CYSCREEN)/4, "PC 잠금", strlen("PC 잠금"));
		SelectObject(hdc, OldFont);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:   
		// 종료할 때 키후킹 종료
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
		

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


//받아온 값에서 어떤 드라이브인지 구해서 반환
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

