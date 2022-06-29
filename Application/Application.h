#pragma once

#include "resource.h"
#include "XMessageBox.h"

#include <chrono>
#include <thread>

struct Vector4Int
{
    int w, x, y, z;
};

struct PARAMS
{
	HWND hwnd;
	LPCTSTR lpszMessage;
	LPCTSTR lpszCaption = NULL;
	UINT nStyle = MB_OK | MB_ICONEXCLAMATION;
	XMSGBOXPARAMS* pXMB = NULL;
};

void WaitAndSet(int sleepTime, bool* boolToSet)
{
    if (sleepTime > -1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
        *boolToSet = true;
    }
}