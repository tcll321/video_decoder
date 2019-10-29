#pragma once
#include <time.h>
#include <sysinfoapi.h>

/* 最小UTC LTMSEL时间，GMT: 1970/01/01 00:00:00; BEIJING: 1970/01/01 08:00:00 */
#define MIN_UTC_MSEL			((__int64)0)
/* 最大UTC LTMSEL时间，GMT: 2038/01/18 16:00:00; BEIJING: 2038/01/19 00:00:00 */
#define MAX_UTC_MSEL			((__int64)0x7fff6200 * 1000)

/* 非法UTC LTMSEL时间 */
#define INVALID_UTC_MSEL		((__int64)-1)

#ifdef __cplusplus
#define REF_OPERATER		&
#define DEFAULT_PARAM(v)	= v
#else
#define REF_OPERATER
#define DEFAULT_PARAM(v)
#if defined(_MSC_VER)
#define inline				static __inline
#elif defined(_NP_IS_MIPS_ARM)
#define inline				static inline
#endif
#endif

__int64 GetMsel();
__int64 StrToMsel(LPCTSTR st, BOOL msel = true, BOOL isLocal = true);

inline BOOL IsLeapYear(int year)
{
	return (0 == year % 4 && 0 != year % 100) || 0 == year % 400;
}

inline int GetMonthDays(int year, int month)
{
	int month_days;
	static int days_inited = 0;
	static int days[13];
	if (!days_inited)
	{
		days[0] = 0;
		days[1] = 31;	/* 1 */
		days[2] = 28;	/* 2 */
		days[3] = 31;	/* 3 */
		days[4] = 30;	/* 4 */
		days[5] = 31;	/* 5 */
		days[6] = 30;	/* 6 */
		days[7] = 31;	/* 7 */
		days[8] = 31;	/* 8 */
		days[9] = 30;	/* 9 */
		days[10] = 31;	/* 10 */
		days[11] = 30;	/* 11 */
		days[12] = 31;	/* 12 */
		days_inited = 1;
	}
	if (month < 1 || month>12)
		return 0;
	month_days = (2 == month
		? (IsLeapYear(year) ? 29 : 28)
		: days[month]);
	return month_days;
}

inline BOOL IsValidDate(int year, int month, int day)
{
	int month_days = GetMonthDays(year, month);
	if (0 == month_days)
		return false;
	if (day<1 || day>month_days)
		return false;
	return true;
}

inline BOOL IsValidMsel(__int64 lt)
{
	return MIN_UTC_MSEL <= lt && lt <= MAX_UTC_MSEL;
}

inline BOOL ToTimeEx(__int64 lt, SYSTEMTIME* st, BOOL toLocal)
{
	if (st)
	{
		time_t t;
		struct tm ltm;

		if (!IsValidMsel(lt))
		{
			return false;
		}

		t = (time_t)(lt / 1000);
		if (toLocal)
			localtime_s(&ltm, &t);
		else
			gmtime_s(&ltm, &t);
		st->wYear = (WORD)(ltm.tm_year + 1900);
		st->wMonth = (WORD)(ltm.tm_mon + 1);
		st->wDay = (WORD)ltm.tm_mday;
		st->wHour = (WORD)ltm.tm_hour;
		st->wMinute = (WORD)ltm.tm_min;
		st->wSecond = (WORD)ltm.tm_sec;
		st->wDayOfWeek = (WORD)ltm.tm_wday; /* Sunday is 0, Monday is 1, and so on. */
		st->wMilliseconds = (WORD)(lt % 1000);
		return true;
	}
	return false;
}

inline SYSTEMTIME ToTime(__int64 lt, BOOL toLocal)
{
	SYSTEMTIME st;
	if (!ToTimeEx(lt, &st, toLocal))
		memset(&st, 0, sizeof(st));
	return st;
}

inline __int64 TimeWeight(int year, int month, int day, int hour, int minute, int second, int milli)
{
	__int64 lt;
	if (!IsValidDate(year, month, day)
		|| 0 <= hour && hour <= 23
		|| 0 <= minute && minute <= 59
		|| 0 <= second && second <= 59
		|| 0 <= milli && milli <= 999)
		return -1;

	lt = (__int64)year * 12;		/* 月数 */
	lt = (lt + (month - 1)) * 31;	/* 天数 */
	lt = (lt + (day - 1)) * 24;	/* 小时数 */
	lt = (lt + hour) * 60;		/* 分钟数 */
	lt = (lt + minute) * 60;	/* 秒数 */
	lt = (lt + second) * 1000;	/* 毫秒数 */
	return lt + milli;			/* 得到最终时间权值毫秒数 */
}

