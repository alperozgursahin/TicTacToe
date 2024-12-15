#include <tchar.h>
#include <windows.h>
#include "resource.h"
#include <cstdio>
#include "tictactoedllheader.h"

// Window procedure declaration
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Global variables for window class, brush, font, and controls
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");
HBRUSH hBrush;
HFONT hFont, hScoreFont;

// Handles for controls like text boxes, labels, buttons, etc.
HWND hP1TextBox, hP2TextBox, hStartButton = NULL;
HWND hTitleLabel, hP1Label, hP1ScoreLabel, hP2Label, hP2ScoreLabel, hTurnLabel, hVersusLabel = NULL;
HWND hGridButtons[3][3];

// Player tracking (X or O) and name storage
bool isPlayerX = true; // Tracks if the current player is X
TCHAR player1Name[100], player2Name[100];
int roundsPlayed, player1Score, player2Score = 0;

bool isGameStarted = false;

TCHAR filePath[] = _T("game_results.txt");

HINSTANCE hinstDLL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    hinstDLL = LoadLibrary("TicTacToeDLL.dll");
    if (hinstDLL != 0)
    {
        MessageBox(NULL, _T("TicTacToeDLL Loaded"), _T("DLL Load"), MB_OK);
    }
    // Define window class properties
    WNDCLASSEX wincl =
    {
        sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure, 0, 0, hInstance,
        LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
        hBrush = CreateSolidBrush(RGB(173, 216, 230)), NULL,
        szClassName, LoadIcon(NULL, IDI_APPLICATION)
    };

    // Register the window class and check if registration was successful
    if (!RegisterClassEx(&wincl)) return 0;

    // Create main window
    HWND hwnd = CreateWindowEx(0, szClassName, _T("TicTacToe"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, HWND_DESKTOP, LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1)), hInstance, NULL);

    // Display the window
    ShowWindow(hwnd, nCmdShow);

    // Main message loop
    MSG messages;
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}


// Initializes and displays the start screen elements
void CreateStartScreen(HWND hwnd)
{

    hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    hScoreFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Impact"));

    // Helper function to create and configure controls
    auto SetControl = [&](LPCTSTR type, LPCTSTR text, DWORD style, int x, int y, int w, int h, HMENU id = NULL)
    {
        HWND hCtrl = CreateWindow(type, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwnd, id, NULL, NULL);
        SendMessage(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        return hCtrl;
    };

    // Create title label
    hTitleLabel = CreateWindow(_T("STATIC"), _T("TIC TAC TOE"), WS_CHILD | WS_VISIBLE | SS_CENTER, 300, 20, 200, 50, hwnd, NULL, NULL, NULL);
    SendMessage(hTitleLabel, WM_SETFONT, (WPARAM)hScoreFont, TRUE);

    // Create player labels and text boxes
    hP1Label = SetControl(_T("STATIC"), _T("(X) PLAYER 1"), SS_CENTER, 25, 150, 200, 50);
    hP2Label = SetControl(_T("STATIC"), _T("(O) PLAYER 2"), SS_CENTER, 550, 150, 200, 50);

    // Create text boxes for player names
    hP1TextBox = SetControl(_T("EDIT"), _T("P1 Name"), WS_BORDER, 25, 200, 200, 40);
    hP2TextBox = SetControl(_T("EDIT"), _T("P2 Name"), WS_BORDER, 550, 200, 200, 40);

    // Start button
    hStartButton = SetControl(_T("BUTTON"), _T("Start"), BS_PUSHBUTTON, 300, 400, 200, 50, (HMENU)ID_BUTTON_START);

    // Load and display "versus" icon
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(ID_VERSUS_ICON), IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR);
    if (hIcon)
    {
        hVersusLabel = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | SS_ICON, 330, 150, 200, 200, hwnd, (HMENU)ID_VERSUS_ICON, (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
        SendMessage(hVersusLabel, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
    }
}


// Resets the grid to start a new game
void ResetGameGrid()
{
    ResetGameGrid(hGridButtons, roundsPlayed, isPlayerX);
}


// Checks for a winner by examining rows, columns, and diagonals
bool CheckWinner()
{
    CheckWinner(hGridButtons);
}


void UpdateScore()
{
    char buffer[32];
    // Player 1 Score Update
    if (isPlayerX)
    {
        player1Score++;
        sprintf(buffer, "%d", player1Score);
        SetWindowText(hP1ScoreLabel, buffer);
    }
    // Player 2 Score Update
    else
    {
        player2Score++;
        sprintf(buffer, "%d", player2Score);
        SetWindowText(hP2ScoreLabel, buffer);
    }
}


bool IsGridFull()
{
    TCHAR text[2];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            GetWindowText(hGridButtons[i][j], text, 2);
            if (text[0] == _T('\0'))
            {
                return false; // If there is a null button, return false
            }
        }
    }
    return true; // All buttons are full
}

