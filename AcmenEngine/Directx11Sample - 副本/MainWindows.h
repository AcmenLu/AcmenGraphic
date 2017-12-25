#ifndef _MAINWINDOWS_H
#define _MAINWINDOWS_H
#include <Windows.h>
#include "Utils.h"
#include "GameTimer.h"

class MainWindows
{
public:
	typedef void (*OnIdle)( float elapse);

private:
	HINSTANCE	mHInstance;
	HWND		mWindows;
	char*		mWinName;
	int			mClientWidth;
	int			mClientHeight;
	OnIdle		mIdleFunc;
	GameTimer	mTimer;

private:
	static long __stdcall MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

public:
	MainWindows( HINSTANCE hInstance,
		char* winName = "AcmenEngine",
		int width = 800, int height = 600 )
		:mHInstance( hInstance ), mWinName( winName ),
		mClientWidth( width ), mClientHeight( height ),
		mIdleFunc( 0 )
	{

	}

	~MainWindows( ){ }
	
	inline int GetWindowsWidth( )
		{ return mClientWidth; }

	inline int GetWindowsHeight( )
		{ return mClientHeight; }

	inline HWND GetWindows( )
		{ return mWindows; }

	inline HINSTANCE GetInstance( )
		{ return mHInstance; }

	inline void SetOnIdleCallBack( OnIdle funcprt )
		{ mIdleFunc = funcprt; }

	inline float AspectRatio( )const
		{ return static_cast<float>( mClientWidth ) / mClientHeight; }

	bool InitMainWindow( );
	void MessageLoop( );
};

#endif