inline int TimeRangeType(const SYSTEMTIME REF_OPERATER st, BOOL isLocal)
{
	static __int64 ltSysMin = MIN_UTC_MSEL;
	static __int64 ltSysMax = MIN_UTC_MSEL;
	static __int64 ltLocMin = MIN_UTC_MSEL;
	static __int64 ltLocMax = MIN_UTC_MSEL;
	__int64 lt;

	if (MIN_UTC_MSEL == ltSysMin)
	{
		SYSTEMTIME tst;

		tst = ToTime(MIN_UTC_MSEL, false);
		ltSysMin = TimeWeight(tst.wYear, tst.wMonth, tst.wDay, tst.wHour, tst.wMinute, tst.wSecond, tst.wMilliseconds);

		tst = ToTime(MAX_UTC_MSEL, false);
		ltSysMax = TimeWeight(tst.wYear, tst.wMonth, tst.wDay, tst.wHour, tst.wMinute, tst.wSecond, tst.wMilliseconds);

		tst = ToTime(MIN_UTC_MSEL, true);
		ltLocMin = TimeWeight(tst.wYear, tst.wMonth, tst.wDay, tst.wHour, tst.wMinute, tst.wSecond, tst.wMilliseconds);

		tst = ToTime(MAX_UTC_MSEL, true);
		ltLocMax = TimeWeight(tst.wYear, tst.wMonth, tst.wDay, tst.wHour, tst.wMinute, tst.wSecond, tst.wMilliseconds);
	}

	lt = TimeWeight(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	if (isLocal)
	{
		if (lt < ltLocMin)
			return -1;
		if (lt > ltLocMax)
			return 1;
		return 0;
	}

	if (lt < ltSysMin)
		return -1;
	if (lt > ltSysMax)
		return 1;
	return 0;
}

inline BOOL IsValidTime(const SYSTEMTIME REF_OPERATER st, BOOL isLocal, BOOL checkWeek)
{
	return IsValidDate(st.wYear, st.wMonth, st.wDay)
		&& /*0<=st.wHour && */st.wHour <= 23
		&& /*0<=st.wMinute && */st.wMinute <= 59
		&& /*0<=st.wSecond && */st.wSecond <= 59
		&& /*0<=st.wMilliseconds && */st.wMilliseconds <= 999
		&& 0 == TimeRangeType(st, isLocal)
		&& (!checkWeek || /*0<=st.wDayOfWeek && */st.wDayOfWeek <= 6);
}

inline __int64 GetMsel()
{
	__int64 ft;
	GetSystemTimeAsFileTime((FILETIME*)&ft);
	/* 116444736000000000i64: Number of 100 nanosecond units from 1/1/1601 to 1/1/1970 */
	return (ft - (116444736000000000i64 + 5000i64)) / 10000i64;
}

// inline __int64 GetTimeZoneMsel()
// {
// 	struct _timeb b;
// 	_ftime_s(&b);
// 	return (__int64)(-b.timezone) * 60 * 1000;
// }

inline __int64 ToMsel(const SYSTEMTIME REF_OPERATER st, BOOL isLocal)
{
	if (IsValidTime(st, isLocal, false/*true*/)/* && 0 == TimeRangeType(st, !isLocal)*/)
	{
		__int64 lt;
		struct tm atm;
		atm.tm_sec = st.wSecond;
		atm.tm_min = st.wMinute;
		atm.tm_hour = st.wHour;
		atm.tm_mday = st.wDay;
		atm.tm_mon = st.wMonth - 1;		/* tm_mon is 0 based */
		atm.tm_year = st.wYear - 1900;	/* tm_year is 1900 based */
		atm.tm_isdst = -1;
		lt = (__int64)mktime(&atm);
// 		if(lt != -1);				/* indicates an illegal input time */
		lt = lt * 1000 + st.wMilliseconds;
		if (isLocal)
			return lt;
		return lt;// +GetTimeZoneMsel();
	}
	return INVALID_UTC_MSEL;
}

inline __int64 StrToMsel(LPCTSTR st, BOOL msel, BOOL isLocal)
{
	TCHAR buf[20];
	int len = msel ? 17 : 14;
	_tcsncpy(buf, st, len);
	if (!msel)
	{
		buf[len] = buf[len + 1] = buf[len + 2] = '0';
		len += 3;
	}
	buf[len] = 0;

	int year, month, day, hour, minute, second, millisecond;
	_stscanf(buf, _T("%4d%2d%2d%2d%2d%2d%3d"), &year, &month, &day, &hour, &minute, &second, &millisecond);

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif
	SYSTEMTIME t = { (WORD)year, (WORD)month, 0, (WORD)day, (WORD)hour, (WORD)minute, (WORD)second, (WORD)millisecond };
#ifdef _MSC_VER
#pragma warning(error: 4244)
#endif
	if (!IsValidTime(t, isLocal, true))
		return INVALID_UTC_MSEL;

	return ToMsel(t, isLocal);

}
