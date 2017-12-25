#ifndef _APP_UTIL_H_
#define _APP_UTIL_H_

#include <Windows.h>
#include <xnamath.h>

template<typename T>
void SafeRelease(T t)
{
	if(t)
	{
		t->Release();
		t = NULL;
	}
}

template<typename T>
T Clamp(T vMin, T vMax, T value)
{
	value = max(vMin,value);
	value = min(vMax,value);

	return value;
}

inline int KeyDown(int vKey)
{
	return GetAsyncKeyState(vKey) & 0x8000;
}

namespace Colors
{
	//定义几个常见的颜色值，方便在程序中使用
	const XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
	const XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
	const XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
	const XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
	const XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 1.0f};
	const XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
	const XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
	const XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};
	const XMVECTORF32 Silver	= {0.75f,0.75f,0.75f,1.0f};
};


#endif