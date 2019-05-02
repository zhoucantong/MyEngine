#pragma once

#include <windows.h>
#include <string>
#include "D3D11.h"
#include "GameClock.h"
#include "GameTime.h"
#include <vector>
#include "ServiceContainer.h"

using namespace Library;

namespace Library
{
	class GameComponent;
    class Game
    {
    public:
        Game(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        virtual ~Game();

        HINSTANCE Instance() const;
        HWND WindowHandle() const;
        const WNDCLASSEX& Window() const; 
        const std::wstring& WindowClass() const;
        const std::wstring& WindowTitle() const;
        int ScreenWidth() const;
        int ScreenHeight() const;

        virtual void Run();
        virtual void Exit();
        virtual void Initialize();
        virtual void Update(const GameTime& gameTime);
        virtual void Draw(const GameTime& gameTime);

		ID3D11Device* Direct3DDevice() const;
		ID3D11DeviceContext* Direct3DDeviceContext() const;
		bool DepthBufferEnabled() const;
		bool IsFullScreen() const;
		const D3D11_TEXTURE2D_DESC& BackBufferDesc() const;
		const D3D11_VIEWPORT& Viewport() const;
		const std::vector<GameComponent*>& Components() const;

		ServiceContainer& Services();
		float AspectRatio() const;
    protected:
        virtual void InitializeWindow();
		virtual void Shutdown();
		virtual void InitializeDirectX();

        static const UINT DefaultScreenWidth;
        static const UINT DefaultScreenHeight;

        HINSTANCE mInstance;
        std::wstring mWindowClass;
        std::wstring mWindowTitle;
        int mShowCommand;
        
        HWND mWindowHandle;
        WNDCLASSEX mWindow;		

        UINT mScreenWidth;
        UINT mScreenHeight;

        GameClock mGameClock;
        GameTime mGameTime;

		static const UINT DefaultFrameRate;
		static const UINT DefaultMultiSamplingCount;
		D3D_FEATURE_LEVEL mFeatureLevel;
		ID3D11Device* mDirect3DDevice;
		ID3D11DeviceContext* mDirect3DDeviceContext;
		IDXGISwapChain* mSwapChain;
		UINT mFrameRate;
		bool mIsFullScreen;
		bool mDepthStencilBufferEnabled;
		bool mMultiSamplingEnabled;
		UINT mMultiSamplingCount;
		UINT mMultiSamplingQualityLevels;
		ID3D11Texture2D* mDepthStencilBuffer;
		D3D11_TEXTURE2D_DESC mBackBufferDesc;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
		ID3D11RasterizerState* mWireframeRS;
		D3D11_VIEWPORT mViewport;

		std::vector<GameComponent*> mComponents;

		ServiceContainer mServices;
    private:
        Game(const Game& rhs);
        Game& operator=(const Game& rhs);

        POINT CenterWindow(int windowWidth, int windowHeight);
        static LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);		
    };
}