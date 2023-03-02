# learned from https://github.com/colmap/colmap/blob/9f3a75ae9c72188244f2403eb085e51ecf4397a8/cmake/CMakeHelper.cmake
macro(EDITOR_ADD_SOURCE)
	set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES "^/.*")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                 "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(EditorSourceFiles ${EditorSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(EDITOR_ADD_SOURCE)
# end from

set(EditorSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/EditorLayer.h" "${CMAKE_CURRENT_SOURCE_DIR}/EditorLayer.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/FlameEditorApp.cpp")

add_subdirectory(Panels)
add_subdirectory(ImGuiWrapper)
add_subdirectory(IconManager)
add_subdirectory(RawRenderTest)

source_group(TREE ${EngineEditorDir} FILES ${EditorSourceFiles})

add_executable(Flame-Editor ${EditorSourceFiles})


add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}") #预定义

target_link_libraries(Flame-Editor PRIVATE Flame-Runtime)

target_include_directories(Flame-Editor PRIVATE
	"${EngineSourceDir}"
	"${EngineRuntimeDir}"
)

target_compile_definitions(Flame-Editor PRIVATE
	FLAME_PLATFORM_WINDOWS
	FLAME_DEBUG
	_CRT_SECURE_NO_WARNINGS
)

set_target_properties(Flame-Editor PROPERTIES FOLDER "Engine")

if(MSVC)
	set_target_properties(
	Flame-Editor PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${EngineEditorDir}")
endif()
