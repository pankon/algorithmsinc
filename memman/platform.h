/*
 * Platform determination
 */


#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/*
 * https://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#if defined(unix) || defined(__unix__) || defined(__unix)
#define PREDEF_PLATFORM_UNIX
#elif defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#define PREDEF_PLATFORM_WINDOWS
#endif


#endif /* __PLATFORM_H__ */