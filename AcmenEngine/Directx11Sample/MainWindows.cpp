
#include "MainWindows.h"

long MainWindows::MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

bool MainWindows::InitMainWindow( )
{
	WNDCLASSEX wclass;
	wclass.cbClsExtra		= 0;
	wclass.cbSize			= sizeof( WNDCLASSEX );
	wclass.cbWndExtra		= 0;
	wclass.hbrBackground	= ( HBRUSH )GetStockObject( BLACK_BRUSH );
	wclass.hCursor			= LoadCursor( NULL,IDC_ARROW);
	wclass.hIcon			= LoadIcon( NULL, IDI_WINLOGO );
	wclass.hIconSm			= wclass.hIcon;
	wclass.hInstance		= mHInstance;
	wclass.lpfnWndProc		= MainWndProc;
	wclass.lpszClassName	= mWinName;
	wclass.lpszMenuName		= NULL;
	wclass.style			= CS_HREDRAW | CS_VREDRAW;

	if( !RegisterClassEx( &wclass ) )
	{
		MessageBox( 0, "RegisterClass Failed.", 0, 0 );
		return false;
	}

	RECT rect = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, false );
	int width  = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	mWindows = CreateWindowEx( WS_EX_APPWINDOW, mWinName,
			mWinName, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,
			CW_USEDEFAULT, width, height, NULL,
			NULL, mHInstance, NULL ); 
	if( !mWindows )
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0 );
		return false;
	}

	ShowWindow( mWindows, SW_SHOW );
	UpdateWindow( mWindows );

	return true;
}

void MainWindows::MessageLoop( )
{
	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0, 0,PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if ( *mIdleFunc )
				mIdleFunc( 0 );
		}
	}
}