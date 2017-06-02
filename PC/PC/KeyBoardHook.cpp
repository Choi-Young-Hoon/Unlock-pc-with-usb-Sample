#include"Hook변수.h"
// 후킹 콜백
LRESULT CALLBACK DisableKeys(int nCode, WPARAM wparam, LPARAM lparam)
{
	KBDLLHOOKSTRUCT *pkh = (KBDLLHOOKSTRUCT*)lparam;
	if (nCode == HC_ACTION)
	{
		if (g_bActiva)
		{
			BOOL bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT)* 8) - 1);

			if ((pkh->vkCode == VK_ESCAPE && bCtrlKeyDown))
			{
				//MessageBoxA(NULL, ("Ctrl + Esc\n"), "TEST", MB_OK);
				return 1;
			}
			if (pkh->vkCode == VK_TAB && pkh->flags & LLKHF_ALTDOWN)
			{
				//MessageBoxA(NULL, ("Alt + Tab\n"), "TEST", MB_OK);
				return 1;
			}
			if (pkh->vkCode == VK_ESCAPE && pkh->flags & LLKHF_ALTDOWN)
			{
				//MessageBoxA(NULL, ("Alt + Esc\n"), "TEST", MB_OK);
				return 1;
			}
			if (pkh->vkCode == VK_LWIN || pkh->vkCode == VK_RWIN)
			{
				//MessageBoxA(NULL, ("Start Menu\n"), "TEST", MB_OK);
				return 1;
			}
			if (pkh->vkCode == VK_F4 && pkh->flags & LLKHF_ALTDOWN)
			{
				return 1;
			}

		}
	}

	return CallNextHookEx(g_hHook, nCode, wparam, lparam);
}