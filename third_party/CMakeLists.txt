#add_library(vma INTERFACE)

#target_include_directories(vma INTERFACE vma)

add_subdirectory(vma)

add_subdirectory(SPIRV-Reflect)

add_subdirectory(abseil-cpp)