// Shows the winner and resets the game
void ShowWinner()
{
    if (CheckWinner())
    {
        TCHAR message[100];
        _stprintf_s(message, _T("Congratulations %s, you won!"), isPlayerX ? player1Name : player2Name);
        MessageBox(NULL, message, _T("Winner"), MB_OK);
        UpdateScore();
        ResetGameGrid();
    }
    else if (IsGridFull())
    {
        MessageBox(NULL, _T("The game is a draw!"), _T("Draw"), MB_OK);
        ResetGameGrid();
    }
}


// Updates the turn label text with the current player's turn
void SetTurnLabelText()
{
    TCHAR turnText[100];
    _stprintf_s(turnText, _T("%s's Turn"), isPlayerX ? player1Name : player2Name);
    SetWindowText(hTurnLabel, turnText);
}


// Handles click events on the grid
void HandleGridButtonClick(HWND hButton)
{
    TCHAR text[2];
    GetWindowText(hButton, text, 2);
    if (text[0] == _T('\0'))
    {
        SetWindowText(hButton, isPlayerX ? _T("X") : _T("O"));
        ShowWinner();
        isPlayerX = !isPlayerX;
        SetTurnLabelText();
    }
}


// Creates the 3x3 game grid for Tic-Tac-Toe
void CreateGameGrid(HWND hwnd)
{
    int xOffset = 250, yOffset = 150, buttonSize = 100;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            hGridButtons[i][j] = CreateWindow(_T("BUTTON"), _T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                              xOffset + j * buttonSize, yOffset + i * buttonSize, buttonSize, buttonSize,
                                              hwnd, (HMENU)(ID_BUTTON_START + 1 + i * 3 + j), NULL, NULL);
            SendMessage(hGridButtons[i][j], WM_SETFONT, (WPARAM)hFont, TRUE);
        }
    }
}


// Function to check if the string is empty or contains only whitespace
bool IsStringEmptyOrWhitespace(const TCHAR* str)
{
    while (*str)
    {
        if (!_istspace(*str))
        {
            return false; // If a non-space character is found, it's not empty
        }
        str++; // Move to the next character
    }
    return true; // If only spaces or an empty string is found, return true
}

