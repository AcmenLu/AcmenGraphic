#include "DirectXinit.h"

DirectXinit::DirectXinit()
{
	std::cout<<"DirectXinit::DirectXinit()" <<std::endl;
}

DirectXinit::~DirectXinit( )
{
	std::cout<<"DirectXinit::~DirectXinit()" <<std::endl;
}

bool DirectXinit::Onrender( )
{
	std::cout<<"DirectXinit::Onrender()" <<std::endl;
	return true;
}
