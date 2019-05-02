#include "TriangleDemo.h"
#include "Game.h"
#include "GameException.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Utility.h"
#include "D3DCompiler.h"
#include "d3dUtil.h"
#include "DXTrace.h"
namespace Rendering
{

	RTTI_DEFINITIONS(TriangleDemo)
		TriangleDemo::TriangleDemo(Game& game, Camera& camera)
		: DrawableGameComponent(game, camera),
		/*	mEffect(nullptr), mTechnique(nullptr), mPass(nullptr),
			mWvpVariable(nullptr),*/
		mInputLayout(nullptr), mWorldMatrix(MatrixHelper::Identity),
		mVertexBuffer(nullptr)
	{
	}
	TriangleDemo::~TriangleDemo()
	{

		ReleaseObject(mInputLayout);
		ReleaseObject(mVertexBuffer);
	}
	void TriangleDemo::Initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		// ����������ɫ��
		// Compile the shader
		InitEffect();
		InitResource();
	
	}

	void TriangleDemo::InitEffect()
	{
		UINT shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ����������ɫ��
		ID3DBlob* blob = NULL;
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Triangle_VS.cso", L"Content\\HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", &blob));
		/*if (FAILED(hr))
		{
			throw GameException("CreateShaderFromFile() failed.", hr);
		}*/
		HRESULT hr2 = mGame->Direct3DDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader);
		if (FAILED(hr2))
		{
			throw GameException("CreateVertexShader() failed.", hr2);
		}
		// �������㲼��

		// �������㲼��
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		HR(GetGame()->Direct3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions),
			blob->GetBufferPointer(), blob->GetBufferSize(), &mInputLayout));
		// ����������ɫ��
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Triangle_PS.cso", L"Content\\HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", &blob));
		/*if (FAILED(hr))
		{
			throw GameException("CreateShaderFromFile() failed.", hr);
		}*/
		HRESULT hr = (mGame->Direct3DDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mPixelShader));
		if (FAILED(hr))
		{
			throw GameException("CreatePixelShader() failed.", hr);
		}
	}

	void TriangleDemo::InitResource()
	{
		// ******************
	// ���������嶥��
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3
		BasicEffectVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
		};
		// ���ö��㻺��������
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof vertices;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		// �½����㻺����
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&vbd, &InitData, &mVertexBuffer));

		// ******************
		// ��������
			WORD indices[] = {
			// ����
			0,3,
			3,7,
			7,4,
			4,0,
			1,5,
			5,6,
			6,2,
			2,1,
			1,0,
			2,3,
			6,7,
			5,4
		};
		//WORD indices[] = {
		//	// ����
		//	0, 1, 2,
		//	2, 3, 0,
		//	// ����
		//	4, 5, 1,
		//	1, 0, 4,
		//	// ����
		//	1, 5, 6,
		//	6, 2, 1,
		//	// ����
		//	7, 6, 5,
		//	5, 4, 7,
		//	// ����
		//	3, 2, 6,
		//	6, 7, 3,
		//	// ����
		//	4, 0, 3,
		//	3, 7, 4
		//};
		mIndexCount = sizeof(indices)/ sizeof(WORD);
		// ������������������
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof indices;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		// �½�����������
		InitData.pSysMem = indices;
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&ibd, &InitData, &mIndexBuffer));
		// ����װ��׶ε���������������
		GetGame()->Direct3DDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


		// ******************
		// ���ó�������������
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(FPerObject);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		// �½���������������ʹ�ó�ʼ����
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&cbd, nullptr,&mConstantBuffer));

		// ��ʼ��������������ֵ
		CBuffer.world = XMMatrixIdentity();	// ��λ�����ת����������
		CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
			XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		));
		CBuffer.proj = XMMatrixTranspose(mCamera->ProjectionMatrix());/*XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, GetCamera().AspectRatio(), 1.0f, 1000.0f));*/

		// ******************
		// ����Ⱦ���߸����׶ΰ󶨺�������Դ

		// ����װ��׶εĶ��㻺��������
		UINT stride = sizeof(BasicEffectVertex);	// ��Խ�ֽ���
		UINT offset = 0;						// ��ʼƫ����

		GetGame()->Direct3DDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		// ����ͼԪ���ͣ��趨���벼��
		GetGame()->Direct3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		GetGame()->Direct3DDeviceContext()->IASetInputLayout(mInputLayout);
		// ����ɫ���󶨵���Ⱦ����
		GetGame()->Direct3DDeviceContext()->VSSetShader(mVertexShader, nullptr, 0);
		// �����ºõĳ����������󶨵�������ɫ��
		GetGame()->Direct3DDeviceContext()->VSSetConstantBuffers(0, 1, &mConstantBuffer);

		GetGame()->Direct3DDeviceContext()->PSSetShader(mPixelShader, nullptr, 0);

	}

	void TriangleDemo::Update(const GameTime& gameTime)
	{
		DrawableGameComponent::Update(gameTime);
		static float phi = 0.0f, theta = 0.0f;
		phi += 0.0001f, theta += 0.00015f;
		CBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
		// ���³�������������������ת����
		D3D11_MAPPED_SUBRESOURCE mappedData;
		(mGame->Direct3DDeviceContext()->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(CBuffer), &CBuffer, sizeof(CBuffer));
		mGame->Direct3DDeviceContext()->Unmap(mConstantBuffer, 0);
	}

	void TriangleDemo::Draw(const GameTime& gameTime)
	{

		GetGame()->Direct3DDeviceContext()->DrawIndexed(mIndexCount, 0,0);

	}
}