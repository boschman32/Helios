#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>

#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <set>

#include "Core/Logger.h"

//Libraries

//RTTR
#pragma warning( push )
#pragma warning (disable : 26495 )
#pragma warning (disable : 26439 )
#pragma warning (disable : 26451 )
#include <rttr/registration>
#include <rttr/registration_friend.h>
#include <rttr/type>
#pragma warning ( pop )

//Optick
#include <optick.h>

//Rapidjson
#pragma warning( push )
#pragma warning( disable : 26444 )
#pragma warning( disable : 26451 )
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#pragma warning( pop )

//Eventpp
#include <eventpp/callbacklist.h>
#include <eventpp/eventdispatcher.h>
#include <eventpp/eventpolicies.h>
#include <eventpp/eventqueue.h>

//FMOD
#include <fmod_studio.hpp>

//Imgui
#include <imgui.h>

//includes for rendering
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> // For CommandLineToArgvW

#include <wrl.h>

//FXGLTF for loading gltf models
#include <fx/gltf.h>

// DirectX 12 specific headers.
#include <d3d12.h>
#include "Renderer/d3dx12.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"
#include "DirectXTex.h"

//GLM
#include <vec2.hpp>
#include <vec3.hpp>
#include <matrix.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtx/matrix_major_storage.hpp>

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <gtx/string_cast.hpp>
#include <gtx/quaternion.hpp>
#pragma warning( pop )