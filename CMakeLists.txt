# =============================================================================
# 1. Project Setup
# =============================================================================
cmake_minimum_required(VERSION 3.16)
project(CryptoDesktop CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_NAME cryptokaro)

# =============================================================================
# 2. Add Dependencies from the 'extern' folder
# =============================================================================
add_subdirectory(extern/cryptopp)

# =============================================================================
# 3. Define The Executable (UPDATED)
# =============================================================================
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/backend.cpp
    src/globals.cpp          # HIGHLIGHT: Add our new global source file
    src/handlers/hash_handler.cpp
    src/handlers/aes_handler.cpp
    src/handlers/key_handler.cpp
)

# =============================================================================
# 4. Set Include Directories
# =============================================================================
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
)

# =============================================================================
# 5. Link Libraries (Platform-Specific)
# =============================================================================
target_link_libraries(${EXECUTABLE_NAME} PRIVATE cryptopp)

if(WIN32)
    include(FetchContent)
    FetchContent_Declare(
        WebView2
        GIT_REPOSITORY https://github.com/MicrosoftEdge/WebView2Feedback.git
        GIT_TAG 1.0.1264.42
    )
    FetchContent_MakeAvailable(WebView2)

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ole32 comctl32 oleaut32 uuid gdi32 advapi32 shell32 user32
        ${WEBVIEW2_LIBRARY}
    )
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${WEBVIEW2_RUNTIME_DLL}
                       $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)

elseif(APPLE)
    # On macOS, link against WebKit and the CoreFoundation frameworks.
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework WebKit" "-framework CoreFoundation")

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
if(APPLE)
    # On macOS, copy resources into the app bundle's Resources directory.
    set(RESOURCE_DIR "${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME}.app/Contents/Resources")
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/www
                ${RESOURCE_DIR}/www
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json
                ${RESOURCE_DIR}/config.json
        COMMENT "Copying macOS bundle assets..."
    )
else()
    # For Windows and Linux, copy next to the executable.
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/www
                $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/www
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json
                $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/config.json
        COMMENT "Copying runtime assets..."
    )
endif()

