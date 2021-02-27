#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

struct CommandAllocatorData
{
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    uint64_t fenceValue;
};