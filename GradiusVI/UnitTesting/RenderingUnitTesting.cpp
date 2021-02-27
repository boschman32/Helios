#include "pch.h"
#if defined(_DEBUG)
#include "hepch.h"
#include "CppUnitTest.h"
#include "Renderer/DX12Renderer.h"
#include "Renderer/CommandQueue.h"
#include "Renderer/CommandList.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

namespace RenderingUnitTesting
{
    TEST_CLASS(Initialization)
    {
    public:
        friend class DX12Renderer;

        TEST_METHOD(GetHardwareAdapter)
        {
            HRESULT hr;

            ComPtr<IDXGIFactory4> dxgiFactory;
            UINT createFactoryFlags = 0;

            hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
            Assert::IsTrue(SUCCEEDED(hr));

            ComPtr<IDXGIAdapter1> dxgiAdapter1;
            ComPtr<IDXGIAdapter4> dxgiAdapter4;

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
                        hr = dxgiAdapter1.As(&dxgiAdapter4);
                        Assert::IsTrue(SUCCEEDED(hr));
                    }
                }
            }
        }

        TEST_METHOD(CreateDX12Device)
        {
            HRESULT hr;

            auto dxgiAdapter = GetAdapter(false);
            Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device;

            hr = D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device));
            Assert::IsTrue(SUCCEEDED(hr));
        }
    };

    TEST_CLASS(ResourceLoading)
    {
    public:
        friend class Mesh;
        friend class Texture;

        TEST_METHOD(LoadGLTFModel)
        {
            DX12Renderer::CreateWithoutWindow();

            Mesh mesh;
            Material material;

            std::string modelFilePath = "..\\..\\..\\..\\GradiusVI\\Assets\\GLTF\\Duck\\Duck.gltf";

            fx::gltf::Document doc = fx::gltf::LoadFromText(modelFilePath);
            const fx::gltf::Mesh& meshData = doc.meshes[0];
            fx::gltf::Primitive const& primitive = meshData.primitives[0];

            auto const& attrib = primitive.attributes;

            uint32_t positionIndex = attrib.at("POSITION");
            fx::gltf::Accessor const& vertexAccessor = doc.accessors[positionIndex];
            fx::gltf::Accessor const& indexAccessor = doc.accessors[primitive.indices];

            mesh.LoadFromGLTFWithoutTexture(modelFilePath, 0);

            Assert::AreEqual(static_cast<size_t>(vertexAccessor.count), mesh.GetVertexBufferSize());
            Assert::AreEqual(static_cast<size_t>(indexAccessor.count), mesh.GetIndexBufferSize());

            DX12Renderer::Destroy();
        }

        TEST_METHOD(LoadTexture)
        {
            DX12Renderer::CreateWithoutWindow();

            auto device = DX12Renderer::GetInstance().GetDevice();
            auto commandQueue = DX12Renderer::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
            auto commandList = commandQueue->GetCommandList();

            Texture texture;
            {
                std::wstring textureFilePath = L"..\\..\\..\\..\\GradiusVI\\Assets\\Textures\\DefaultWhite.bmp";
                commandList->LoadTextureFromFile(texture, textureFilePath);

                Assert::AreEqual(texture.m_width, static_cast<uint32_t>(1));
                Assert::AreEqual(texture.m_height, static_cast<uint32_t>(1));
            }

            {
                std::wstring textureFilePath = L"..\\..\\..\\..\\GradiusVI\\Assets\\Textures\\Mona_Lisa.jpg";
                commandList->LoadTextureFromFile(texture, textureFilePath);

                Assert::AreEqual(texture.m_width, static_cast<uint32_t>(2835));
                Assert::AreEqual(texture.m_height, static_cast<uint32_t>(4289));
            }

            {
                std::wstring textureFilePath = L"..\\..\\..\\..\\GradiusVI\\Assets\\Textures\\transparency test 2.png";
                commandList->LoadTextureFromFile(texture, textureFilePath);

                Assert::AreEqual(texture.m_width, static_cast<uint32_t>(512));
                Assert::AreEqual(texture.m_height, static_cast<uint32_t>(512));
            }

            DX12Renderer::Destroy();
        }
    };
}
#endif