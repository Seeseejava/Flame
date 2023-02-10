

file(GLOB EditorHeaderFiles CONFIGUE_DEPENDS "*.h" "Panels/*.h")
file(GLOB EditorSourceFiles CONFIGUE_DEPENDS "*.cpp" "Panels/*.cpp")

source_group(TREE ${EngineEditorDir} FILES ${EditorHeaderFiles} ${EditorSourceFiles})

add_executable(Flame-Editor ${EditorHeaderFiles} ${EditorSourceFiles})


#add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}") #预定义

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

if(MSVC)
	set_target_properties(
	Flame-Editor PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${EngineEditorDir}")
endif()
