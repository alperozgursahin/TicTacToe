#include <tchar.h>
#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[] = _T("CodeBlocksWindowsApp");
HBRUSH hBrush;
HFONT hFont;
HWND hP1TextBox = NULL, hP2TextBox = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    WNDCLASSEX wincl = {
        sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure, 0, 0, hInstance, LoadIcon(NULL, IDI_APPLICATION),
        LoadCursor(NULL, IDC_ARROW), hBrush = CreateSolidBrush(RGB(173, 216, 230)), NULL,
        szClassName, LoadIcon(NULL, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&wincl)) return 0;

    HWND hwnd = CreateWindowEx(0, szClassName, _T("TicTacToe"), WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, HWND_DESKTOP, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    MSG messages;
    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

void CreateControls(HWND hwnd) {
    hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    auto SetControl = [&](LPCTSTR type, LPCTSTR text, DWORD style, int x, int y, int w, int h, HMENU id = NULL) {
        HWND hCtrl = CreateWindow(type, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwnd, id, NULL, NULL);
        SendMessage(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        return hCtrl;
    };

    SetControl(_T("STATIC"), _T("TIC TAC TOE"), SS_CENTER, 300, 30, 200, 30);
    SetControl(_T("STATIC"), _T("PLAYER 1"), SS_CENTER, 150, 150, 100, 30);
    SetControl(_T("STATIC"), _T("PLAYER 2"), SS_CENTER, 550, 150, 100, 30);

    hP1TextBox = SetControl(_T("EDIT"), _T("P1 Name"), WS_BORDER, 100, 200, 200, 40);
    hP2TextBox = SetControl(_T("EDIT"), _T("P2 Name"), WS_BORDER, 500, 200, 200, 40);

    SetControl(_T("BUTTON"), _T("Start"), BS_PUSHBUTTON, 300, 400, 200, 50, (HMENU)ID_BUTTON_START);
}

void CreateMenuBar(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hOptionsMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hOptionsMenu, _T("Options"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, _T("Help"));

    AppendMenu(hOptionsMenu, MF_STRING, ID_MENU_OPTIONS, _T("Settings"));
    AppendMenu(hHelpMenu, MF_STRING, ID_MENU_HELP, _T("About"));

    SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            CreateControls(hwnd);
            CreateMenuBar(hwnd);  // Menü çubuðunu oluþtur
            break;
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            return (LRESULT)hBrush;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BUTTON_START:
                    MessageBox(hwnd, _T("Start Button Clicked"), _T("Game Mode"), MB_OK);
                    break;
                case ID_MENU_OPTIONS:
                    MessageBox(hwnd, _T("Settings option clicked."), _T("Options"), MB_OK);
                    break;
                case ID_MENU_HELP:
                    MessageBox(hwnd, _T("TicTacToe Game v1.0\nMade by Alper"), _T("About"), MB_OK);
                    break;
            }
            break;
        case WM_DESTROY:
            DeleteObject(hBrush);
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
