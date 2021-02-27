workspace "GradiusVI-Team1"
	architecture "x64"
	startproject "GradiusVI"

	configurations
	{
		"Debug-Editor",
		"Release-Editor",
		"No-Editor-Debug",
		"Release"
	}

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory) --
IncludeDir = {}
IncludeDir["ImGui"] = "Helios/Libs/imgui"
IncludeDir["spdlog"] = "Helios/Libs/spdlog/include"
IncludeDir["rttr"] = "Helios/Libs/rttr/Include"
IncludeDir["rapidjson"] = "Helios/Libs/rapidjson/include"
IncludeDir["FMOD"] = "Helios/Libs/FMOD/inc"
IncludeDir["optick"] = "Helios/Libs/Optick_/include"
IncludeDir["eventpp"] = "Helios/Libs/eventpp/include"
IncludeDir["glm"] = "Helios/Libs/glm"
IncludeDir["DirectXTex"] = "Helios/Libs/DirectXTex/DirectXTex"
IncludeDir["Lua"] = "Helios/Libs/Lua/include"
IncludeDir["fxgltf"] = "Helios/Libs/fx-gltf/include"
IncludeDir["nlohmann"] = "Helios/Libs/fx-gltf/test/thirdparty"

--Libraries
LibDir = {}
LibDir["rttr"] = "Helios/Libs/rttr/Lib"
LibDir["FMOD"] = "Helios/Libs/FMOD/lib"
LibDir["optick"] = "Helios/Libs/Optick_/lib/x64/release"
LibDir["optick_d"] = "Helios/Libs/Optick_/lib/x64/debug"
LibDir["Lua"] = "Helios/Libs/Lua"

--Project dependecies
group "Dependecies"
	include "Helios/Libs/imgui"
	include "Helios/Libs/DirectXTex"
group ""

include "UnitTesting"

--Engine
project "Helios"
	location "Helios"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	systemversion "latest"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("obj/" .. OutputDir .. "/%{prj.name}")

	pchheader "hepch.h"
	pchsource "Helios/Src/hepch.cpp"

	buildoptions
	{
		"/bigobj"
	}

	files
	{
		"%{prj.name}/Include/**.h",
		"%{prj.name}/Src/**.cpp",
		"%{prj.name}/Shaders/**.hlsl"
	}

	includedirs
	{
		"%{prj.name}/Include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.DirectXTex}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.rapidjson}",
		"%{IncludeDir.FMOD}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.eventpp}",
		"%{IncludeDir.eventpp}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Lua}",
		"%{IncludeDir.fxgltf}",
		"%{IncludeDir.nlohmann}"
	}

	libdirs
	{
		"%{LibDir.rttr}",
		"%{LibDir.FMOD}",
		"%{LibDir.Lua}"
	}

	links
	{
		"ImGui",
		"DirectXTex"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"_NULL_IMPORT_DESCIPTOR",
		"LUA53",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	warnings "extra"

	flags
	{
		"FatalWarnings",
		"MultiProcessorCompile"
	}

	ignoredefaultlibraries
	{
		"libcmt.lib"
	}

	--Debug
	filter "configurations:Debug-Editor or configurations:No-Editor-Debug"
		defines
		{
			"HE_DEBUG"
		}

		runtime "Debug"
		symbols "on"

		libdirs
		{
			"%{LibDir.optick_d}"
		}

	filter "configurations:Debug-Editor"
		defines "WITH_EDITOR"
	filter "configurations:Release-Editor"
		defines "WITH_EDITOR"

	--Release
	filter "configurations:Release or configurations:Release-Editor"
		defines {
			"HE_RELEASE"
		}
		runtime "Release"
		optimize "on"

		libdirs
		{
			"%{LibDir.optick}"
		}
	
	
	filter { "files:**.hlsl" }
		flags "ExcludeFromBuild"
		shadermodel "6.0"
		--note: below path has the name of the game project hardcoded
		shaderobjectfileoutput("%{wks.location}\\bin\\" .. OutputDir .. "\\GradiusVI\\Assets\\Shaders\\%{file.basename}.cso")
		shadervariablename "g_%{file.basename}"
		shaderheaderfileoutput("%{wks.location}\\Helios\\Shaders\\Headers\\%{file.basename}.h")
	filter { "files:**_ps.hlsl" }
		removeflags "ExcludeFromBuild"
		shadertype "Pixel"
		shaderentry "main"
	filter { "files:**_vs.hlsl" }
		removeflags "ExcludeFromBuild"
		shadertype "Vertex"
		shaderentry "main"
	filter { "files:**_cs.hlsl" }
		removeflags "ExcludeFromBuild"
		shadertype "Compute"
		shaderentry "main"

--Game project
project "GradiusVI"
	location "GradiusVI"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	systemversion "latest"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    pchheader "gvipch.h"
	pchsource "GradiusVI/Src/gvipch.cpp"

	buildoptions
	{
		"/bigobj"
	}

	files
	{
		"%{prj.name}/Include/**.h",
		"%{prj.name}/Src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/Include",
		"Helios/Include",
		"Helios/Libs",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.rapidjson}",
		"%{IncludeDir.FMOD}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.eventpp}",
		"%{IncludeDir.eventpp}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Lua}",
		"%{IncludeDir.fxgltf}",
		"%{IncludeDir.nlohmann}"
	}

	libdirs
	{
		"%{LibDir.rttr}",
		"%{LibDir.FMOD}",
		"%{LibDir.optick}",
		"%{LibDir.Lua}"
	}

	links
	{
		"Helios",
		"gdi32.lib",
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		"d3dcompiler.lib",
		"OptickCore.lib",
		"lua53.lib"
	}

	defines
	{
		"LUA53",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	warnings "extra"

	flags
	{
		"FatalWarnings",
		"MultiProcessorCompile"
	}

	ignoredefaultlibraries
	{
		"libcmt.lib"
	}
	
	postbuildcommands 
	{
		"../BuildScripts/CopyBuildResources.bat $(SolutionDir) " ..OutputDir.. " $(ProjectName) $(Configuration)"
	}

	--Debug
	filter "configurations:Debug-Editor or configurations:No-Editor-Debug"
		defines 
		{
			"HE_DEBUG"
		}
		runtime "Debug"
		symbols "on"

		links
		{
			"librttr_core_d",
			"fmodL_vc",
			"fmodstudioL_vc"
		}

	--Release
	filter "configurations:Release or configurations:Release-Editor"
		defines
		{
			"HE_RELEASE"
		}
		runtime "Release"
		optimize "on"

		links
		{
			"librttr_core",
			"sfml-graphics-s",
			"sfml-system-s",
			"sfml-window-s",
			"fmod_vc",
			"fmodstudio_vc"
		}
