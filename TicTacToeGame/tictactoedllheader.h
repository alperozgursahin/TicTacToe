#ifndef tictactoedllheader_H
#define tictactoedllheader_H

#include <windows.h>

namespace dllspec {
class dllclass {
public:
    static __declspec(dllexport) void DestroyGameGrid(HWND hGridButtons[3][3]);
    static __declspec(dllexport) void SaveGameResult(HWND hwnd,
                                                     const TCHAR* filePath,
                                                     const TCHAR* player1Name,
                                                     int player1Score,
                                                     const TCHAR* player2Name,
                                                     int player2Score,
                                                     bool& isGameStarted);
    static __declspec(dllexport) void UpdateScore(bool isPlayerX,
                                                  int player1Score,
                                                  int player2Score,
                                                  HWND hP1ScoreLabel,
                                                  HWND hP2ScoreLabel);
    static __declspec(dllexport) void CreateGameGrid(HWND hwnd,
                                                     HWND hGridButtons[3][3],
                                                     HFONT hFont,
                                                     int ButtonStartId);
};
}

#endif // tictactoedllheader_H
