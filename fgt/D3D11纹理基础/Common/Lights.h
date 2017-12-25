#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <Windows.h>
#include <xnamath.h>

namespace Lights
{
	//这些光源类型定义与Effect中对应光源的定义严格对应
	
	//平行光
	struct DirLight
	{
		XMFLOAT4	ambient;	//环境光
		XMFLOAT4	diffuse;	//漫反射光
		XMFLOAT4	specular;	//高光

		XMFLOAT3	dir;		//光照方向
		float		unused;		//用于与HLSL中"4D向量"对齐规则匹配
	};

	//点光源
	struct PointLight
	{
		XMFLOAT4	ambient;	//环境光
		XMFLOAT4	diffuse;	//漫反射光
		XMFLOAT4	specular;	//高光

		XMFLOAT3	pos;		//光源位置
		float		range;		//光照范围

		XMFLOAT3	att;		//衰减系数
		float		unused;		//用于与HLSL中"4D向量"对齐规则匹配
	};

	//聚光灯
	struct SpotLight
	{
		XMFLOAT4	ambient;	//环境光
		XMFLOAT4	diffuse;	//漫反射光
		XMFLOAT4	specular;	//高光

		XMFLOAT3	dir;		//光照方向
		float		range;		//光照范围

		XMFLOAT3	pos;		//光源位置
		float		spot;		//聚光强度系数

		XMFLOAT3	att;		//衰减系数
		float		theta;		//最大发散角度
	};

	//材质
	struct Material
	{
		XMFLOAT4	ambient;
		XMFLOAT4	diffuse;
		XMFLOAT4	specular;	//第4个元素为材质的镜面反射系数，即代表材质表面的光滑程度
	};
};

#endif