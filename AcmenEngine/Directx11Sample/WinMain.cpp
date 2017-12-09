#include "DirectXInit.h"

MainWindows* window = 0;
DirectXInit* directx = 0;

void OnIdle( float elpace )
{
	directx->OnRender( );
}

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	window = new MainWindows( hInstance, "AcmenWindow", 800, 600 );
	window->InitMainWindow( );
	directx = new DirectXInit( window );
	directx->InitDirectX3D( );
	window->SetOnIdleCallBack( OnIdle );
	window->MessageLoop( );
	return 0;
}
