# This CMakeLists.txt is designed to build the Crypto++ library from source.
# Place this file inside the root of the unzipped cryptopp source folder.

cmake_minimum_required(VERSION 3.10)

project(cryptopp CXX)

# Gather all the source files needed for the library.
# The `GLOB` command finds all files matching the pattern.
file(GLOB CRYPTOPP_SOURCES
    "*.cpp"
    "*.cc"
)

# Create the library target.
# We build it as a STATIC library, which means it will be compiled directly
# into your final executable. This makes distribution easier.
add_library(cryptopp STATIC ${CRYPTOPP_SOURCES})

# Tell CMake that this library's source directory should be added to the
# include path. This is crucial so that the .cpp files can find the .h files.
# 'PUBLIC' means that any other target that links to this one (your main app)
# will also inherit this include path.
target_include_directories(cryptopp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Optional: Set the C++ standard for this library if needed.
# set_target_properties(cryptopp PROPERTIES CXX_STANDARD 17)
