#include "ModelDemo.h"
#include "Game.h"
#include "GameException.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Utility.h"
#include "D3DCompiler.h"
#include "d3dUtil.h"
#include "Mesh.h"
#include "Model.h"
#include "DXTrace.h"
namespace Rendering
{

	RTTI_DEFINITIONS(ModelDemo)
		ModelDemo::ModelDemo(Game& game, Camera& camera)
		: DrawableGameComponent(game, camera),
	/*	mEffect(nullptr), mTechnique(nullptr), mPass(nullptr),
		mWvpVariable(nullptr),*/
		mInputLayout(nullptr), mWorldMatrix(MatrixHelper::Identity),
		mVertexBuffer(nullptr)
	{
	}
	ModelDemo::~ModelDemo()
	{
		//ReleaseObject(mWvpVariable);
		//ReleaseObject(mPass);
		//ReleaseObject(mTechnique);
		//ReleaseObject(mEffect);
		ReleaseObject(mInputLayout);
		ReleaseObject(mVertexBuffer);
	}
	void ModelDemo::Initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
	
		InitEffect();
		InitResource();
	}
	void ModelDemo::CreateVertexBuffer(ID3D11Device* device, const Mesh&
		mesh, ID3D11Buffer** vertexBuffer) const
	{
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		std::vector<BasicEffectVertex> vertices;
		vertices.reserve(sourceVertices.size());
		if (mesh.VertexColors().size() > 0)
		{
			std::vector<XMFLOAT4>* vertexColors = mesh.VertexColors().
				at(0);
			assert(vertexColors->size() == sourceVertices.size());
			for (UINT i = 0; i < sourceVertices.size(); i++)
			{
				XMFLOAT3 position = sourceVertices.at(i);
				XMFLOAT4 color = vertexColors->at(i);
				vertices.push_back(BasicEffectVertex(XMFLOAT3(position.x,
					position.y, position.z), color));
			}
		}
		else
		{
			for (UINT i = 0; i < sourceVertices.size(); i++)
			{
				XMFLOAT3 position = sourceVertices.at(i);
				XMFLOAT4 color = XMFLOAT4(ColorHelper::Purple);
				vertices.push_back(BasicEffectVertex(XMFLOAT3(position.x,
					position.y, position.z), color));
			}
		}
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(BasicEffectVertex) * vertices.
			size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = &vertices[0];
		if (FAILED(device->CreateBuffer(&vertexBufferDesc,
			&vertexSubResourceData, vertexBuffer)))
		{
			throw GameException("ID3D11Device::CreateBuffer() failed.");
		}
	}


	void ModelDemo::InitEffect()
	{
		// 创建顶点着色器
	// Compile the shader
		UINT shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ID3DBlob* blob = NULL;
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Triangle_VS.cso", L"Content\\HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", &blob));
		HR(mGame->Direct3DDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));

		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		// ID3DBlob* blob2;
		HR(mGame->Direct3DDevice()->
			CreateInputLayout(inputElementDescriptions,
				ARRAYSIZE(inputElementDescriptions), blob->GetBufferPointer(),
				blob->GetBufferSize(), &mInputLayout));
		// 创建像素着色器
		HR(d3dUtil::CreateShaderFromFile(L"Content\\HLSL\\Triangle_PS.cso", L"Content\\HLSL\\Triangle_PS.hlsl", "PS", "ps_5_0", &blob))
		HR(mGame->Direct3DDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mPixelShader));
	
	}

	void ModelDemo::InitResource()
	{
		std::unique_ptr<Model> model(new Model(*mGame,
			"Content\\Models\\Cube.obj", true));
		// Create the vertex and index buffers
		Mesh* mesh = model->Meshes().at(0);
		CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, &mVertexBuffer);
		mesh->CreateIndexBuffer(&mIndexBuffer);
		mIndexCount = mesh->Indices().size();
		mVertexCount = mesh->Vertices().size();

	}

	void ModelDemo::CreateConstBuffer()
	{
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(FPerObject);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		// 新建常量缓冲区，不使用初始数据
		if (FAILED(mGame->Direct3DDevice()->CreateBuffer(&cbd, nullptr, &mConstantBuffer)))
		{
			throw GameException("ID3D11Device::	void ModelDemo::CreateConstBuffer() failed.");
		}
	}

	void ModelDemo::Update(const GameTime& gameTime)
	{
		DrawableGameComponent::Update(gameTime);
	/*	XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->ViewMatrix() *
			mCamera->ProjectionMatrix();
		CBuffer.view = wvp;
		D3D11_MAPPED_SUBRESOURCE mappedData;
		FAILED(mGame->Direct3DDeviceContext()->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(CBuffer), &CBuffer, sizeof(CBuffer));*/
		/*mGame->Direct3DDeviceContext()->Unmap(mConstantBuffer, 0);*/
	}

	void ModelDemo::Draw(const GameTime& gameTime)
	{
		ID3D11DeviceContext* direct3DDeviceContext =
			mGame->Direct3DDeviceContext();
		direct3DDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader, nullptr, 0);

		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout);

		UINT stride = sizeof(BasicEffectVertex);
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer,
			&stride, &offset);
		direct3DDeviceContext->DrawIndexed(mIndexCount, 0,0);

	}
}