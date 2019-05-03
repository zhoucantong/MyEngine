#include "TriangleDemo.h"
#include "Game.h"
#include "GameException.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Utility.h"
#include "D3DCompiler.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "DXTrace.h"
#include "Geometry.h"
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
		ReleaseObject(mPixelShader);

		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
		for (int i=0;i<2;i++)
		{
			ReleaseObject(mConstantBuffer[i]);
		}
		
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
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Light_VS.cso", L"Content\\HLSL\\Light_VS.hlsl", "VS", "vs_5_0", &blob));
		HRESULT hr2 = mGame->Direct3DDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader);
		if (FAILED(hr2))
		{
			throw GameException("CreateVertexShader() failed.", hr2);
		}

		// �������㲼��
		HR(GetGame()->Direct3DDevice()->CreateInputLayout(VertexPosNormalColor::inputLayout, ARRAYSIZE(VertexPosNormalColor::inputLayout),
			blob->GetBufferPointer(), blob->GetBufferSize(), &mInputLayout));
		// ����������ɫ��
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Light_PS.cso", L"Content\\HLSL\\Light_PS.hlsl", "PS", "ps_5_0", &blob));
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
		Geometry::MeshData<VertexPosNormalColor> BoxMesh = Geometry::CreateBox<VertexPosNormalColor>();
		// ******************
	// ���������嶥��
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3
	/*	BasicEffectVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
		};*/
		// ���ö��㻺��������
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof (VertexPosNormalColor)*BoxMesh.vertexVec.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		// �½����㻺����
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = BoxMesh.vertexVec.data();
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&vbd, &InitData, &mVertexBuffer));
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
		mIndexCount = (BoxMesh.indexVec.size());
		// ������������������
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof (WORD)*mIndexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		// �½�����������
		InitData.pSysMem = BoxMesh.indexVec.data();
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&ibd, &InitData, &mIndexBuffer));
		// ����װ��׶ε���������������



		{
			// ��ʼ��Ĭ�Ϲ���
		// �����
			m_DirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			m_DirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			m_DirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			m_DirLight.Direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
			// ���
			m_PointLight.Position = XMFLOAT3(0.0f, 0.0f, -10.0f);
			m_PointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
			m_PointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
			m_PointLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			m_PointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
			m_PointLight.Range = 25.0f;
			// �۹��
			m_SpotLight.Position = XMFLOAT3(0.0f, 0.0f, -5.0f);
			m_SpotLight.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
			m_SpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			m_SpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_SpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_SpotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
			m_SpotLight.Spot = 12.0f;
			m_SpotLight.Range = 10000.0f;
			// ��ʼ������VS�ĳ�����������ֵ
			m_VSConstantBuffer.world = XMMatrixIdentity();
			m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
				XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			));
			m_VSConstantBuffer.proj = XMMatrixTranspose(mCamera->ProjectionMatrix());
			m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();

			// ��ʼ������PS�ĳ�����������ֵ
			m_PSConstantBuffer.material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			m_PSConstantBuffer.material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_PSConstantBuffer.material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 5.0f);
			// ʹ��Ĭ��ƽ�й�
			m_PSConstantBuffer.dirLight = m_DirLight;
			// ע�ⲻҪ�������ô˴��Ĺ۲�λ�ã�����������ֻ�������
			m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);


			/*	{
					m_PSConstantBuffer.dirLight = m_DirLight;
					m_PSConstantBuffer.pointLight = PointLight();
					m_PSConstantBuffer.spotLight = SpotLight();

				}*/
			{
				m_PSConstantBuffer.dirLight = DirectionalLight();
				m_PSConstantBuffer.pointLight = m_PointLight;
				m_PSConstantBuffer.spotLight = SpotLight();
			}
			/*{
				m_PSConstantBuffer.dirLight = DirectionalLight();
				m_PSConstantBuffer.pointLight = PointLight();
				m_PSConstantBuffer.spotLight = m_SpotLight;
			}*/
		}
		// ******************
		// ���ó�������������
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(VSConstantBuffer);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		// �½���������������ʹ�ó�ʼ����
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&cbd, nullptr,&mConstantBuffer[0]));

		cbd.ByteWidth = sizeof(PSConstantBuffer);
		HR(GetGame()->Direct3DDevice()->CreateBuffer(&cbd, nullptr, &mConstantBuffer[1]));
		// ��ʼ��������������ֵ
		//CBuffer.world = XMMatrixIdentity();	// ��λ�����ת����������
		//CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
		//	XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		//	XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		//));
		//CBuffer.proj = XMMatrixTranspose(mCamera->ProjectionMatrix());

	}

	void TriangleDemo::Update(const GameTime& gameTime)
	{
		DrawableGameComponent::Update(gameTime);
		static float phi = 0.0f, theta = 0.0f;
		phi += 0.0001f, theta += 0.00015f;
		XMMATRIX W = XMMatrixRotationX(phi) * XMMatrixRotationY(theta);
		m_VSConstantBuffer.world = XMMatrixTranspose(W);
		m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, W);	// ����ת�ÿ��Ե���
		//// ���³�������������������ת����
		D3D11_MAPPED_SUBRESOURCE mappedData;
		(mGame->Direct3DDeviceContext()->Map(mConstantBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
		mGame->Direct3DDeviceContext()->Unmap(mConstantBuffer[0], 0);

		HR(mGame->Direct3DDeviceContext()->Map(mConstantBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
		(mGame->Direct3DDeviceContext()->Unmap(mConstantBuffer[1], 0));
	}

	void TriangleDemo::Draw(const GameTime& gameTime)
	{

		{
			// ����ͼԪ���ͣ��趨���벼��
			GetGame()->Direct3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // D3D11_PRIMITIVE_TOPOLOGY_LINELIST
			GetGame()->Direct3DDeviceContext()->IASetInputLayout(mInputLayout);
			// ����ɫ���󶨵���Ⱦ����
			GetGame()->Direct3DDeviceContext()->VSSetShader(mVertexShader, nullptr, 0);
			// �����ºõĳ����������󶨵�������ɫ��
			GetGame()->Direct3DDeviceContext()->VSSetConstantBuffers(0, 1, &mConstantBuffer[0]);

			GetGame()->Direct3DDeviceContext()->PSSetShader(mPixelShader, nullptr, 0);
			GetGame()->Direct3DDeviceContext()->PSSetConstantBuffers(1, 1, &mConstantBuffer[1]);
		}
		// ����װ��׶εĶ��㻺��������
		UINT stride = sizeof(VertexPosNormalColor);	// ��Խ�ֽ���
		UINT offset = 0;						// ��ʼƫ����

		GetGame()->Direct3DDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		GetGame()->Direct3DDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		GetGame()->Direct3DDeviceContext()->DrawIndexed(mIndexCount, 0,0);

	}
}