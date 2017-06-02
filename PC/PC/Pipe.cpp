#include<Windows.h>
#include<stdio.h>
#include<tchar.h>

DWORD WINAPI HideThreadProc(PVOID pParam)
{
	PVOID *args = (PVOID *)pParam;
	char driverPath[70] = { 0, };
	HWND hwndMainWindow = (HWND)args[0];
	char upcText[100] = { 0, };
	strcpy(driverPath, (char*)args[1]);

	sprintf_s(driverPath, "%sUPC.aof", driverPath);

	FILE *UPC = fopen(driverPath, "r");
	if (UPC == NULL)
		return 0;

	char accessUPC[] = "AccessUserMakeAccessUserMakeAccessUserMakeAccessUserMakeAccessUserMakeAccessUserMakeAccessUserMakeAccessUserMake";
	if (fread(upcText, 1, 100, UPC) != 0)
	{
		if (strncmp(upcText, accessUPC, strlen(accessUPC)))
		{

			SuspendThread((HANDLE)args[2]);
			
			if (!SetWindowPos(hwndMainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE) || !SetWindowPos(hwndMainWindow,HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE))
				MessageBox(NULL, TEXT("ERROR!"), TEXT("ERROR!"), MB_OK);

			ShowWindow(hwndMainWindow, SW_HIDE);
			FILE *accessFile = fopen("access", "w+");
			fprintf(accessFile, "Access123");
			fclose(accessFile);
		}
	}

	fclose(UPC);

	return 0;
}


DWORD WINAPI ShowThreadProc(PVOID pParam)
{
	PVOID *args = (PVOID *)pParam;
	HWND hwndMainWindow = (HWND)args[0];
	char upcText[100] = { 0, };
	FILE *UPC = fopen("access", "r");
	if (UPC == NULL)
		return 0;

	if (fread(upcText, 1, 1000, UPC) != 0)
	{
		if (!strncmp(upcText, "Access123", 9))
		{
			ResumeThread((HANDLE)args[2]);
			ShowWindow(hwndMainWindow, SW_SHOW);
		}
	}
	fclose(UPC);
	DeleteFile(_T("access"));
	return 0;
}