#include "backend.hpp"
#include "globals.hpp" // Include our new globals header
#include <webview/webview.h>

#ifdef _WIN32
#include <windows.h>
#include <string>
#include <vector>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Helper function to get the directory of the executable in a cross-platform way
std::string getExecutableDir() {
#ifdef _WIN32
    std::vector<char> pathBuf;
    DWORD copied = 0;
    do {
        pathBuf.resize(pathBuf.size() + MAX_PATH);
        copied = GetModuleFileNameA(NULL, &pathBuf.at(0), pathBuf.size());
    } while (copied >= pathBuf.size());
    pathBuf.resize(copied);
    std::string path(pathBuf.begin(), pathBuf.end());
    return path.substr(0, path.find_last_of("\\/"));
#elif defined(__APPLE__)
    // This part is a placeholder for macOS and might need a more robust solution
    // using Objective-C or CoreFoundation for production apps. For now, it assumes
    // the executable is in a known location relative to the bundle.
    return "."; // This will be handled correctly by the resource path logic
#else
    // Linux specific implementation
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = std::string(result, (count > 0) ? count : 0);
    return path.substr(0, path.find_last_of("/"));
#endif
}

int main() {
    webview::webview w(true, nullptr);
    w.set_title("CryptoKaro");
    w.set_size(800, 600, WEBVIEW_HINT_NONE);

    // --- UPDATED LOGIC TO FIND AND STORE RESOURCES ---
    std::string resourcePath;

#ifdef __APPLE__
    // On macOS, we must use the CoreFoundation APIs to find the bundle's Resources directory.
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        // Error handling if path not found
        return 1;
    }
    CFRelease(resourcesURL);
    resourcePath = std::string(path) + "/";
#else
    // On Windows and Linux, the resources are next to the executable.
    resourcePath = getExecutableDir() + "/";
#endif

    // Store the calculated path in our new global variable for the backend handlers to use.
    G_RESOURCE_PATH = resourcePath;

    std::string fileURL = "file://" + resourcePath + "www/index.html";
    
    w.bind("invoke", invoke);
    w.navigate(fileURL);
    w.run();

    return 0;
}

