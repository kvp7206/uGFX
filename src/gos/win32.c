/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    src/gos/chibios.c
 * @brief   GOS ChibiOS Operating System support.
 */
#include "gfx.h"

#if GFX_USE_OS_WIN32

#include <stdio.h>

static HANDLE	SystemMutex;

void _gosInit(void) {
}

void gfxHalt(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	ExitProcess(1);
}

void gfxSleepMicroseconds(delaytime_t ms) {
    static LARGE_INTEGER pcfreq;
    static int initflag;
    LARGE_INTEGER t1, t2, tdiff;

    switch(ms) {
	case TIME_IMMEDIATE:	return;
	case TIME_INFINITE:		while(1) Sleep(1000);	return;
	}

    if (!initflag) {
        QueryPerformanceFrequency(&pcfreq);
        initflag++;
    }
    tdiff.QuadPart = pcfreq.QuadPart * ms / 1000000;

    QueryPerformanceCounter(&t1);
    do {
    	QueryPerformanceCounter(&t2);
    } while (t2.QuadPart - t1.QuadPart < tdiff.QuadPart);
}

void gfxSystemLock(void) {
	if (!SystemMutex)
		SystemMutex = CreateMutex(NULL, FALSE, NULL);
	WaitForSingleObject(SystemMutex, INFINITE);
}

void gfxSystemUnlock(void) {
	ReleaseMutex(SystemMutex);
}

bool_t gfxSemWait(gfxSem *psem, delaytime_t ms) {
	return WaitForSingleObject(*psem, ms) == WAIT_OBJECT_0;
}

typedef LONG __stdcall (*_NtQuerySemaphore)(
    HANDLE SemaphoreHandle,
    DWORD SemaphoreInformationClass, /* Would be SEMAPHORE_INFORMATION_CLASS */
    PVOID SemaphoreInformation,      /* but this is to much to dump here     */
    ULONG SemaphoreInformationLength,
    PULONG ReturnLength OPTIONAL
);

semcount_t gfxSemCounter(gfxSem *pSem) {
	static _NtQuerySemaphore NtQuerySemaphore;
	struct _SEMAPHORE_BASIC_INFORMATION {
	    ULONG CurrentCount;
	    ULONG MaximumCount;
	} BasicInfo;

    if (!NtQuerySemaphore)
    	NtQuerySemaphore = (_NtQuerySemaphore)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySemaphore");

    NtQuerySemaphore(*pSem, 0, &BasicInfo, sizeof(BasicInfo), NULL);
    return BasicInfo.CurrentCount;
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param) {
	(void)	stackarea;
	HANDLE	thd;

	if (!(thd = CreateThread(NULL, stacksz, fn, param, CREATE_SUSPENDED, NULL)))
		return FALSE;
	SetThreadPriority(thd, prio);
	ResumeThread(thd);
	return thd;
}

threadreturn_t gfxThreadWait(gfxThreadHandle thread) {
	DWORD	ret;

	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &ret);
	CloseHandle(thread);
	return ret;
}

#endif /* GFX_USE_OS_WIN32 */
/** @} */
