file(GLOB_RECURSE RuntimeHeaderFiles CONFIGUE_DEPENDS "*.h")
file(GLOB_RECURSE RuntimeSourceFiles CONFIGUE_DEPENDS "*.cpp")

source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeHeaderFiles} ${RuntimeSourceFiles})


add_library(Flame-Runtime STATIC ${RuntimeHeaderFiles} ${RuntimeSourceFiles})

#链接了Flame的模块，不会包含这些文件
target_include_directories(Flame-Runtime 
	PRIVATE "${EngineSourceDir}"
    	PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "flamepch.h"
)

target_compile_definitions(Flame-Runtime

    PRIVATE FLAME_PLATFORM_WINDOWS
    PRIVATE FLAME_BUILD_DLL
    PRIVATE GLFW_INCLUDE_NONE
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    PRIVATE FLAME_DEBUG
   "ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

target_precompile_headers(Flame-Runtime PUBLIC "flamepch.h")


target_link_libraries(Flame-Runtime
    PUBLIC glfw
    PUBLIC Glad
    PUBLIC ${OPENGL_LIBRARIES}
    PUBLIC yaml-cpp
    PUBLIC spdlog
    PUBLIC headers
    PUBLIC glm
    PUBLIC entt
    PUBLIC Box2D
    PUBLIC stb
    PUBLIC ImGuizmo
    PUBLIC assimp
    PUBLIC BulletSoftBody
    PUBLIC Bullet3Dynamics 
    PUBLIC BulletDynamics 
    PUBLIC Bullet3Collision 
    PUBLIC BulletCollision 
    PUBLIC Bullet3Common 
    PUBLIC LinearMath  # ？
    PUBLIC pybind11::embed pybind11::module pybind11::pybind11
    PUBLIC magic_enum
)


