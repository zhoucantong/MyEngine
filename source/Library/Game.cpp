#include "Game.h"
#include "GameException.h"
#include "Common.h"
#include "GameComponent.h"
#include "DrawableGameComponent.h"
namespace Library
{
    const UINT Game::DefaultScreenWidth = 1920;
    const UINT Game::DefaultScreenHeight = 1080;
	const UINT Game::DefaultFrameRate = 60;
	const UINT Game::DefaultMultiSamplingCount = 4;

    Game::Game(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
        : mInstance(instance), mWindowClass(windowClass), mWindowTitle(windowTitle), mShowCommand(showCommand),
          mWindowHandle(), mWindow(),
          mScreenWidth(DefaultScreenWidth), mScreenHeight(DefaultScreenHeight),
		mGameClock(), mGameTime(), mFeatureLevel(D3D_FEATURE_LEVEL_9_1),
		mDirect3DDevice(nullptr),
		mDirect3DDeviceContext(nullptr), mSwapChain(nullptr),
		mFrameRate(DefaultFrameRate), mIsFullScreen(false),
		mDepthStencilBufferEnabled(false),
		mMultiSamplingEnabled(false),
		mMultiSamplingCount(DefaultMultiSamplingCount),
		mMultiSamplingQualityLevels(0),
		mDepthStencilBuffer(nullptr), mRenderTargetView(nullptr),
		mDepthStencilView(nullptr), mViewport()
    {
    }

    Game::~Game()
    {
    }

    HINSTANCE Game::Instance() const
    {
        return mInstance;
    }

    HWND Game::WindowHandle() const
    {
        return mWindowHandle;
    }

    const WNDCLASSEX& Game::Window() const
    {
        return mWindow;
    }

    const std::wstring& Game::WindowClass() const
    {
        return mWindowClass;
    }

    const std::wstring& Game::WindowTitle() const
    {
        return mWindowTitle;
    }

    int Game::ScreenWidth() const
    {
        return mScreenWidth;
    }

    int Game::ScreenHeight() const
    {
        return mScreenHeight;
    }
        
    void Game::Run()
    {
        InitializeWindow();
		InitializeDirectX();
        Initialize();

        MSG message;
        ZeroMemory(&message, sizeof(message));
        
        mGameClock.Reset();		

        while(message.message != WM_QUIT)
        {
            if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
            else
            {
                mGameClock.UpdateGameTime(mGameTime);
                Update(mGameTime);
                Draw(mGameTime);
            }
        }

		Shutdown();
    }

    void Game::Exit()
    {
        PostQuitMessage(0);
    }

    void Game::Initialize()
    {
		for (GameComponent* component : mComponents)
		{
			component->Initialize();
		}
    }

    void Game::Update(const GameTime& gameTime)
    {
		for (GameComponent* component : mComponents)
		{
			if (component->Enabled())
			{
				component->Update(gameTime);
			}
		}
    }

    void Game::Draw(const GameTime& gameTime)
    {
		for (GameComponent* component : mComponents)
		{
			DrawableGameComponent* drawableGameComponent =
				component->As<DrawableGameComponent>();
			if (drawableGameComponent != nullptr &&
				drawableGameComponent->Visible())
			{
				drawableGameComponent->Draw(gameTime);
			}
		}
    }

    void Game::InitializeWindow()
    {
        ZeroMemory(&mWindow, sizeof(mWindow));
        mWindow.cbSize = sizeof(WNDCLASSEX);
        mWindow.style = CS_CLASSDC;
        mWindow.lpfnWndProc = WndProc;
        mWindow.hInstance = mInstance;
        mWindow.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        mWindow.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        mWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
        mWindow.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
        mWindow.lpszClassName = mWindowClass.c_str();		

        RECT windowRectangle = { 0, 0, (LONG)mScreenWidth, (LONG)mScreenHeight };
        AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

        RegisterClassEx(&mWindow);
        POINT center = CenterWindow(mScreenWidth, mScreenHeight);
        mWindowHandle = CreateWindow(mWindowClass.c_str(), mWindowTitle.c_str(), WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, mInstance, nullptr);

        ShowWindow(mWindowHandle, mShowCommand);
        UpdateWindow(mWindowHandle);
    }
	void Game::InitializeDirectX()
	{
		HRESULT hr;
		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] = {
				D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};
		// Step 1: Create the Direct3D device and device context interfaces
	/*		ID3D11Device* direct3DDevice = nullptr;
		ID3D11DeviceContext* direct3DDeviceContext = nullptr;*/
		if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels,
			ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &mDirect3DDevice,
			&mFeatureLevel, &mDirect3DDeviceContext)))
		{
			throw GameException("D3D11CreateDevice() failed", hr);
		}
		if (FAILED(hr = mDirect3DDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&mDirect3DDevice))))
		{
			throw GameException("ID3D11Device::QueryInterface()failed", hr);
		}
		/*	if (FAILED(hr = mDirect3DDevice->QueryInterface
			(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>
				(&mDirect3DDeviceContext))))
			{
				throw GameException("ID3D11Device::QueryInterface()failed", hr);
			}*/
		//ReleaseObject(mDirect3DDevice);
		// ReleaseObject(mDirect3DDevice);
		// Step 2: Check for multisampling support
		mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMultiSamplingCount, &mMultiSamplingQualityLevels);
		if (mMultiSamplingQualityLevels == 0)
		{
			throw GameException("Unsupported multi-sampling quality");
		}
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = mScreenWidth;
		swapChainDesc.BufferDesc.Height = mScreenHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.OutputWindow = mWindowHandle;
		swapChainDesc.Windowed = true;
		swapChainDesc.Flags = 0;
		// Use MSAA
		if (mMultiSamplingEnabled)
		{
			swapChainDesc.SampleDesc.Count = mMultiSamplingCount;
			swapChainDesc.SampleDesc.Quality =
				mMultiSamplingQualityLevels - 1;
		}
		else
		{
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
		}
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		//
		D3D11_RASTERIZER_DESC noCullDesc;
		ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
		noCullDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME;// D3D11_FILL_SOLID;
		noCullDesc.CullMode = D3D11_CULL_NONE;
		noCullDesc.FrontCounterClockwise = false;
		noCullDesc.DepthClipEnable = true;
		noCullDesc.ScissorEnable = true;
		if (FAILED(hr = mDirect3DDevice->CreateRasterizerState(&noCullDesc, &mWireframeRS)))
		{
			throw GameException("ID3D11Device::CreateRasterizerState()failed", hr);
		}

		{
			// mDirect3DDeviceContext->RSSetState(mWireframeRS);
		}
		// Step 3: Create the swap chain
		IDXGIDevice* dxgiDevice = nullptr;
		if (FAILED(hr = mDirect3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))))
		{
			throw GameException("ID3D11Device::QueryInterface()failed", hr);
		}
		IDXGIAdapter *dxgiAdapter = nullptr;
		if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
			reinterpret_cast<void**>(&dxgiAdapter))))
		{
			ReleaseObject(dxgiDevice);
			throw GameException("IDXGIDevice::GetParent() failedretrieving adapter.", hr);
		}
		IDXGIFactory* dxgiFactory = nullptr;
		if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(&dxgiFactory))))
		{
			ReleaseObject(dxgiDevice);
			ReleaseObject(dxgiAdapter);
			throw GameException("IDXGIAdapter::GetParent() failedretrieving factory.", hr);
		}
		DXGI_SWAP_CHAIN_DESC fullScreenDesc;
		ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
		fullScreenDesc.BufferDesc.RefreshRate.Numerator = mFrameRate;
		fullScreenDesc.BufferDesc.RefreshRate.Denominator = 1;
		fullScreenDesc.Windowed = !mIsFullScreen;
		fullScreenDesc.OutputWindow = mWindowHandle;
		if (FAILED(hr = dxgiFactory->CreateSwapChain(dxgiDevice, &swapChainDesc,&mSwapChain)))
		{
			ReleaseObject(dxgiDevice);
			ReleaseObject(dxgiAdapter);
			ReleaseObject(dxgiFactory);
			throw GameException("IDXGIDevice::CreateSwapChain()failed.", hr);
		}
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);
		// Step 4: Create the render target view
		ID3D11Texture2D* backBuffer;
		if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
		{
			throw GameException("IDXGISwapChain::GetBuffer() failed.", hr);
		}
		backBuffer->GetDesc(&mBackBufferDesc);
		if (FAILED(hr = mDirect3DDevice->CreateRenderTargetView
		(backBuffer, nullptr, &mRenderTargetView)))
		{
			ReleaseObject(backBuffer);
			throw GameException("IDXGIDevice::CreateRenderTargetView()failed.", hr);
		}
		ReleaseObject(backBuffer);
		// Step 5: Create the depth-stencil view
		if (mDepthStencilBufferEnabled)
		{
			D3D11_TEXTURE2D_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.Width = mScreenWidth;
			depthStencilDesc.Height = mScreenHeight;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			if (mMultiSamplingEnabled)
			{
				depthStencilDesc.SampleDesc.Count =
					mMultiSamplingCount;
				depthStencilDesc.SampleDesc.Quality =
					mMultiSamplingQualityLevels - 1;
			}
			else
			{
				depthStencilDesc.SampleDesc.Count = 1;
				depthStencilDesc.SampleDesc.Quality = 0;
			}
			if (FAILED(hr = mDirect3DDevice->CreateTexture2D
			(&depthStencilDesc, nullptr, &mDepthStencilBuffer)))
			{
				throw GameException("IDXGIDevice::CreateTexture2D()failed.", hr);
			}
			if (FAILED(hr = mDirect3DDevice->CreateDepthStencilView
			(mDepthStencilBuffer, nullptr, &mDepthStencilView)))
			{
				throw GameException("IDXGIDevice::CreateDepthStencilView()failed.", hr);
			}
		}
		// Step 6: Bind the render target and depth-stencil views to OM stage
		mDirect3DDeviceContext->OMSetRenderTargets(1,&mRenderTargetView, mDepthStencilView);
		mViewport.TopLeftX = 0.0f;
		mViewport.TopLeftY = 0.0f;
		mViewport.Width = static_cast<float>(mScreenWidth);
		mViewport.Height = static_cast<float>(mScreenHeight);
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;
		// Step 7: Set the viewport
		mDirect3DDeviceContext->RSSetViewports(1, &mViewport);
	}
	void Game::Shutdown()
	{
		ReleaseObject(mRenderTargetView);
		ReleaseObject(mDepthStencilView);
		ReleaseObject(mSwapChain);
		ReleaseObject(mDepthStencilBuffer);
		if (mDirect3DDeviceContext != nullptr)
		{
			mDirect3DDeviceContext->ClearState();
		}
		ReleaseObject(mDirect3DDeviceContext);
		ReleaseObject(mDirect3DDevice);
		UnregisterClass(mWindowClass.c_str(), mWindow.hInstance);
	}

    LRESULT WINAPI Game::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(message)
        {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(windowHandle, message, wParam, lParam);
    }

    POINT Game::CenterWindow(int windowWidth, int windowHeight)
    {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        POINT center;
        center.x = (screenWidth - windowWidth) / 2;
        center.y = (screenHeight - windowHeight) / 2;

        return center;
    }	

	ID3D11Device* Game::Direct3DDevice() const
	{
		return mDirect3DDevice;
	}
	ID3D11DeviceContext*  Game::Direct3DDeviceContext() const
	{
		return mDirect3DDeviceContext;
	}
	bool  Game::DepthBufferEnabled() const
	{
		return mDepthStencilBufferEnabled;
	}
	bool  Game::IsFullScreen() const
	{
		return mIsFullScreen;
	}
	const D3D11_TEXTURE2D_DESC&  Game::BackBufferDesc() const
	{
		return mBackBufferDesc;
	}
	const D3D11_VIEWPORT&  Game::Viewport() const
	{
		return mViewport;
	}

	Game& Game::operator=(const Game& rhs)
	{
		return *this;
	}

	float Game::AspectRatio() const
	{
		return mScreenWidth*1.f/ mScreenHeight;
	}

	ServiceContainer& Game::Services()
	{
		return mServices;
	}
}