#include <tchar.h>
#include <windows.h>
#include "resource.h"
#include <cstdio>

// Window procedure declaration
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Global variables for window class, brush, font, and controls
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");
HBRUSH hBrush;
HFONT hFont;

// Handles for controls like text boxes, labels, buttons, etc.
HWND hP1TextBox = NULL, hP2TextBox = NULL, hStartButton = NULL;
HWND hP1Label = NULL, hP2Label = NULL, hTurnLabel = NULL, hVersusLabel = NULL;
HWND hGridButtons[3][3];

// Player tracking (X or O) and name storage
bool isPlayerX = true; // Tracks if the current player is X
TCHAR player1Name[100], player2Name[100];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    // Define window class properties
    WNDCLASSEX wincl = {
        sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure, 0, 0, hInstance,
        LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
        hBrush = CreateSolidBrush(RGB(173, 216, 230)), NULL,
        szClassName, LoadIcon(NULL, IDI_APPLICATION)
    };

    // Register the window class and check if registration was successful
    if (!RegisterClassEx(&wincl)) return 0;

    // Create main window
    HWND hwnd = CreateWindowEx(0, szClassName, _T("TicTacToe"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, HWND_DESKTOP, NULL, hInstance, NULL);

    // Display the window
    ShowWindow(hwnd, nCmdShow);

    // Main message loop
    MSG messages;
    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}

// Initializes and displays the start screen elements
void CreateStartScreen(HWND hwnd) {
    hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    // Helper function to create and configure controls
    auto SetControl = [&](LPCTSTR type, LPCTSTR text, DWORD style, int x, int y, int w, int h, HMENU id = NULL) {
        HWND hCtrl = CreateWindow(type, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwnd, id, NULL, NULL);
        SendMessage(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        return hCtrl;
    };

    // Create title label
    SetControl(_T("STATIC"), _T("TIC TAC TOE"), SS_CENTER, 300, 30, 200, 30);

    // Create player labels and text boxes
    hP1Label = SetControl(_T("STATIC"), _T("(X) PLAYER 1"), SS_CENTER, 50, 150, 150, 50);
    hP2Label = SetControl(_T("STATIC"), _T("(O) PLAYER 2"), SS_CENTER, 575, 150, 150, 50);

    // Create text boxes for player names
    hP1TextBox = SetControl(_T("EDIT"), _T("P1 Name"), WS_BORDER, 25, 200, 200, 40);
    hP2TextBox = SetControl(_T("EDIT"), _T("P2 Name"), WS_BORDER, 550, 200, 200, 40);

    // Start button
    hStartButton = SetControl(_T("BUTTON"), _T("Start"), BS_PUSHBUTTON, 300, 400, 200, 50, (HMENU)ID_BUTTON_START);

    // Load and display "versus" icon
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(ID_VERSUS_ICON), IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR);
    if (hIcon) {
        hVersusLabel = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | SS_ICON, 330, 150, 200, 200, hwnd, (HMENU)ID_VERSUS_ICON, (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
        SendMessage(hVersusLabel, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
    }
}

// Creates and sets up the menu bar
void CreateMenuBar(HWND hwnd) {
    HMENU hMenu = CreateMenu(), hOptionsMenu = CreateMenu(), hHelpMenu = CreateMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hOptionsMenu, _T("Options"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, _T("Help"));
    AppendMenu(hOptionsMenu, MF_STRING, ID_MENU_OPTIONS, _T("Settings"));
    AppendMenu(hHelpMenu, MF_STRING, ID_MENU_HELP, _T("About"));
    SetMenu(hwnd, hMenu);
}

// Creates the 3x3 game grid for Tic-Tac-Toe
void CreateGameGrid(HWND hwnd) {
    int xOffset = 250, yOffset = 150, buttonSize = 100;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            hGridButtons[i][j] = CreateWindow(_T("BUTTON"), _T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                              xOffset + j * buttonSize, yOffset + i * buttonSize, buttonSize, buttonSize,
                                              hwnd, (HMENU)(ID_BUTTON_START + 1 + i * 3 + j), NULL, NULL);
            SendMessage(hGridButtons[i][j], WM_SETFONT, (WPARAM)hFont, TRUE);
        }
    }
}

