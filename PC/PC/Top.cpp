#include<windows.h>

DWORD WINAPI WindowUpProc(PVOID pParam)
{
	HWND hWnd = (HWND)pParam;
	while (1)
	{
		Sleep(1000);
		//�׻��� ����
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		//Ȱ��ȭ �ҽ�
		SetForegroundWindow(hWnd);
	}
}