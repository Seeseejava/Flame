macro(RUNTIME_ADD_SOURCE)
	set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES "^/.*")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                 "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(RuntimeSourceFiles ${RuntimeSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(RUNTIME_ADD_SOURCE)

set(RuntimeSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/Flame.h" "${CMAKE_CURRENT_SOURCE_DIR}/flamepch.h" "${CMAKE_CURRENT_SOURCE_DIR}/flamepch.cpp")


add_subdirectory(Animation)
add_subdirectory(Camera)
add_subdirectory(Core)
add_subdirectory(Debug)
add_subdirectory(ECS)
add_subdirectory(Events)
add_subdirectory(ImGui)
add_subdirectory(Library)
add_subdirectory(Math)
add_subdirectory(Mesh)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(Resource)
add_subdirectory(Utils)


source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeSourceFiles})

add_library(Flame-Runtime STATIC ${RuntimeSourceFiles})

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
    PUBLIC LinearMath
    PUBLIC pybind11::embed pybind11::module pybind11::pybind11
    PUBLIC magic_enum
    PUBLIC FMOD::Core FMOD::Fsbank FMOD::Studio
)


