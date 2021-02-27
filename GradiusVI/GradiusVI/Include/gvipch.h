#pragma once
#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <array>

#include "Core/Logger.h"

//Libraries

#pragma warning( push )
#pragma warning (disable : 26495 )
#pragma warning (disable : 26439 )
#pragma warning (disable : 26451 )
#include <rttr/registration>
#include <rttr/registration_friend.h>
#include <rttr/type>
#pragma warning ( pop )

#include <optick.h>

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