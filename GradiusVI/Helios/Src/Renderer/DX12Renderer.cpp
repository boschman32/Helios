#include "hepch.h"
#include "Renderer/DX12Renderer.h"

#include "Renderer/Window.h"
#include "Renderer/WindowGUI.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/DescriptorAllocator.h"
#include "Renderer/Pipeline.h"
#include "Renderer/RenderHelpers.h"
#include "Renderer/d3dx12.h"
#include "Renderer/InputEvents.h"

#include "Core/Core.h"
#include <cassert>

using namespace Microsoft::WRL;

DX12Renderer* DX12Renderer::ms_renderer = nullptr;
bool DX12Renderer::ms_isInitialized = false;
uint64_t DX12Renderer::ms_frameCount = 0;

struct MakeWindow : public Window
{
    MakeWindow(HINSTANCE a_hInstance, const std::wstring& a_windowClass, const std::wstring& a_windowTitle, uint32_t a_width, uint32_t a_height, bool a_vsync)
        : Window(a_hInstance, a_windowClass, a_windowTitle, a_width, a_height, a_vsync)
    { }
};

void DX12Renderer::Create(HINSTANCE a_hInstance, const std::wstring a_windowTitle, uint32_t a_windowWidth, uint32_t a_windowHeight)
{
    HE_CORE_ASSERT(ms_renderer == nullptr);

    ms_renderer = new DX12Renderer(a_hInstance);
    ms_renderer->Initialize(a_windowTitle, a_windowWidth, a_windowHeight);
}

void DX12Renderer::Destroy()
{
    HE_CORE_ASSERT(ms_renderer != nullptr);

    ms_renderer->Flush();

    delete ms_renderer;
}

bool DX12Renderer::GetIsInitialized()
{
    return ms_isInitialized;
}

DX12Renderer& DX12Renderer::GetInstance()
{
    HE_CORE_ASSERT(ms_renderer != nullptr);

    return *ms_renderer;
}

void DX12Renderer::NewFrame(std::shared_ptr<Window> a_pWindow)
{
    HE_CORE_ASSERT(a_pWindow != nullptr);

    if (a_pWindow->m_gui != nullptr)
    {
        a_pWindow->m_gui->NewFrame();
    }
}

bool DX12Renderer::RenderToWindow(std::shared_ptr<Window> a_pWindow, DirectX::XMFLOAT4 viewportRanges, std::vector<Helios::Entity*>& pEntities)
{
    ms_frameCount++;

    m_pPipeline->RenderScene(*a_pWindow, viewportRanges, pEntities);


    return true;
}

bool DX12Renderer::TestDeleteThis()
{
    MSG msg = {};
    while (msg.message != WM_QUIT) //The message loop on is a standard message loop for a windows application
    {
        //Messages are dispatched to the window procedure (the WndProc function described earlier) until the WM_QUIT message is posted to the message queue using the PostQuitMessage function (this happens in the WndProc function)
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    return false;
}

void DX12Renderer::Flush()
{
    m_pDirectCommandQueue->Flush();
    m_pComputeCommandQueue->Flush();
    m_pCopyCommandQueue->Flush();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DX12Renderer::CreateDescriptorHeap(UINT a_numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE a_type)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = a_type;
    desc.NumDescriptors = a_numDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

UINT DX12Renderer::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE a_type) const
{
    return m_d3d12Device->GetDescriptorHandleIncrementSize(a_type);
}

DescriptorAllocation DX12Renderer::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
    return m_DescriptorAllocators[type]->Allocate(numDescriptors);
}

void DX12Renderer::ReleaseStaleDescriptors(uint64_t finishedFrame)
{
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_DescriptorAllocators[i]->ReleaseStaleDescriptors(finishedFrame);
    }
}

bool DX12Renderer::IsTearingSupported()
{
    return m_isTearingSupported;
}

size_t DX12Renderer::GetWindowListSize()
{
    return m_windowByNameMap.size();
}

