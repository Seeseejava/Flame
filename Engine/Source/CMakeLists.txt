set(ThirdPartyDir "${EngineSourceDir}/ThirdParty")
set(EngineRuntimeDir "${EngineSourceDir}/Runtime")
set(EngineEditorDir "${EngineSourceDir}/Editor")
set(EngineScriptsDir "${EngineSourceDir}/Scripts")

file(COPY ${EngineScriptsDir}
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(WITH_PYTHON OFF) # will judge in ThirdParty/pybind11

add_subdirectory(ThirdParty)

if(WITH_PYTHON)
    add_compile_definitions(WITH_PYTHON)
endif()

add_subdirectory(Runtime)
add_subdirectory(Editor)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Flame-Editor)