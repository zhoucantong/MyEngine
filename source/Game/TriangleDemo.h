#pragma once
#include "DrawableGameComponent.h"
#include "LightHelper.h"
namespace Rendering
{
	class TriangleDemo :public DrawableGameComponent
	{
		RTTI_DECLARATIONS(TriangleDemo, DrawableGameComponent)
	public:
		TriangleDemo(Game& game, Camera& camera);
		virtual ~TriangleDemo();
		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;
	private:
		void InitEffect();
		void InitResource();
		void CreateIndexBuffer(ID3D11Buffer** indexBuffer);
		void CreateConstBuffer();

	private:
		//typedef struct _BasicEffectVertex
		//{
		//	XMFLOAT3 Position;
		//	XMFLOAT4 Color;
		//	_BasicEffectVertex() { }
		//	_BasicEffectVertex(XMFLOAT3 position, XMFLOAT4 color)
		//		: Position(position), Color(color) { }
		//} BasicEffectVertex;
		TriangleDemo();
		TriangleDemo(const TriangleDemo& rhs);
		TriangleDemo& operator=(const TriangleDemo& rhs);
		//ID3DX11Effect* mEffect;
		//ID3DX11EffectTechnique* mTechnique;
		//ID3DX11EffectPass* mPass;
		//ID3DX11EffectMatrixVariable* mWvpVariable;
		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mVertexShader;
		ID3D11PixelShader* mPixelShader;

		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		ID3D11Buffer* mConstantBuffer[2];

		XMFLOAT4X4 mWorldMatrix;

	/*	FPerObject CBuffer;*/

		UINT mIndexCount;
		UINT mVertexCount;

		///  lightTest
		VSConstantBuffer m_VSConstantBuffer;			// 用于修改用于VS的GPU常量缓冲区的变量
		PSConstantBuffer m_PSConstantBuffer;			// 用于修改用于PS的GPU常量缓冲区的变量
		DirectionalLight m_DirLight;					// 默认环境光
		PointLight m_PointLight;						// 默认点光
		SpotLight m_SpotLight;						    // 默认汇聚光
	};

}