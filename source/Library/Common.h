#pragma once
#include <windows.h>
#include <exception>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "D3D11.h"
#include <dinput.h>
#undef _HAS_STD_BYTE

#define DeleteObject(object) if((object) != NULL) { delete object;object = NULL; }
#define DeleteObjects(objects) if((objects) != NULL) { delete[] objects; objects = NULL; }
#define ReleaseObject(object) if((object) != NULL) { object->Release();object = NULL; }
namespace Library
{
	typedef unsigned char BYTE;
}
using namespace DirectX;
using namespace DirectX::PackedVector;