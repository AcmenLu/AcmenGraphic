#include "DirectXinit.h"
#include <stdlib.h>

DirectXinit* inits = 0;
void test( )
{
	inits->Onrender( );
}
int main( )
{
	inits = new DirectXinit( );
	test( );
	system("pause");
}