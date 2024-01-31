// TicTacToe.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cctype>
#include <string.h>
#include <algorithm>
#include <vector>

using namespace std;

char ticTacToe[3][3] = {
    {'.', '.', '.'},
    {'.', '.', '.'},
    {'.', '.', '.'}
};

char playerSymbol = ' ';
char machineSymbol = ' ';

void getCurrentStateOfTicTacToe() {
    cout << "\n";
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            cout << ticTacToe[row][column] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool isTicTacToeFull() {
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            if (ticTacToe[row][column] == '.') {
                return false;
            }
        }
    }
    return true;
}

int const COEFFICIENT_FOR_DEPTH_CHECKING = 10;

int judgeCurrentStateBasedOfCoefficients(int currentDepthOfGame) {
    for (int row = 0; row < 3; row++) {
        if (ticTacToe[row][0] == ticTacToe[row][1] && ticTacToe[row][0] == ticTacToe[row][2]) {
            if (ticTacToe[row][0] == playerSymbol) {
                return currentDepthOfGame - COEFFICIENT_FOR_DEPTH_CHECKING;
            }
            else if(ticTacToe[row][0] == machineSymbol) {
                return COEFFICIENT_FOR_DEPTH_CHECKING - currentDepthOfGame;
            }
        }
    }

    for (int column = 0; column < 3; column++) {
        if (ticTacToe[0][column] == ticTacToe[1][column] && ticTacToe[0][column] == ticTacToe[2][column]) {
            if (ticTacToe[0][column] == playerSymbol) {
                return currentDepthOfGame - COEFFICIENT_FOR_DEPTH_CHECKING;
            }
            else if (ticTacToe[0][column] == machineSymbol) {
                return COEFFICIENT_FOR_DEPTH_CHECKING - currentDepthOfGame;
            }
        }
    }

    if (ticTacToe[0][0] == ticTacToe[1][1] && ticTacToe[0][0] == ticTacToe[2][2]) {
        if (ticTacToe[0][0] == playerSymbol) {
            return currentDepthOfGame - COEFFICIENT_FOR_DEPTH_CHECKING;
        }
        else if(ticTacToe[0][0] == machineSymbol) {
            return COEFFICIENT_FOR_DEPTH_CHECKING - currentDepthOfGame;
        }
    }

    if (ticTacToe[0][2] == ticTacToe[1][1] && ticTacToe[0][2] == ticTacToe[2][0]) {
        if (ticTacToe[0][2] == playerSymbol) {
            return currentDepthOfGame - COEFFICIENT_FOR_DEPTH_CHECKING;
        }
        else if(ticTacToe[0][2] == machineSymbol) {
            return COEFFICIENT_FOR_DEPTH_CHECKING - currentDepthOfGame;
        }
    }
    return 0;
}

bool hasNoWinner() {
    if (judgeCurrentStateBasedOfCoefficients(0) == 0) {
        return true;
    }
    return false;
}

void placeSymbol(int row, int column, bool isPlayersTurn) {
    if (isPlayersTurn) {
        ticTacToe[row][column] = playerSymbol;
    }
    else {
        ticTacToe[row][column] = machineSymbol;
    }
    cout << "New state of TicTacToe: \n";
    getCurrentStateOfTicTacToe();

    if (!hasNoWinner()) {
        if (isPlayersTurn) {
            cout << "You win! \n";
        }
        else {
            cout << "You lose! \n";
        }
    }
}
int minValue(int alpha, int beta, int currentDepth);

int maxValue(int alpha, int beta, int currentDepth) {
    if (isTicTacToeFull()) {
        return 0;
    }
    int currentCoefficient = judgeCurrentStateBasedOfCoefficients(currentDepth);
    if (currentCoefficient != 0) {
        return currentCoefficient;
    }
    // v <- -inf
    int bestValue = INT_MIN;
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            if (ticTacToe[row][column] == '.') {
                ticTacToe[row][column] = machineSymbol;
                bestValue = max(bestValue, minValue(alpha, beta, currentDepth + 1));
                ticTacToe[row][column] = '.';

                // prune the branch
                if (bestValue >= beta) {
                    return bestValue;
                }
                alpha = max(alpha, bestValue);
            }
        }
    }
    return bestValue;
}
int minValue(int alpha, int beta, int currentDepth) {
    if (isTicTacToeFull()) {
        return 0;
    }
    int currentCoefficient = judgeCurrentStateBasedOfCoefficients(currentDepth);
    if (currentCoefficient != 0) {
        return currentCoefficient;
    }
    // v <- +inf
    int bestValue = INT_MAX;
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            if (ticTacToe[row][column] == '.') {
                ticTacToe[row][column] = playerSymbol;
                bestValue = min(bestValue, maxValue(alpha, beta, currentDepth + 1));
                ticTacToe[row][column] = '.';

                // prune the branch
                if (bestValue <= alpha) {
                    return bestValue;
                }
                beta = min(beta, bestValue);
            }
        }
    }
    return bestValue;
}

vector<int> minimaxAlphaBetaDecision() {
    int bestValue = INT_MIN;
    vector<int> bestPlace;
    bestPlace.push_back(-1);
    bestPlace.push_back(-1);

    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            if (ticTacToe[row][column] == '.') {
                ticTacToe[row][column] = machineSymbol;
                int currentValue = minValue(INT_MIN, INT_MAX, 0);
                ticTacToe[row][column] = '.';
                // maximising minValue
                if (currentValue > bestValue) {
                    bestValue = currentValue;
                    bestPlace[0] = row;
                    bestPlace[1] = column;
                }
            }
        }
    }
    return bestPlace;
}

bool isPlayersTurn = false;

void treatPlayersAnswer(string answer) {
    transform(answer.begin(), answer.end(), answer.begin(),
        [](unsigned char c) { return std::tolower(c); });
    if (answer=="yes") {
        playerSymbol = 'X';
        machineSymbol = 'O';
        isPlayersTurn = true;
    }
    else {
        playerSymbol = 'O';
        machineSymbol = 'X';
    }
    cout << "You are playing with \"" << playerSymbol << "\". \n";
}

int main()
{
    getCurrentStateOfTicTacToe();
    cout << "Possible places to mark are signed with \".\". \nDo you want to start the game? \n";
    
    string answer = "";
    cin >> answer;
    treatPlayersAnswer(answer);
    
    int row = 0;
    int column = 0;
    while (!isTicTacToeFull() && hasNoWinner()) {
        if (isPlayersTurn) {
            cout << "Insert your wanted row and column: \n";
            cin >> row >> column;
            placeSymbol(row - 1, column - 1, isPlayersTurn);
            isPlayersTurn = false;
        }
        // for when the player makes the last possible move on the ticTacToe or wins
        if (isTicTacToeFull() || !hasNoWinner()) {
            break;
        }
        cout << "M Machine plays.\n";
        vector<int> bestMachineMove = minimaxAlphaBetaDecision();
        placeSymbol(bestMachineMove[0], bestMachineMove[1], isPlayersTurn);
        isPlayersTurn = true;     
    }
    if (isTicTacToeFull() && hasNoWinner()) {
        cout << "It's a draw! \n";
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
