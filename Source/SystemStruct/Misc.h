#pragma once
#define _WINSOCKAPI_  // windows.hを定義した際に、winsock.hを自動的にインクルードしない
#include <windows.h>
#include <crtdbg.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline LPWSTR HRTrace(HRESULT hr)
{
    LPWSTR msg;
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg), 0, NULL);
    return msg;
}

class Benchmark
{
    LARGE_INTEGER ticksPerSecond_;
    LARGE_INTEGER startTicks_;
    LARGE_INTEGER currentTicks_;

public:
    Benchmark()
    {
        QueryPerformanceFrequency(&ticksPerSecond_);
        QueryPerformanceCounter(&startTicks_);
        QueryPerformanceCounter(&currentTicks_);
    }
    void begin()
    {
        QueryPerformanceCounter(&startTicks_);
    }
    float end()
    {
        QueryPerformanceCounter(&currentTicks_);
        return static_cast<float>(currentTicks_.QuadPart - startTicks_.QuadPart) / static_cast<float>(ticksPerSecond_.QuadPart);
    }
};