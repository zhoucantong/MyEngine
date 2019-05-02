#pragma once
#include<d3d.h>
#include "D3DCompiler.h"
#include "Common.h"
#include <iostream>
#include<filesystem>

class d3dUtil
{
public:
	d3dUtil();
	~d3dUtil();

public:
	static HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
		LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut);
};