uint64_t DX12Renderer::GetFrameCount()
{
    return ms_frameCount;
}

DXGI_SAMPLE_DESC DX12Renderer::GetMultisampleQualityLevels(DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) const
{
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format = format;
    qualityLevels.SampleCount = 1;
    qualityLevels.Flags = flags;
    qualityLevels.NumQualityLevels = 0;

    while (qualityLevels.SampleCount <= numSamples && SUCCEEDED(m_d3d12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS))) && qualityLevels.NumQualityLevels > 0)
    {
        // That works...
        sampleDesc.Count = qualityLevels.SampleCount;
        sampleDesc.Quality = qualityLevels.NumQualityLevels - 1;

        // But can we do better?
        qualityLevels.SampleCount *= 2;
    }

    return sampleDesc;
}

void DX12Renderer::OnKeyPressed(KeyEventArgs& e)
{
    e;
    /*switch (e.Key)
    {
    case KeyCode::Up:
    case KeyCode::W:
        m_pPipeline->m_forward = 0.05f;
        break;
    case KeyCode::Left:
    case KeyCode::A:
        m_pPipeline->m_left = 0.05f;
        break;
    case KeyCode::Down:
    case KeyCode::S:
        m_pPipeline->m_backward = 0.05f;
        break;
    case KeyCode::Right:
    case KeyCode::D:
        m_pPipeline->m_right = 0.05f;
        break;
    default:
        break;
    }*/
}

void DX12Renderer::OnKeyReleased(KeyEventArgs& e)
{
    e;
    /*switch (e.Key)
    {
    case KeyCode::Up:
    case KeyCode::W:
        m_pPipeline->m_forward = 0.0f;
        break;
    case KeyCode::Left:
    case KeyCode::A:
        m_pPipeline->m_left = 0.0f;
        break;
    case KeyCode::Down:
    case KeyCode::S:
        m_pPipeline->m_backward = 0.0f;
        break;
    case KeyCode::Right:
    case KeyCode::D:
        m_pPipeline->m_right = 0.0f;
        break;
    default:
        break;
    }*/
}

DX12Renderer::DX12Renderer(HINSTANCE a_hInstance)
    : m_hInstance(a_hInstance)
{
}

DX12Renderer::~DX12Renderer()
{
    Flush();

    ms_renderer = nullptr;
    ms_isInitialized = false;
}

void DX12Renderer::Initialize(const std::wstring a_windowTitle, uint32_t a_windowWidth, uint32_t a_windowHeight)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

#if defined(_DEBUG)
    //enable the debug layer before doing anything DX12 related so all possible errors generated while creating DX12 objects are caught by the debug layer.
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif

    std::wstring windowClassName = L"BasicWindowClass";
    std::wstring windowName = a_windowTitle;

    WNDCLASSEXW wndClass = { 0 };

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = m_hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hIcon = ::LoadIcon(m_hInstance, NULL);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = windowClassName.c_str();
    wndClass.hIconSm = ::LoadIcon(m_hInstance, NULL);

    Window::RegisterWindowClass(wndClass);

    auto dxgiAdapter = GetAdapter(m_useWarp);
    if (dxgiAdapter)
    {
        CreateDevice(dxgiAdapter, m_d3d12Device);
    }

    m_pDirectCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_pComputeCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    m_pCopyCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

    m_isTearingSupported = CheckTearingSupport();

    // Create descriptor allocators
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_DescriptorAllocators[i] = std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
    }

    std::shared_ptr<Window> tempWindow = CreateWindowInstance(windowClassName, windowName, a_windowWidth, a_windowHeight, true);
    tempWindow->Show();

    m_pPipeline = std::make_unique<Pipeline>(a_windowWidth, a_windowHeight);

    ms_isInitialized = true;
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool a_useWarp)
{
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (a_useWarp)
    {
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else
    {
        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    return dxgiAdapter4;
}

void CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> a_adapter, Microsoft::WRL::ComPtr<ID3D12Device2>& a_d3d12Device)
{
    ThrowIfFailed(D3D12CreateDevice(a_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&a_d3d12Device)));
    //    NAME_D3D12_OBJECT(d3d12Device2);

        // Enable debug messages in debug mode.
#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(a_d3d12Device.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
    }
