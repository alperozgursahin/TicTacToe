#include <windows.h> // for HWND and WinAPI functions
#include "tictactoedllheader.h"
#include <tchar.h>
#include <stdio.h>

namespace dllspec {
    void dllclass::DestroyGameGrid(HWND hGridButtons[3][3]) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (hGridButtons[i][j] != NULL) {
                    DestroyWindow(hGridButtons[i][j]); // Destroy button
                    hGridButtons[i][j] = NULL;
                }
            }
        }
    }
    void dllclass::SaveGameResult(HWND hwnd,
                              const TCHAR* filePath,
                              const TCHAR* player1Name,
                              int player1Score,
                              const TCHAR* player2Name,
                              int player2Score,
                              bool& isGameStarted) {
    if (!isGameStarted) return;

    MessageBox(hwnd, _T("Results are saving.."), _T("Game Mode"), MB_OK);
    isGameStarted = false;

    // Format the game result as a string
    TCHAR gameResult[300];
    _stprintf_s(gameResult, _countof(gameResult),
                _T("%s's Score => %d , %s's Score => %d\n"),
                player1Name, player1Score, player2Name, player2Score);

    // Open or create the file
    HANDLE hFile = CreateFile(
        filePath,
        FILE_APPEND_DATA,
        FILE_SHARE_READ,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(nullptr, _T("Failed to open the file!"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Write the result to the file
    DWORD bytesWritten = 0;
    BOOL success = WriteFile(
        hFile,
        gameResult,
        static_cast<DWORD>(_tcslen(gameResult) * sizeof(TCHAR)),
        &bytesWritten,
        nullptr);

    if (!success) {
        MessageBox(nullptr, _T("Failed to write to the file!"), _T("Error"), MB_OK | MB_ICONERROR);
    }

    // Close the file handle
    CloseHandle(hFile);
}

void UpdateScore(HWND hwnd, bool isPlayerX, int player1Score, int player2Score, HWND hP1ScoreLabel, HWND hP2ScoreLabel) {
    char buffer[32];
    // Player 1 Score Update
    if (isPlayerX) {
        player1Score++;
        sprintf(buffer, "%d", player1Score);
        SetWindowText(hP1ScoreLabel, buffer);
    }
    // Player 2 Score Update
    else {
        player2Score++;
        sprintf(buffer, "%d", player2Score);
        SetWindowText(hP2ScoreLabel, buffer);
    }

}

void CreateGameGrid(HWND hwnd, HWND hGridButtons[3][3], HFONT hFont, int ButtonStartId) {
    int xOffset = 250, yOffset = 150, buttonSize = 100;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            hGridButtons[i][j] = CreateWindow(_T("BUTTON"), _T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                              xOffset + j * buttonSize, yOffset + i * buttonSize, buttonSize, buttonSize,
                                              hwnd, (HMENU)(ButtonStartId + 1 + i * 3 + j), NULL, NULL);
            SendMessage(hGridButtons[i][j], WM_SETFONT, (WPARAM)hFont, TRUE);
        }
    }
}

}
