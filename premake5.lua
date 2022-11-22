workspace "Flame"
    architecture "x64"
    startproject "Flame-Editor"

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
IncludeDir["stb_image"] = "Flame/vendor/stb_image"
IncludeDir["entt"] = "Flame/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Flame/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Flame/vendor/ImGuizmo"
IncludeDir["box2d"] = "Flame/vendor/box2d/include"

group "Dependency"
include "Flame/vendor/GLFW"
include "Flame/vendor/Glad"
include "Flame/vendor/imgui"
include "Flame/vendor/yaml-cpp"
include "Flame/vendor/box2d" --必须要有premake5.lua才能include
group ""

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
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "Flame/vendor/ImGuizmo/ImGuizmo.h",
        "Flame/vendor/ImGuizmo/ImGuizmo.cpp",
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
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.box2d}",
    }

    links
    {
        "GLFW", 
        "Glad",
        "ImGui",
        "yaml-cpp",
        "opengl32.lib",
        "box2d",
    }

    -- 对于ImGuizmo里的cpp, 不使用PCH, 因为我并不想去修改Submodule里它的cpp文件
	-- 改了以后, 单独的submodule就不可以编译了
	filter "files:Flame/vendor/ImGuizmo/ImGuizmo.cpp"
    flags { "NoPCH" }

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
            "%{IncludeDir.entt}",
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

project "Flame-Editor"
    location "Flame-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

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
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
    }

    links
    {
        "Flame"
    }

    filter "system:windows"
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