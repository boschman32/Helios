#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <memory>
#include <string>
#include <map>

#include "Renderer/DescriptorAllocation.h"

class Window;
class CommandQueue;
class DescriptorAllocator;
class Pipeline;
class KeyEventArgs;

namespace Helios { class Entity; }

class DX12Renderer
{
public:
    static void Create(HINSTANCE a_hInstance, const std::wstring a_windowTitle, uint32_t a_windowWidth, uint32_t a_windowHeight);
    static void Destroy();

    static bool GetIsInitialized();

    //get static instance of DX12Renderer
    static DX12Renderer& GetInstance();

    void NewFrame(std::shared_ptr<Window> a_pWindow);
    bool RenderToWindow(std::shared_ptr<Window> a_pWindow, DirectX::XMFLOAT4 viewportRanges, std::vector<Helios::Entity*>& pEntities);

    bool TestDeleteThis();

    //RenderToWindow(std::wstring windowTitle, Camera& camera) : void

    //returns the DirectX 12 device
    Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice();

    std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_commandListType);

    std::shared_ptr<Window> CreateWindowInstance(const std::wstring& a_windowClassName, const std::wstring& a_windowName, int a_width, int a_height, bool a_vSync);

    //removes window from list: if window goes out of scope, it is destroyed
    void RemoveWindowInstance(std::shared_ptr<Window> a_window);
    /*void DestroyWindowInstance(const std::wstring& a_windowName);*/
    /*void DestroyWindowInstance(std::shared_ptr<Window> a_window);*/

    std::shared_ptr<Window> GetWindowByName(const std::wstring& a_windowName);
    std::shared_ptr<Window> GetWindowByHandle(const HWND a_hWnd);
    std::shared_ptr<Window> GetFirstWindow();

    //ensures that no GPU commands are currently executing and that no GPU resources are in use
    void Flush();

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT a_numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE a_type);
    UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE a_type) const;

    DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);
    void ReleaseStaleDescriptors(uint64_t finishedFrame);

    bool IsTearingSupported();

    size_t GetWindowListSize();

    static uint64_t GetFrameCount();

    DXGI_SAMPLE_DESC GetMultisampleQualityLevels(DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;

    void OnKeyPressed(KeyEventArgs& e);
    void OnKeyReleased(KeyEventArgs& e);

    static void CreateWithoutWindow();
private:
    void InitializeWithoutWindow();

    //The constructor and destructor are private as they are called by the Create() function
    DX12Renderer(HINSTANCE a_hInstance);
    ~DX12Renderer();

    void Initialize(const std::wstring a_windowTitle, uint32_t a_windowWidth, uint32_t a_windowHeight);
	//called by Initialize() function

    bool CheckTearingSupport();

public:
    
private:

    static DX12Renderer* ms_renderer; //singleton instance
	static bool ms_isInitialized;
    static uint64_t ms_frameCount;

    //DirectX 12 device
    Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;

    //used to keep track of Win32 instance
    HINSTANCE m_hInstance;

    bool m_useWarp = false;
    bool m_isTearingSupported = false;

    std::map<HWND, std::shared_ptr<Window>> m_windowByHandleMap;
    std::map<std::wstring, std::shared_ptr<Window>> m_windowByNameMap;

    std::shared_ptr<CommandQueue> m_pDirectCommandQueue;
    std::shared_ptr<CommandQueue> m_pComputeCommandQueue;
    std::shared_ptr<CommandQueue> m_pCopyCommandQueue;

    std::unique_ptr<DescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    std::unique_ptr<Pipeline> m_pPipeline = nullptr;
};

Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool a_useWarp);
void CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> a_adapter, Microsoft::WRL::ComPtr<ID3D12Device2>& a_d3d12Device);

//Window callback function
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);