#endif
}

Microsoft::WRL::ComPtr<ID3D12Device2> DX12Renderer::GetDevice()
{
    return m_d3d12Device;
}

std::shared_ptr<CommandQueue> DX12Renderer::GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_commandListType)
{
    std::shared_ptr<CommandQueue> pCommandQueue = nullptr;

    switch (a_commandListType)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        pCommandQueue = m_pDirectCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        pCommandQueue = m_pComputeCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        pCommandQueue = m_pCopyCommandQueue;
        break;
    default:
        HE_CORE_ASSERT(false, "Invalid command queue type.");
    }

    return pCommandQueue;
}

std::shared_ptr<Window> DX12Renderer::CreateWindowInstance(const std::wstring& a_windowClassName, const std::wstring& a_windowName, int a_width, int a_height, bool a_vSync)
{
    if (GetWindowByName(a_windowName))
    {
        HE_CORE_ASSERT(false, "Attempted to create a window with an already taken name.");
    }

    std::shared_ptr<Window> pWindow = std::make_shared<MakeWindow>(m_hInstance, a_windowClassName, a_windowName, a_width, a_height, a_vSync);

    m_windowByHandleMap.insert(std::map<HWND, std::shared_ptr<Window>>
        ::value_type(pWindow->GetWindowHandle(), pWindow));

    m_windowByNameMap.insert(std::map<std::wstring, std::shared_ptr<Window>>
        ::value_type(pWindow->GetWindowName(), pWindow));

    return pWindow;
}

void DX12Renderer::RemoveWindowInstance(std::shared_ptr<Window> a_window)
{
    ms_renderer->Flush();

    m_windowByNameMap.erase(a_window->GetWindowName());
    m_windowByHandleMap.erase(a_window->GetWindowHandle());
}

std::shared_ptr<Window> DX12Renderer::GetWindowByName(const std::wstring& a_windowName)
{
    std::shared_ptr<Window> pWindow = nullptr;
    std::map <std::wstring, std::shared_ptr<Window>>::iterator iter = m_windowByNameMap.find(a_windowName);
    if (iter != m_windowByNameMap.end())
    {
        pWindow = iter->second;
    }
    
    return pWindow;
}

std::shared_ptr<Window> DX12Renderer::GetWindowByHandle(const HWND a_hWnd)
{
    std::shared_ptr<Window> pWindow = nullptr;
    std::map <HWND, std::shared_ptr<Window>>::iterator iter = m_windowByHandleMap.find(a_hWnd);
    if (iter != m_windowByHandleMap.end())
    {
        pWindow = iter->second;
    }

    return pWindow;
}

std::shared_ptr<Window> DX12Renderer::GetFirstWindow()
{
    return m_windowByNameMap.begin()->second;
}

