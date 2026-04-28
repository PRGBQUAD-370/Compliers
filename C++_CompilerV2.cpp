#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>

std::string selectedFile;
int arch = 64;

// ----------------------------
// Check file exists
// ----------------------------
bool fileExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

// ----------------------------
// Find Visual Studio vcvars
// ----------------------------
std::string findVCVars() {
    std::vector<std::string> bases = {
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\",
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\",
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Auxiliary\\Build\\",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\"
    };

    for (auto& b : bases) {
        if (fileExists(b + "vcvars64.bat")) {
            return b;
        }
    }
    return "";
}

// ----------------------------
// File picker
// ----------------------------
void PickFile(HWND hwnd) {
    char fileName[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "C/C++ Files (*.c;*.cpp)\0*.c;*.cpp\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        selectedFile = fileName;
        SetWindowTextA(hwnd, selectedFile.c_str());
    }
}

// ----------------------------
// Base name
// ----------------------------
std::string getBaseName(const std::string& path) {
    size_t s = path.find_last_of("\\/");
    size_t d = path.find_last_of(".");
    return path.substr(s + 1, d - s - 1);
}

// ----------------------------
// FIXED COMPILE (MAIN FIX)
// ----------------------------
void Compile(HWND hwnd) {
    if (selectedFile.empty()) {
        MessageBoxA(hwnd, "No file selected.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string basePath = findVCVars();
    if (basePath.empty()) {
        MessageBoxA(hwnd,
            "Visual Studio C++ tools not found.",
            "Error",
            MB_OK | MB_ICONERROR);
        return;
    }

    std::string vcvars =
        basePath + (arch == 64 ? "vcvars64.bat" : "vcvars32.bat");

    std::string output = getBaseName(selectedFile) + ".exe";

    // ✔ FIXED: correct MSVC order + proper /link usage
    std::string command =
        "cmd.exe /k \"call \"" + vcvars +
        "\" && cl \"" + selectedFile +
        "\" /EHsc /O2 /MD /Fe:" + output +
        " /link user32.lib gdi32.lib winmm.lib Msimg32.lib comdlg32.lib"
        " && echo BUILD SUCCESS && pause\"";

    system(command.c_str());
}

// ----------------------------
// Window procedure
// ----------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE:
        CreateWindowA("BUTTON", "Select File", WS_VISIBLE | WS_CHILD,
            20, 20, 120, 30, hwnd, (HMENU)1, NULL, NULL);

        CreateWindowA("BUTTON", "Compile", WS_VISIBLE | WS_CHILD,
            160, 20, 120, 30, hwnd, (HMENU)2, NULL, NULL);

        CreateWindowA("BUTTON", "x86", WS_VISIBLE | WS_CHILD,
            20, 70, 60, 30, hwnd, (HMENU)3, NULL, NULL);

        CreateWindowA("BUTTON", "x64", WS_VISIBLE | WS_CHILD,
            100, 70, 60, 30, hwnd, (HMENU)4, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: PickFile(hwnd); break;
        case 2: Compile(hwnd); break;
        case 3: arch = 32; MessageBoxA(hwnd, "x86 selected", "Info", MB_OK); break;
        case 4: arch = 64; MessageBoxA(hwnd, "x64 selected", "Info", MB_OK); break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ----------------------------
// Entry point
// ----------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "CompilerGUI";

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "CompilerGUI",
        "C++_ComplierV2.exe",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        320, 180,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}