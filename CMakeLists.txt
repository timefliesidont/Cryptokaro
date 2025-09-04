# =============================================================================
# 1. Project Setup
# =============================================================================
cmake_minimum_required(VERSION 3.16)
project(CryptoDesktop CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# HIGHLIGHT: The executable name has been changed here
set(EXECUTABLE_NAME cryptokaro)

# =============================================================================
# 2. Add Dependencies from the 'extern' folder
# =============================================================================
# For Crypto++: This will build the library from source.
# It assumes the cryptopp source folder contains a CMakeLists.txt
add_subdirectory(extern/cryptopp)

# For nlohmann/json and webview, we only need to add them to the include path,
# which is handled in section 4.

# =============================================================================
# 3. Define The Executable
# =============================================================================
# Define our application executable with its source files.
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/backend.cpp
)

# =============================================================================
# 4. Set Include Directories
# =============================================================================
# This tells the compiler where to find our header files.
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # For backend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern    # For webview/ and nlohmann/
)

# =============================================================================
# 5. Link Libraries (Platform-Specific)
# =============================================================================

# Always link against our compiled Crypto++ library
target_link_libraries(${EXECUTABLE_NAME} PRIVATE cryptopp)

# --- Platform-specific linking for Webview ---
if(WIN32)
    # On Windows, we need the WebView2 SDK and standard Win32 libs
    include(FetchContent)
    FetchContent_Declare(
        WebView2
        GIT_REPOSITORY https://github.com/MicrosoftEdge/WebView2Feedback.git
        GIT_TAG 1.0.1264.42 # Use a specific, stable version
    )
    FetchContent_MakeAvailable(WebView2)

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ole32 comctl32 oleaut32 uuid gdi32 advapi32 shell32 user32
        ${WEBVIEW2_LIBRARY} # From FetchContent
    )
    # Ensure the loader DLL is copied to the output directory
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${WEBVIEW2_RUNTIME_DLL}
                       $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)

elseif(APPLE)
    # On macOS, link against the WebKit framework and create an App bundle
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework WebKit")

else()
    # On Linux, use pkg-config to find and link WebKitGTK
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${WEBKIT_LIBRARIES} pthread)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${WEBKIT_INCLUDE_DIRS})
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ${WEBKIT_CFLAGS_OTHER})
endif()

# =============================================================================
# 6. Copy Runtime Resources
# =============================================================================
# This command ensures your 'www' and 'config' folders are copied to the
# build directory next to your executable after a successful build.
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/www
            $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/www
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json
            $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/config.json
    COMMENT "Copying runtime assets..."
)
