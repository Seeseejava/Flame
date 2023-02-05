set(bullet3SourceDir ${ThirdPartyDir}/bullet3)

file(GLOB_RECURSE bullet3Src
    "${bullet3SourceDir}/src/Bullet3Collision/*.cpp"
    "${bullet3SourceDir}/src/*.cpp"
)

add_library(bullet3 STATIC ${bullet3Src})
target_include_directories(bullet3 PUBLIC "${bullet3SourceDir}/src")