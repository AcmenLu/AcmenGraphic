#include "DirectXBase.h"

DirectXBase::DirectXBase()
{
	std::cout<<"DirectXBase::DirectXBase()" <<std::endl;
}

DirectXBase::~DirectXBase()
{
	std::cout<<"DirectXBase::~DirectXBase()" <<std::endl;
}

bool DirectXBase::Onrender()
{
	std::cout<<"DirectXBase::Onrender()" <<std::endl;
	return true;
}

void DirectXBase::SetUp( )
{
	std::cout<<"DirectXBase::SetUp()" <<std::endl;
}