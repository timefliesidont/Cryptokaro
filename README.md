# CryptoKaro
A lightweight, cross-platform desktop application for common cryptographic operations, built with C++ and a modern web-based UI.

## About The Project
CryptoKaro provides a simple, secure, and intuitive interface for essential cryptographic tasks. Unlike heavy frameworks like Electron, this application uses the native web rendering engine on each operating system (via the Webview library), resulting in a tiny executable and minimal resource usage.

The backend logic is powered by the robust and time-tested Crypto++ library, ensuring that all operations are performed securely and efficiently.

### Features
* Symmetric Key Generation: Create cryptographically secure random keys for AES-256.

* AES-256 Encryption & Decryption: Encrypt and decrypt text data using a secure key and a randomly generated IV for each operation.

* SHA-256 Digest Generation: Calculate the SHA-256 hash of any text input.

* HMAC-SHA256 Digest Generation: Create a Hash-based Message Authentication Code to verify data integrity and authenticity.

### Tech Stack
* **Backend:** C++17

* **Cryptography:** Crypto++

* **GUI Framework:** Webview

* **Build System:** CMake

* **JSON Handling:** nlohmann/json

* **Frontend:** HTML5, CSS3, JavaScript (Single Page Application)

## Getting Started
Follow these steps to build and run the application on your local machine.

### Prerequisites
1. You must have a C++17 compatible compiler and CMake installed.

2. C++ Compiler (e.g., GCC, Clang, or MSVC)

3. CMake (version 3.16 or higher)

4. Additionally, you need the platform-specific development libraries for Webview:

    * **macOS:** Xcode Command Line Tools are required.

        - Install by running: xcode-select --install

    * **Linux (Debian/Ubuntu):** The WebKit2GTK development library is required.

        - Install by running: sudo apt-get install libwebkit2gtk-4.0-dev

    * **Windows:** Visual Studio 2019 or later with the "Desktop development with C++" workload is required. The WebView2 runtime is typically pre-installed on modern Windows 10/11 systems.

### Build & Run Instructions
1. Clone the repository:
```
git clone <your-repo-url>
cd crypto-desktop
```
2. Create a build directory:
```
mkdir build
cd build
```
3. Configure the project with CMake:
```
cmake ..
```
4. Build the application:
```
cmake --build .
```
5. Run the executable:

    * **macOS:** The application bundle cryptokaro.app will be in the build directory. You can run it from Finder or use the terminal:

        - `open cryptokaro.app`

    * **Linux:** The executable cryptokaro will be in the build directory.

        - `./cryptokaro`

    * **Windows:** The executable cryptokaro.exe will be inside a Debug or Release folder within the build directory.

## Project Structure
```
crypto-desktop/
├── CMakeLists.txt      # Main build script for the entire project
├── config/             # Runtime configuration files (e.g., config.json)
├── extern/             # Third-party libraries (Crypto++, Webview, etc.)
├── include/            # C++ header files (.hpp) for our application logic
├── src/                # C++ source files (.cpp) for our application logic
└── www/                # All frontend assets (HTML, CSS, JS)
```
