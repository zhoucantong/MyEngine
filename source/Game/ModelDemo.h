#pragma once
#include "DrawableGameComponent.h"

using namespace Library;
namespace Library
{
	class Mesh;
}
namespace Rendering
{
	class ModelDemo :public DrawableGameComponent
	{
		RTTI_DECLARATIONS(ModelDemo, DrawableGameComponent)
	public:
		ModelDemo(Game& game, Camera& camera);
		virtual ~ModelDemo();
		virtual void Initialize() override;
		virtual void Draw(const GameTime& gameTime) override;
		virtual void Update(const GameTime& gameTime) override;


	private:
		void InitEffect();
		void InitResource();
		void CreateConstBuffer();
		void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const;

	private:
		typedef struct _BasicEffectVertex
		{
			XMFLOAT3 Position;
			XMFLOAT4 Color;
			_BasicEffectVertex() { }
			_BasicEffectVertex(XMFLOAT3 position, XMFLOAT4 color)
				: Position(position), Color(color) { }
		} BasicEffectVertex;
		ModelDemo();
		ModelDemo(const ModelDemo& rhs);
		ModelDemo& operator=(const ModelDemo& rhs);

		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mVertexShader;
		ID3D11PixelShader* mPixelShader;

		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		ID3D11Buffer* mConstantBuffer;

		XMFLOAT4X4 mWorldMatrix;

		FPerObject CBuffer;
		UINT mIndexCount;
		UINT mVertexCount;
	};

}