#pragma once
/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file Mesh.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A mesh class encapsulates the index and vertex buffers for a geometric primitive.
  */

#include "Renderer/CommandList.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include <Renderer/DX12Texture.h>

#include "Core/Resource.h"

#include <DirectXMath.h>
#include <d3d12.h>

#include <wrl.h>

#include <memory> // For std::unique_ptr
#include <vector>

#include <fx/gltf.h>

struct Material;
class Texture;

  // Vertex struct holding position, normal vector, and texture mapping information.
struct VertexPositionNormalTexture
{
    VertexPositionNormalTexture()
    {
    }

    VertexPositionNormalTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& textureCoordinate)
        : position(position),
        normal(normal),
        textureCoordinate(textureCoordinate)
    { }

    VertexPositionNormalTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR textureCoordinate)
    {
        XMStoreFloat3(&this->position, position);
        XMStoreFloat3(&this->normal, normal);
        XMStoreFloat2(&this->textureCoordinate, textureCoordinate);
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;

    static const int InputElementCount = 3;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

using VertexCollection = std::vector<VertexPositionNormalTexture>;
using IndexCollection = std::vector<uint16_t>;

class Mesh : public Helios::Resource
{
public:
    Mesh();
    Mesh(Material& a_material);
    Mesh(const Mesh& copy) = delete;
    virtual ~Mesh();

    virtual void OnInitialize() override;

    void Render(CommandList& commandList, uint32_t instanceCount = 1, uint32_t firstInstance = 0) const;

    static std::unique_ptr<Mesh> CreateCube(float size = 1, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateSphere(float diameter = 1, size_t tessellation = 16, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateCone(float diameter = 1, float height = 1, size_t tessellation = 32, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateTorus(float diameter = 1, float thickness = 0.333f, size_t tessellation = 32, bool rhcoords = false);
	static std::unique_ptr<Mesh> CreatePlane(float width = 1, float height = 1, bool rhcoords = false);
	static std::unique_ptr<Mesh> CreateTriangle(float width = 1, float height = 1, bool rhcoords = false);

    void LoadFromGLTF(const std::string& a_filePath, std::size_t a_meshIndex, Material& a_material);

    std::shared_ptr<Helios::DX12Texture> GetTextureResource() const { return m_primitiveTextureResource; }

    void SetTexture(const std::shared_ptr<Helios::DX12Texture>& a_pTexture) { m_primitiveTextureResource = a_pTexture; }

    void LoadFromGLTFWithoutTexture(const std::string& a_filePath, std::size_t a_meshIndex);

    size_t GetVertexBufferSize() { return m_VertexBuffer.GetNumVertices(); }
    size_t GetIndexBufferSize() { return m_IndexBuffer.GetNumIndicies(); }
protected:

private:
    friend struct std::default_delete<Mesh>;

    Material* m_pMaterial = nullptr;

    void Initialize(CommandList& commandList, VertexCollection& vertices, IndexCollection& indices, bool rhcoords);

    std::shared_ptr<Helios::DX12Texture> m_primitiveTextureResource = nullptr;

    VertexBuffer m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    UINT m_IndexCount;

    bool m_isInitialized = false;
};

static uint32_t CalculateDataTypeSize(fx::gltf::Accessor const& accessor) noexcept
{
    uint32_t elementSize = 0;
    switch (accessor.componentType)
    {
    case fx::gltf::Accessor::ComponentType::Byte:
    case fx::gltf::Accessor::ComponentType::UnsignedByte:
        elementSize = 1;
        break;
    case fx::gltf::Accessor::ComponentType::Short:
    case fx::gltf::Accessor::ComponentType::UnsignedShort:
        elementSize = 2;
        break;
    case fx::gltf::Accessor::ComponentType::Float:
    case fx::gltf::Accessor::ComponentType::UnsignedInt:
        elementSize = 4;
        break;
    }

    switch (accessor.type)
    {
    case fx::gltf::Accessor::Type::Mat2:
        return 4 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Mat3:
        return 9 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Mat4:
        return 16 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Scalar:
        return elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec2:
        return 2 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec3:
        return 3 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec4:
        return 4 * elementSize;
        break;
    }

    return 0;
}

struct BufferInfo
{
    fx::gltf::Accessor const* Accessor;

    uint8_t const* Data;
    uint32_t DataTypeSize;
    uint32_t TotalSize;
    uint32_t NumElements;
    uint32_t EmptySpace;

    bool HasData() const noexcept
    {
        return Data != nullptr;
    }
};

static BufferInfo GetData(fx::gltf::Document const& doc, fx::gltf::Accessor const& accessor)
{
    fx::gltf::BufferView const& bufferView = doc.bufferViews[accessor.bufferView];
    fx::gltf::Buffer const& buffer = doc.buffers[bufferView.buffer];

    const uint32_t dataTypeSize = CalculateDataTypeSize(accessor);
    auto emptySpace = bufferView.byteStride == 0 ? 0 : bufferView.byteStride - dataTypeSize;

    return BufferInfo
    {
        &accessor,
        &buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + accessor.byteOffset],
        dataTypeSize,
        accessor.count* dataTypeSize,
        accessor.count,
        emptySpace
    };
}

template<typename T>
T const* GetElement(BufferInfo bufferInfo, uint32_t index)
{
    bufferInfo;
    index;
    /*return 0;*/

    assert(bufferInfo.HasData() && "Error! No data in this BufferInfo object.");
    /*assert(sizeof(T) == bufferInfo.DataTypeSize && "Error! Data type specified is not of the same length as the internal format.");*/
    assert(index < bufferInfo.NumElements && "Error! Index out of bounds for BufferInfo.");

    return reinterpret_cast<T const*>(reinterpret_cast<size_t>(bufferInfo.Data) +
        (static_cast<size_t>(index)* static_cast<size_t>(bufferInfo.EmptySpace)) +
        (static_cast<size_t>(index)* static_cast<size_t>(bufferInfo.DataTypeSize)));
}

