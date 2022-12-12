set(StbSourceDir ${ThirdPartyDir}/stb_image)

file(GLOB_RECURSE box2dSrc 
    "${StbSourceDir}/*.h"
    "${StbSourceDir}/*.cpp"
)
add_library(stb STATIC ${box2dSrc})
target_include_directories(stb PUBLIC "${StbSourceDir}")