project "UnitTesting"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	staticruntime "off"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("obj/" .. OutputDir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "pch.cpp"

	buildoptions "/bigobj"

	files
	{
		"*.h",
		"*.cpp"
	}

	includedirs
	{
		"../Helios/Include",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.DirectXTex}",
		"../%{IncludeDir.spdlog}",
		"../%{IncludeDir.rttr}",
		"../%{IncludeDir.rapidjson}",
		"../%{IncludeDir.FMOD}",
		"../%{IncludeDir.optick}",
		"../%{IncludeDir.eventpp}",
		"../%{IncludeDir.eventpp}",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.Lua}",
		"../%{IncludeDir.fxgltf}",
		"../%{IncludeDir.nlohmann}",
		"$(VCInstallDir)UnitTest/include"
	}

	libdirs
	{
		"$(VCInstallDir)UnitTest/lib",
		"../%{LibDir.rttr}",
		"../%{LibDir.FMOD}",
		"../%{LibDir.Lua}",
		"../%{LibDir.optick_d}"
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
		"lua53.lib",
		"librttr_core_d",
		"fmodL_vc",
		"fmodstudioL_vc"
	}

	flags
	{
		"FatalWarnings",
		"MultiProcessorCompile"
	}

	ignoredefaultlibraries
	{
		"libcmt.lib"
	}
	
	filter "configurations:Debug-Editor or configurations:No-Editor-Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"
		defines 
		{
			"_DEBUG",
			"HE_DEBUG",
			"WITH_EDITOR",
			"GLM_ENABLE_EXPERIMENTAL"
		}

	filter "configurations:Release or configurations:Release-Editor"
		optimize "On"
		defines 
		{
			"_NDEBUG",
			"HE_RELEASE",
			"WITH_EDITOR"
		}