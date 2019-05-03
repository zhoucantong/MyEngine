#pragma once
#include "Common.h"
#include "Game.h"


namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

using namespace Library;
namespace Library
{
	class Keyboard;
	class Mouse;
	class FirstPersonCamera;
	class FpsComponent;
	class Grid;

}
namespace Rendering
{
	class TriangleDemo;
	class ModelDemo;
}
//using namespace Library::FpsComponent;
namespace Rendering
{
	using Library::FpsComponent;
	class RenderingGame : public Game
	{
	public:
		RenderingGame(HINSTANCE instance, const std::wstring&
			windowClass, const std::wstring& windowTitle, int showCommand);
		~RenderingGame();
		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;
		virtual void Shutdown() override;


	private:
		static const XMVECTORF32 BackgroundColor;

		LPDIRECTINPUT8 mDirectInput;

		FpsComponent* mFpsComponent;
		Keyboard*	mKeyboard;
		Mouse*	mMouse;
		FirstPersonCamera* mCamera;
		TriangleDemo* mDemo;
		ModelDemo* mModelDemo;
		Grid* mGrid;
		TriangleDemo* mTangle;
		XMFLOAT2 mMouseTextPosition;

		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;


	};
}