// Handle Start button click
LRESULT HandleStartButtonClick(HWND hwnd)
{
    // Get the player names from the textboxes
    GetWindowText(hP1TextBox, player1Name, 100);
    GetWindowText(hP2TextBox, player2Name, 100);

    // Check if the names are empty or contain only whitespace
    if (IsStringEmptyOrWhitespace(player1Name) || IsStringEmptyOrWhitespace(player2Name))
    {
        MessageBox(hwnd, _T("Player names cannot be empty or only whitespace."), _T("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Check if the names are the same
    if (_tcscmp(player1Name, player2Name) == 0)
    {
        MessageBox(hwnd, _T("Players must have different names."), _T("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Start the game and show a message
    MessageBox(hwnd, _T("Game is starting..."), _T("Game Mode"), MB_OK);
    isGameStarted = true;

    // Update the labels with player names
    SetWindowText(hP1Label, player1Name);
    SetWindowText(hP2Label, player2Name);

    // Destroy the textboxes and start button
    ShowWindow(hP1TextBox, SW_HIDE);
    ShowWindow(hP2TextBox, SW_HIDE);
    ShowWindow(hStartButton, SW_HIDE);

    // Create the game grid (assumed to be defined elsewhere)
    CreateGameGrid(hwnd);

    // Create labels for player scores and turn indicator
    hTurnLabel = CreateWindow(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 250, 475, 300, 40, hwnd, NULL, NULL, NULL);
    SendMessage(hTurnLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    hP1ScoreLabel = CreateWindow(_T("STATIC"), _T("0"), WS_CHILD | WS_VISIBLE | SS_CENTER, 75, 250, 100, 50, hwnd, NULL, NULL, NULL);
    SendMessage(hP1ScoreLabel, WM_SETFONT, (WPARAM)hScoreFont, TRUE);

    hP2ScoreLabel = CreateWindow(_T("STATIC"), _T("0"), WS_CHILD | WS_VISIBLE | SS_CENTER, 600, 250, 100, 50, hwnd, NULL, NULL, NULL);
    SendMessage(hP2ScoreLabel, WM_SETFONT, (WPARAM)hScoreFont, TRUE);

    // Set the turn label text (assumed to be defined elsewhere)
    SetTurnLabelText();

    return 0;
}


// Function to destroy the tic tac toe game grid 3x3
void DestroyGameGrid()
{
    DestroyGameGrid(hGridButtons);
}


// Function to save the game result to the file
void SaveGameResult(HWND hwnd)
{
    SaveGameResult(hwnd, filePath, player1Name, player1Score, player2Name, player2Score,isGameStarted);
}

// Function for starting new game
void StartNewGame(HWND hwnd)
{
    SaveGameResult(hwnd);
    MessageBox(hwnd,_T("New Game Starting..."),_T("TicTacToe"),MB_OK);
    player1Score = 0;
    player2Score = 0;

    SetWindowText(hP1Label, "(X) PLAYER 1");
    SetWindowText(hP2Label, "(O) PLAYER 2");
    SetWindowText(hP1TextBox, "P1 Name");
    SetWindowText(hP2TextBox, "P2 Name");

    ShowWindow(hP1TextBox, SW_SHOW);
    ShowWindow(hP2TextBox, SW_SHOW);
    ShowWindow(hStartButton, SW_SHOW);
    ShowWindow(hP1ScoreLabel, SW_HIDE);
    ShowWindow(hP2ScoreLabel, SW_HIDE);
    ShowWindow(hTurnLabel, SW_HIDE);

    // Destroy Game Grid
    DestroyGameGrid();
}

// WM_DESTROY message
LRESULT HandleDestroyMessage(HWND hwnd)
{
    SaveGameResult(hwnd);
    DeleteObject(hBrush);
    DeleteObject(hFont);
    PostQuitMessage(0);
    return 0;
}

// WM_COMMAND message
LRESULT HandleCommandMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case ID_BUTTON_START:
        return HandleStartButtonClick(hwnd);
    case ID_MENU_NEW_GAME:
        StartNewGame(hwnd);
        break;
    case ID_MENU_SETTINGS:
        MessageBox(hwnd, _T("Settings option clicked."), _T("Options"), MB_OK);
        break;
    case ID_MENU_ABOUT:
        MessageBox(hwnd, _T("TicTacToe Game v1.0\nMade by Alper"), _T("About"), MB_OK);
        break;
    case ID_MENU_EXIT:
        HandleDestroyMessage(hwnd);
        break;
    default:
        if (LOWORD(wParam) >= ID_BUTTON_START + 1 && LOWORD(wParam) <= ID_BUTTON_START + 9)
        {
            HandleGridButtonClick((HWND)lParam);
        }
        break;
    }
    return 0;
}

// WM_CTLCOLORSTATIC message
LRESULT HandleCtlColorStatic(WPARAM wParam)
{
    HDC hdcStatic = (HDC)wParam;
    SetBkColor(hdcStatic, RGB(173, 216, 230));
    return (INT_PTR)hBrush;
}


// WM_CREATE message
LRESULT HandleCreateMessage(HWND hwnd)
{
    CreateStartScreen(hwnd);
    return 0;
}

// Processes Windows messages and routes them accordingly
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        return HandleCreateMessage(hwnd);

    case WM_CTLCOLORSTATIC:
        return HandleCtlColorStatic(wParam);

    case WM_COMMAND:
        return HandleCommandMessage(hwnd, wParam, lParam);

    case WM_DESTROY:
        return HandleDestroyMessage(hwnd);

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
