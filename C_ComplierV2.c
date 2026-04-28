#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <string.h>

char selectedFile[MAX_PATH] = "";
int arch = 64;

// ----------------------------
// file picker
// ----------------------------
void PickFile(HWND hwnd) {
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = selectedFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "C Files\0*.c\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        SetWindowTextA(hwnd, selectedFile);
    }
}

// ----------------------------
// base name
// ----------------------------
void getBaseName(const char* path, char* out) {
    const char* slash = strrchr(path, '\\');
    const char* dot = strrchr(path, '.');

    if (!slash) slash = path; else slash++;
    if (!dot) dot = path + strlen(path);

    strncpy_s(out, 260, slash, dot - slash);
    out[dot - slash] = '\0';
}

// ----------------------------
// COMPILE (FIXED LINK STAGE)
// ----------------------------
void Compile(HWND hwnd) {
    if (selectedFile[0] == '\0') {
        MessageBoxA(hwnd, "No file selected", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    char base[260];
    getBaseName(selectedFile, base);

    const char* vcvars = (arch == 64)
        ? "vcvars64.bat"
        : "vcvars32.bat";

    char cmd[2048];

    // CLEAN FIX: correct /link usage
    sprintf_s(cmd, sizeof(cmd),
        "cmd.exe /k \""
        "\"%%ProgramFiles(x86)%%\\Microsoft Visual Studio\\Installer\\vswhere.exe\" "
        "-latest -products * "
        "-property installationPath > vs_path.txt && "
        "set /p VS=<vs_path.txt && "
        "call \"%%VS%%\\VC\\Auxiliary\\Build\\%s\" && "
        "cl \"%s\" /TC /EHsc /O2 /MD /Fe:%s.exe /link "
        "user32.lib gdi32.lib winmm.lib Msimg32.lib comdlg32.lib && "
        "echo BUILD SUCCESS && pause\"",
        vcvars,
        selectedFile,
        base
    );

    system(cmd);
}

// ----------------------------
// WINDOW PROC
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

        case 1:
            PickFile(hwnd);
            break;

        case 2:
            Compile(hwnd);
            break;

        case 3:
            arch = 32;
            MessageBoxA(hwnd, "x86 selected", "Info", MB_OK);
            break;

        case 4:
            arch = 64;
            MessageBoxA(hwnd, "x64 selected", "Info", MB_OK);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ----------------------------
// ENTRY
// ----------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "CCompilerGUI";

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "CCompilerGUI",
        "C_ComplierV2.exe",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        320, 180,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}