// FrogLeapProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <random>
#include <ctime>

using namespace std;

vector<string> pathToWantedState;

string moveFrog(string currentState, int currentPosition, int wantedPosition) {
    char frog = currentState[currentPosition];
    currentState[currentPosition] = '_';
    currentState[wantedPosition] = frog;
    return currentState;
}
vector<string> possibleFrogMoves(string currentState) {
    vector<string> possibleMoves;
    // 0 1 2 3 4
    // > > _ < <
    // TO DO: create a function for the if checks as they are with similar logic
    for (int i = 0; i < currentState.length(); i++) {
        // left frogs > ... ->
        // left frogs, one jump
        if (i < currentState.length() - 1 && currentState[i] == '>' && currentState[i + 1] == '_') {
            possibleMoves.push_back(moveFrog(currentState, i, i + 1));
        }
        // rigth frogs, one jump
        if (i > 0 && currentState[i] == '<' && currentState[i - 1] == '_') {
            possibleMoves.push_back(moveFrog(currentState, i, i - 1));
        }
        // left frogs, two jumps
        if (i < currentState.length() - 2 && currentState[i] == '>' && currentState[i + 2] == '_') {
            possibleMoves.push_back(moveFrog(currentState, i, i + 2));
        }
        // right frogs, two jumps
        if (i > 1 && currentState[i] == '<' && currentState[i - 2] == '_') {
            possibleMoves.push_back(moveFrog(currentState, i, i - 2));
        }
    }
    return possibleMoves;
}
int DFS(string currentState, string wantedState) {
    pathToWantedState.push_back(currentState);
    if (currentState == wantedState) {
        return 1;
    }
    else {
        //moveFrogs
        vector<string> currentMoves = possibleFrogMoves(currentState);
        int branchesLeadingToSolution = 0;
        for (string move : currentMoves){
            if (branchesLeadingToSolution >= 1) {
                break;
            }
            branchesLeadingToSolution+=DFS(move, wantedState);
        }
        if (branchesLeadingToSolution==0) {
            pathToWantedState.pop_back();
        }
        return branchesLeadingToSolution;
    }
}
int main()
{
    cout << "Number of frogs in total:";
    int numberOfFrogs = 0;
    cin >> numberOfFrogs;
    string initialState = "";
    string wantedState = "";
    for (int i = 0; i < numberOfFrogs + 1; i++) {
        if (i < numberOfFrogs / 2) {
            initialState += ">";
            wantedState += "<";
        }
        else if (i == numberOfFrogs / 2) {
            initialState += "_";
            wantedState += "_";
        }
        else {
            initialState += "<";
            wantedState += ">";
        }
    }
    auto start = std::chrono::high_resolution_clock::now();
    DFS(initialState, wantedState);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / static_cast<double>(1000000);
    std::cout << std::fixed << std::setprecision(2) << duration;
    for (string state : pathToWantedState) {
        cout << state + "\n";
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
