#pragma once

//#include "Renderer/WindowGUI.h"

#include "Renderer/GUI.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Texture.h"

#include <memory>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

class SwapChain;
class WindowGUI;

class Window
{
public:
    virtual ~Window();

    void Resize(uint32_t a_width, uint32_t a_height);

    bool IsFullScreen() const;
    void SetFullscreen(bool a_fullscreen);

    int GetClientWidth() const;
    int GetClientHeight() const;

    HWND GetWindowHandle() const;

    const std::wstring& GetWindowName() const;
    bool IsVSyncEnabled() const;
    void SetVSync(bool a_vSync);

    UINT Present(const Texture& texture = Texture());

    void Show();
    void Hide();

    const RenderTarget& GetRenderTarget() const;

    Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain();

    void SetDisplayName(const std::string& a_displayName);
	const std::string& GetDisplayName() const { return m_displayName; }

    // Update the render target views for the swapchain back buffers.
    void UpdateRenderTargetViews();

    void SetRect(RECT a_rect);

protected:
    //Window can only be instantiated by friend class DX12Renderer
    Window(HINSTANCE a_hInstance, const std::wstring& a_windowClass, const std::wstring& a_windowTitle, uint32_t a_width, uint32_t a_height, bool a_vsync);

private:
    //called by constructor
    void CreateWindowHandle(HINSTANCE a_hInstance, const wchar_t* a_windowClassName);

    static void RegisterWindowClass(const WNDCLASSEXW& a_wndClass);


public:
    static const UINT s_bufferCount = 3;

private:
    friend class DX12Renderer;
    friend class Pipeline;
    friend class SwapChain;

    // The Window procedure needs to call protected methods of this class.
    friend LRESULT CALLBACK WndProc(HWND a_hWnd, UINT a_message, WPARAM a_wParam, LPARAM a_lParam);

    bool m_isInitialized = false;

    //handle to the window
    HWND m_hWnd;

    uint32_t m_clientWidth;
    uint32_t m_clientHeight;
    RECT m_rect;   

    std::wstring m_windowName;
	std::string m_displayName;

    bool m_vSync = true;
    bool m_isTearingSupported = false;
    bool m_fullscreen = false;

    UINT64 m_fenceValues[s_bufferCount] = { 0 };
    uint64_t m_frameValues[s_bufferCount] = { 0 };

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
    Texture m_backBufferTextures[s_bufferCount];
    // Marked mutable to allow modification in a const function.
    mutable RenderTarget m_renderTarget;

    UINT m_currentBackBufferIndex;

    std::unique_ptr<GUI> m_gui = nullptr;
};

