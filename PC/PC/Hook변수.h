#pragma once
#include<Windows.h>

//전역변수 선언
extern HHOOK g_hHook;			// 훅 핸들
extern bool g_bActiva;			// 내 윈도우가 포커스 중인지

LRESULT CALLBACK DisableKeys(int nCode, WPARAM wparam, LPARAM lparam);