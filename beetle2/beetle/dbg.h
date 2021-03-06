#pragma once

enum LOGLEVEL{
	VERROR=0,
	VWARNING = 1,
	VINFO=2,
	VDEBUG=3,
	VTRACE=4,
};



#if _DEBUG
	void dbg(int loglevel, char *fmt, ...);
	void dbg(int loglevel, wchar_t *fmt, ...);
	#define dbglog() dbg(VTRACE,"%u:%s",__LINE__,__FUNCTION__)
#else
	#define dbg(loglevel,fmt,...)
#endif