// Resets the grid to start a new game
void ResetGameGrid() {
    for (auto& row : hGridButtons) {
        for (HWND hButton : row) {
            SetWindowText(hButton, _T(""));
        }
    }
    isPlayerX = false;
}

// Updates the turn label text with the current player's turn
void SetTurnLabelText() {
    TCHAR turnText[100];
    _stprintf_s(turnText, _T("%s's Turn"), isPlayerX ? player1Name : player2Name);
    SetWindowText(hTurnLabel, turnText);
}

// Checks for a winner by examining rows, columns, and diagonals
bool CheckWinner() {
    TCHAR text1[2], text2[2], text3[2];
    for (int i = 0; i < 3; i++) {
        // Check rows
        GetWindowText(hGridButtons[i][0], text1, 2);
        GetWindowText(hGridButtons[i][1], text2, 2);
        GetWindowText(hGridButtons[i][2], text3, 2);
        if (_tcscmp(text1, text2) == 0 && _tcscmp(text2, text3) == 0 && text1[0] != _T('\0'))
            return true;
    }
    for (int j = 0; j < 3; j++) {
        // Check columns
        GetWindowText(hGridButtons[0][j], text1, 2);
        GetWindowText(hGridButtons[1][j], text2, 2);
        GetWindowText(hGridButtons[2][j], text3, 2);
        if (_tcscmp(text1, text2) == 0 && _tcscmp(text2, text3) == 0 && text1[0] != _T('\0'))
            return true;
    }
    // Check diagonals
    GetWindowText(hGridButtons[0][0], text1, 2);
    GetWindowText(hGridButtons[1][1], text2, 2);
    GetWindowText(hGridButtons[2][2], text3, 2);
    if (_tcscmp(text1, text2) == 0 && _tcscmp(text2, text3) == 0 && text1[0] != _T('\0'))
        return true;
    GetWindowText(hGridButtons[0][2], text1, 2);
    GetWindowText(hGridButtons[1][1], text2, 2);
    GetWindowText(hGridButtons[2][0], text3, 2);
    return _tcscmp(text1, text2) == 0 && _tcscmp(text2, text3) == 0 && text1[0] != _T('\0');
}

// Shows the winner and resets the game
void ShowWinner() {
    if (CheckWinner()) {
        TCHAR message[100];
        _stprintf_s(message, _T("Congratulations %s, you won!"), isPlayerX ? player1Name : player2Name);
        MessageBox(NULL, message, _T("Winner"), MB_OK);
        ResetGameGrid();
    }
}

// Handles click events on the grid
void HandleGridButtonClick(HWND hButton) {
    TCHAR text[2];
    GetWindowText(hButton, text, 2);
    if (text[0] == _T('\0')) {
        SetWindowText(hButton, isPlayerX ? _T("X") : _T("O"));
        ShowWinner();
        isPlayerX = !isPlayerX;
        SetTurnLabelText();
    }
}

// Processes Windows messages and routes them accordingly
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            CreateStartScreen(hwnd);
            CreateMenuBar(hwnd);
            break;
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetBkColor(hdcStatic, RGB(173, 216, 230));  // Label background color
            return (INT_PTR)hBrush;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BUTTON_START:
                    MessageBox(hwnd, _T("Game is starting..."), _T("Game Mode"), MB_OK);
                    break;
                case ID_MENU_OPTIONS:
                    MessageBox(hwnd, _T("Settings option clicked."), _T("Options"), MB_OK);
                    break;
                case ID_MENU_HELP:
                    MessageBox(hwnd, _T("TicTacToe Game v1.0\nMade by Alper"), _T("About"), MB_OK);
                    break;
            }

            if (LOWORD(wParam) == ID_BUTTON_START) {
                GetWindowText(hP1TextBox, player1Name, 100);
                GetWindowText(hP2TextBox, player2Name, 100);
                DestroyWindow(hP1TextBox);
                DestroyWindow(hP2TextBox);
                DestroyWindow(hStartButton);
                CreateGameGrid(hwnd);
                hTurnLabel = CreateWindow(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 250, 450, 300, 40, hwnd, NULL, NULL, NULL);
                SendMessage(hTurnLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
                SetTurnLabelText();
            } else if (LOWORD(wParam) >= ID_BUTTON_START + 1 && LOWORD(wParam) <= ID_BUTTON_START + 9) {
                HandleGridButtonClick((HWND)lParam);
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
