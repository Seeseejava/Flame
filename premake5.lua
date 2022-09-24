workspace "Flame"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
--Include directories relative to root folder(solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Flame/vendor/GLFW/include"
IncludeDir["Glad"] = "Flame/vendor/Glad/include"
IncludeDir["ImGui"] = "Flame/vendor/imgui"
IncludeDir["glm"] = "Flame/vendor/glm"


include "Flame/vendor/GLFW"
include "Flame/vendor/Glad"
include "Flame/vendor/imgui"


project "Flame"
    location "Flame"
    --kind "SharedLib" --这样可以include a static library
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    --staticruntime "off"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "flamepch.h"
    pchsource "Flame/src/flamepch.cpp"
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }
    
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "GLFW", 
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        --cppdialect "C++17"
        systemversion "latest"

        defines 
        {
            "FLAME_PLATFORM_WINDOWS",
            "FLAME_BUILD_DLL",   
            "GLFW_INCLUDE_NONE"
        }

        -- postbuildcommands
        -- {
        --     --("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        --     --优化第一次build失败问题
        --     -- %{cfg.buildtarget.relpath}是生成文件，相较于当前premake5.lua文件的相对路径
        --     ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")

        -- }

    filter "configurations:Debug"
        defines "FLAME_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "FLAME_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "FLAME_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        --staticruntime "Off"--这里为什么还是设置未off
        staticruntime "on" --表示会link对应的dll？

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
        }

        includedirs
        {
            "Flame/vendor/spdlog/include",
            "Flame/src",
            "Flame/vendor",
            "%{IncludeDir.glm}",
        }

        links
        {
            "Flame"
        }

    filter "system:windows"
        --cppdialect "C++17"
        --staticruntime "On"
        systemversion "latest"

        defines 
        {
            "FLAME_PLATFORM_WINDOWS";         
        }


    filter "configurations:Debug"
        defines "FLAME_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "FLAME_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "FLAME_DIST"
        runtime "Release"
        optimize "on"