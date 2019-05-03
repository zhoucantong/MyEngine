#include "RenderingGame.h"
#include "GameException.h"
#include "FpsComponent.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "FirstPersonCamera.h"
#include "TriangleDemo.h"
#include "ModelDemo.h"
#include "SpriteBatch.h"
#include <SpriteFont.h>
#include "Utility.h"
#include <sstream>
#include "ServiceContainer.h"
#include "ColorHelper.h"

// using namespace std;
namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = ColorHelper::Black;

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
		: Game(instance, windowClass, windowTitle, showCommand),
		mFpsComponent(nullptr),
		mDirectInput(nullptr), mKeyboard(nullptr), mMouse(nullptr),
		mGrid(nullptr)
	{
		mDepthStencilBufferEnabled = true;
		mMultiSamplingEnabled = true;
	}

	RenderingGame::~RenderingGame()
	{
	}

	void RenderingGame::Initialize()
	{
		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput, nullptr)))
		{
			throw GameException("DirectInput8Create() failed");
		}

		mKeyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(mKeyboard);
		mServices.AddService(Keyboard::TypeIdClass(), mKeyboard);

		mMouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mMouse);
		mServices.AddService(Mouse::TypeIdClass(), mMouse);

		mCamera = new FirstPersonCamera(*this);
		mComponents.push_back(mCamera);
		mServices.AddService(Camera::TypeIdClass(), mCamera);

		mFpsComponent = new FpsComponent(*this);
		mComponents.push_back(mFpsComponent);

		mTangle = new TriangleDemo(*this, *mCamera);
		mComponents.push_back(mTangle);

		//ModelDemo* pModelDemo = new ModelDemo(*this, *mCamera);
		//mComponents.push_back(pModelDemo);
	
		//mGrid = new Grid(*this, *mCamera);
		// mComponents.push_back(mGrid);


		{
			//构建点光源
		}
		Game::Initialize();

		mCamera->SetPosition(0.0f, 0.0f, 10.0f);
	}

	void RenderingGame::Shutdown()
	{
		//DeleteObject(mGrid);
		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		DeleteObject(mFpsComponent);
		DeleteObject(mCamera);

		ReleaseObject(mDirectInput);

		Game::Shutdown();
	}

	void RenderingGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		HRESULT hr = mSwapChain->Present(0, 0);
		if (FAILED(hr))
		{
			throw GameException("IDXGISwapChain::Present() failed.", hr);
		}
	}
}