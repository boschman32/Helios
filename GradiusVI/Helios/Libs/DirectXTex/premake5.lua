project "DirectXTex"
	kind "StaticLib"
	language "C++"
	
	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("obj/" .. OutputDir .. "/%{prj.name}")
	
	pchheader "DirectXTexP.h"
	pchsource "DirectXTex/DirectXTexP.cpp"
	
	files
	{
		"DirectXTex/**.h",
		"DirectXTex/**.cpp",
		"DirectXTex/Shaders/CompileShaders.cmd",
		"DirectXTex/Shaders/Compiled/**"
	}
	
	includedirs
	{
		"%{IncludeDir.DirectXTex}"
	}

	flags
	{
		"MultiProcessorCompile"
	}
	
	filter { "files:**.hlsl" }
		removeflags "ExcludeFromBuild"
		shadertype "Compute"		
		
	defines
	{
		"_UNICODE",
		"UNICODE",
		"WIN32",
		"_LIB",
		"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS"
	}
	
	filter "configurations:Debug-Editor or configurations:No-Editor-Debug"
		defines "_DEBUG"
		runtime "Debug"
		
	filter "configurations:Release or configurations:Release-Editor"
		defines "NDEBUG"
		runtime "Release"
		