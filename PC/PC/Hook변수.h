#pragma once
#include<Windows.h>

//�������� ����
extern HHOOK g_hHook;			// �� �ڵ�
extern bool g_bActiva;			// �� �����찡 ��Ŀ�� ������

LRESULT CALLBACK DisableKeys(int nCode, WPARAM wparam, LPARAM lparam);