bool DX12Renderer::CheckTearingSupport()
{
    BOOL allowTearing = FALSE;

    // Rather than create the DXGI 1.5 factory interface directly, we create the
    // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
    // graphics debugging tools which will not support the 1.5 factory interface 
    // until a future update.
    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing,
                sizeof(allowTearing)
            )))
            {
                allowTearing = FALSE;
            }
        }
    }

    return allowTearing == TRUE;
}
#include "Core/InputManager.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND a_hWnd, UINT a_message, WPARAM a_wParam, LPARAM a_lParam)
{
    DX12Renderer& renderer = DX12Renderer::GetInstance();
    std::shared_ptr<Window> pWindow = renderer.GetWindowByHandle(a_hWnd);

    if (pWindow)
    {
        if (ImGui_ImplWin32_WndProcHandler(a_hWnd, a_message, a_wParam, a_lParam))
            return true;

        switch (a_message)
        {
        case WM_PAINT:
        {
            
        }
        break;

        case WM_SIZE:
        {
            uint32_t width = static_cast<uint32_t>(LOWORD(a_lParam));
            uint32_t height = static_cast<uint32_t>(HIWORD(a_lParam));

            pWindow->Resize(width, height);
        }
        break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            MSG charMsg;
            // Get the Unicode character (UTF-16)
            unsigned int c = 0;
            // For printable characters, the next message will be WM_CHAR.
            // This message contains the character code we need to send the KeyPressed event.
            // Inspired by the SDL 1.2 implementation.
            if (PeekMessageW(&charMsg, a_hWnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
            {
                GetMessage(&charMsg, a_hWnd, 0, 0);
                c = static_cast<unsigned int>(charMsg.wParam);

                if (charMsg.wParam > 0 && charMsg.wParam < 0x10000)
                    ImGui::GetIO().AddInputCharacter((unsigned short)charMsg.wParam);
            }
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            KeyCode::Key key = (KeyCode::Key)a_wParam;
            unsigned int scanCode = (a_lParam & 0x00FF0000) >> 16;
            KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Pressed, shift, control, alt);
            scanCode;

        	for(auto&c1 : Helios::InputManager::GetAllControllers())
        	{
        		for(auto &b1 : c1->m_buttons)
        		{
					b1.OnKeyPressed(keyEventArgs);
        		}
        	}
            renderer.OnKeyPressed(keyEventArgs);
        }
        break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            KeyCode::Key key = (KeyCode::Key)a_wParam;
            unsigned int c = 0;
            unsigned int scanCode = (a_lParam & 0x00FF0000) >> 16;

            // Determine which key was released by converting the key code and the scan code
            // to a printable character (if possible).
            // Inspired by the SDL 1.2 implementation.
            unsigned char keyboardState[256];
            GetKeyboardState(keyboardState);
            wchar_t translatedCharacters[4];
            if (int result = ToUnicodeEx(static_cast<UINT>(a_wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
            {
                c = translatedCharacters[0];
            }

            KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Released, shift, control, alt);

			for (auto& c2 : Helios::InputManager::GetAllControllers())
			{
				for (auto& b2 : c2->m_buttons)
				{
					b2.OnKeyReleased(keyEventArgs);
				}
			}
            renderer.OnKeyReleased(keyEventArgs);
        }
        break;

        case WM_DESTROY:
        {
            renderer.RemoveWindowInstance(pWindow);

            if (renderer.GetWindowListSize() == 0)
            {
                PostQuitMessage(0);
            }
        }
        break;

        default:
            return ::DefWindowProcW(a_hWnd, a_message, a_wParam, a_lParam);
        }
    }
    else
    {
        return DefWindowProcW(a_hWnd, a_message, a_wParam, a_lParam);
    }

    return 0;
}

void DX12Renderer::CreateWithoutWindow()
{
    HE_CORE_ASSERT(ms_renderer == nullptr);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    ms_renderer = new DX12Renderer(hInstance);
    ms_renderer->InitializeWithoutWindow();
}

void DX12Renderer::InitializeWithoutWindow()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

#if defined(_DEBUG)
    //enable the debug layer before doing anything DX12 related so all possible errors generated while creating DX12 objects are caught by the debug layer.
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif

    auto dxgiAdapter = GetAdapter(m_useWarp);
    if (dxgiAdapter)
    {
        CreateDevice(dxgiAdapter, m_d3d12Device);
    }

    m_pDirectCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_pComputeCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    m_pCopyCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

    m_isTearingSupported = CheckTearingSupport();

    // Create descriptor allocators
    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        m_DescriptorAllocators[i] = std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
    }

    ms_isInitialized = true;
}
