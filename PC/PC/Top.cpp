#include<windows.h>

DWORD WINAPI WindowUpProc(PVOID pParam)
{
	HWND hWnd = (HWND)pParam;
	while (1)
	{
		Sleep(1000);
		//항상위 설정
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		//활성화 소스
		SetForegroundWindow(hWnd);
	}
}