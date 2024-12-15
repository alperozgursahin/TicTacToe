#ifndef TICTACTOEDLLHEADER_H
#define TICTACTOEDLLHEADER_H

#include <windows.h>

extern "C" {
    __declspec(dllexport) void CreateGameGrid(HWND hwnd, HWND hGridButtons[3][3], HFONT hFont, int ButtonStartId);
    __declspec(dllexport) void DestroyGameGrid(HWND hGridButtons[3][3]);
    __declspec(dllexport) void SaveGameResult(HWND hwnd, const TCHAR* filePath, const TCHAR* player1Name, int player1Score, const TCHAR* player2Name, int player2Score, bool& isGameStarted);
    __declspec(dllexport) void UpdateScore(bool isPlayerX, int player1Score, int player2Score, HWND hP1ScoreLabel, HWND hP2ScoreLabel);
    __declspec(dllexport) void ResetGameGrid(HWND hGridButtons[3][3], int& roundsPlayed, bool& isPlayerX);
    __declspec(dllexport) bool CheckWinner(HWND hGridButtons[3][3]);
}

#endif // TICTACTOEDLLHEADER_H
