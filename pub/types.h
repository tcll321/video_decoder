#ifndef _TYPES_H_
#define _TYPES_H_


#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifdef WIN32
#include <WinBase.h>
#include <sysinfoapi.h>
#endif

typedef HANDLE			OSThread;
typedef unsigned long	OSThreadID;


OSThreadID OSThreadSelf();

#pragma warning(disable: 4996)

/* 获取Windows版本信息 */
typedef enum WindowsVersion__
{
	Invalid_WinVer = -1,
	Unknown_WinVer = 0,
	Win32s,
	Windows_95_98,
	Windows_NT,
	Windows_2000,
	Windows_XP,
	Windows_Longhorn,
	Windows_Vista,
	Windows_7,
	Windows_8,
} WindowsVersion;
WindowsVersion GetWindowsVersion();
const char* GetWindowsVersionText();


inline WindowsVersion GetWindowsVersion()
{
	static WindowsVersion sWinVer = Invalid_WinVer;
	if (Invalid_WinVer != sWinVer)
		return sWinVer;

#if defined(_WINBASE_)
	{
		DWORD ver;
		OSVERSIONINFOEX osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx((OSVERSIONINFO*)&osvi))
		{
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if (!GetVersionEx((OSVERSIONINFO*)&osvi))
				return (sWinVer = Unknown_WinVer);
		}

#if defined(_MSC_VER) && (_MSC_VER >= 1500) /* >=VC2008 */
		if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
		{
			if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return (sWinVer = Windows_Vista);
				else
					return (sWinVer = Windows_Longhorn);
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
				return (sWinVer = Windows_8);
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
				return (sWinVer = Windows_7);
			else if (osvi.dwMajorVersion == 5)
			{
				if (osvi.dwMinorVersion == 0)
					return (sWinVer = Windows_2000);
				else
					return (sWinVer = Windows_XP);
			}
			return (sWinVer = Windows_NT);
		}
#endif

		ver = GetVersion();

		if (ver < 0x80000000)
		{
			if ((WORD)ver == 0x0206)
				return (sWinVer = Windows_8);
			else if ((WORD)ver == 0x0106)
				return (sWinVer = Windows_7);
			else if ((WORD)ver == 0x0006)
				return (sWinVer = Windows_Vista); /* 忽略 Windows_Longhorn */
			else if (osvi.dwMajorVersion == 5)
			{
				if (osvi.dwMinorVersion == 0)
					return (sWinVer = Windows_2000);
				else
					return (sWinVer = Windows_XP);
			}
			return (sWinVer = Windows_NT);
		}
		else if ((DWORD)(LOBYTE(LOWORD(ver))) < 4)
			return (sWinVer = Win32s);
		else
			return (sWinVer = Windows_95_98);
	}
#else
	return (sWinVer = Unknown_WinVer);
#endif
}

inline const char* GetWindowsVersionText()
{
	WindowsVersion ver = GetWindowsVersion();
	const char* verTexts[] =
	{
		"Unknown_WinVer",
		"Win32s",
		"Windows_95/98",
		"Windows_NT",
		"Windows_2000",
		"Windows_XP",
		"Windows_Vista",
		"Windows_Longhorn",
		"Windows_7",
		"Windows_8",
	};
	if (0 <= ver && ver < (int)(sizeof(verTexts) / sizeof(verTexts[0])))
		return verTexts[ver];
	return "Invalid_WinVer";
}



inline OSThreadID OSThreadSelf()
{
	return GetCurrentThreadId();
}

#endif