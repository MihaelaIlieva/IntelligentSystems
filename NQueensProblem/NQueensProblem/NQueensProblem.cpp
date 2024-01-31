// NQueensProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <random>
#include <ctime>

using namespace std;

int countConficts(int rowOfWantedQueen, int columnOfWantedQueen,const vector<vector<string>>& stateOfBoard) {
    int countOfConflicts = 0;
    // there are no vertical based on the initialisation of the board in the beggining
    // horisontal conflicts
    for (int row = 0; row < stateOfBoard.size(); row++) {
        for (int column = 0; column < stateOfBoard.size(); column++) {
            if (row == rowOfWantedQueen && column != columnOfWantedQueen && stateOfBoard[row][column] == "*") {
                countOfConflicts++;
            }
            else if (row != rowOfWantedQueen && column == columnOfWantedQueen && stateOfBoard[row][column] == "*") {
                countOfConflicts++;
            }
        }
    }
    // diagonal conflicts to the left of the queen
    // up left
    int currentRow = rowOfWantedQueen - 1;
    int currentColumn = columnOfWantedQueen - 1;

    while (currentColumn >= 0 && currentRow >= 0) {
        if (stateOfBoard[currentRow][currentColumn] == "*") {
            countOfConflicts++;
        }
        currentRow--;
        currentColumn--;
    }
    // down right
    currentRow = rowOfWantedQueen + 1;
    currentColumn = columnOfWantedQueen + 1;
    while (currentColumn < stateOfBoard.size() && currentRow < stateOfBoard.size()) {
        if (stateOfBoard[currentRow][currentColumn] == "*") {
            countOfConflicts++;
        }
        currentRow++;
        currentColumn++;
    }
    // down left
    currentRow = rowOfWantedQueen + 1;
    currentColumn = columnOfWantedQueen - 1;
    while (currentColumn >= 0 && currentRow < stateOfBoard.size()) {
        if (stateOfBoard[currentRow][currentColumn] == "*") {
            countOfConflicts++;
        }
        currentRow++;
        currentColumn--;
    }
    // up right
    currentRow = rowOfWantedQueen - 1;
    currentColumn = columnOfWantedQueen + 1;
    while (currentColumn < stateOfBoard.size() && currentRow >= 0) {
        if (stateOfBoard[currentRow][currentColumn] == "*") {
            countOfConflicts++;
        }
        currentRow--;
        currentColumn++;
    }
    return countOfConflicts;
}

void printWholeBoard(vector<vector<string>>& stateOfBoard) {
    for (int row = 0; row < stateOfBoard.size(); row++) {
        for (int column = 0; column < stateOfBoard.size(); column++) {
            cout << stateOfBoard[row][column] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

vector<int> getMinConflictedFreePlace(int currentRowOfQueen, int columnToLook, const vector<vector<string>>& stateOfBoard) {
    vector<int> newCoordinates;
    vector<int>rowsOfMinConflictedPlaces;
    int minConflicts = INT_MAX;
    int flagCurrentQueenIn = 0;
    for (int row = 0; row < stateOfBoard.size(); row++) {
        //stateOfBoard = moveQueen(currentRowOfQueen, columnToLook, row, columnToLook, stateOfBoard);
        int currentConflicts = countConficts(row, columnToLook, stateOfBoard);
        if (row != currentRowOfQueen) {
            // because it counts the current queen as a conflict as we haven't moved it
            currentConflicts -= 1;
            if (currentConflicts < minConflicts) {
                minConflicts = currentConflicts;
                rowsOfMinConflictedPlaces.clear();
                rowsOfMinConflictedPlaces.push_back(row);
            }
            else if (currentConflicts == minConflicts && flagCurrentQueenIn == 0) {
                rowsOfMinConflictedPlaces.push_back(row);
            }
            else if (currentConflicts == minConflicts && flagCurrentQueenIn != 0) {
                rowsOfMinConflictedPlaces.clear();
                rowsOfMinConflictedPlaces.push_back(row);
            }
        }
        else {
            if (currentConflicts < minConflicts) {
                minConflicts = currentConflicts;
                rowsOfMinConflictedPlaces.clear();
                rowsOfMinConflictedPlaces.push_back(row);
                flagCurrentQueenIn = 1;
            }
        }

    }
    int indexOfPlaceToMove = rand() % rowsOfMinConflictedPlaces.size();
    int rowOfPlaceToMove = rowsOfMinConflictedPlaces[indexOfPlaceToMove];
    newCoordinates.push_back(rowOfPlaceToMove);
    newCoordinates.push_back(columnToLook);

    return newCoordinates;
}

vector<vector<string>> initialBoardFilling(int numberOfQueens) {

    vector<vector<string>> initialBoardState;
    vector<string> currentRow;
    for (int row = 0; row < numberOfQueens; row++) {
        for (int column = 0; column < numberOfQueens; column++) {
                currentRow.push_back("_");
        }
        initialBoardState.push_back(currentRow);
        currentRow.clear();
    }
    
    auto logindex = log2(numberOfQueens);
    int wholepart = floor(logindex) - 1;
    int difference = numberOfQueens / logindex + 1;
    int columnIndex = rand() % difference + 0;
    int rowIndex = rand() % difference + 0;
    initialBoardState[rowIndex][columnIndex] = "*";
    vector<int> takenColumns;
    //initialBoardState[0][1] = "*";
    takenColumns.push_back(columnIndex);
   
    for (int row = 1; row < numberOfQueens; row++) {
        int minConflicts = INT_MAX;
        int columnWithMinConflicts = 0;
        for (int column = 0; column < numberOfQueens; column++) {
            auto findingColumnAvailability = find(takenColumns.begin(), takenColumns.end(), column);
            // if this column is not occupied
            if (findingColumnAvailability == takenColumns.end()) {
                int currentCountOfConflicts = countConficts(row, column, initialBoardState);
                if (currentCountOfConflicts < minConflicts) {
                    minConflicts = currentCountOfConflicts;
                    columnWithMinConflicts = column;
                }
            }
        }
        initialBoardState[row][columnWithMinConflicts] = "*";
        takenColumns.push_back(columnWithMinConflicts);
    }
    return initialBoardState;
}

vector<vector<string>> moveQueen(int rowOfQueen, int columnOfQueen, int newRow, int newColumn, vector<vector<string>>& currentStateOfBoard) {
    
    currentStateOfBoard[rowOfQueen][columnOfQueen] = "_";
    currentStateOfBoard[newRow][newColumn] = "*";
    return currentStateOfBoard;
}
vector<int> getMaxConflictedQueen(const vector<vector<string>>& stateOfBoard) {
    vector<int> coordinatesOfQueenToMove;
    vector<int>columnsOfMaxConflictedQueens;
    vector<int>rowsOfMaxConflictedQueens;
    int maxConflicts = INT_MIN;
    for (int row = 0; row < stateOfBoard.size(); row++) {
        for (int column = 0; column < stateOfBoard.size(); column++) {
            if (stateOfBoard[row][column] == "*") {
                int currentConflicts = countConficts(row, column,stateOfBoard);
                if (currentConflicts > maxConflicts) {
                    maxConflicts = currentConflicts;
                    columnsOfMaxConflictedQueens.clear();
                    columnsOfMaxConflictedQueens.push_back(column);
                    rowsOfMaxConflictedQueens.clear();
                    rowsOfMaxConflictedQueens.push_back(row);
                }
                else if (currentConflicts == maxConflicts) {
                    columnsOfMaxConflictedQueens.push_back(column);
                    rowsOfMaxConflictedQueens.push_back(row);
                }
            }
        }
    }
    int indexOfQueenToMove = rand() % columnsOfMaxConflictedQueens.size();
    int columnOfQueenToMove = columnsOfMaxConflictedQueens[indexOfQueenToMove];
    int rowOfQueenToMove = rowsOfMaxConflictedQueens[indexOfQueenToMove];
    coordinatesOfQueenToMove.push_back(rowOfQueenToMove);
    coordinatesOfQueenToMove.push_back(columnOfQueenToMove);

    return coordinatesOfQueenToMove;
}


int getConflictsOfWholeBoard(const vector<vector<string>>& stateOfBoard) {
    int countOfBoardConflicts = 0;
    for (int row = 0; row < stateOfBoard.size(); row++) {
        for (int column = 0; column < stateOfBoard.size(); column++) {
            if (stateOfBoard[row][column] == "*") {
                countOfBoardConflicts += countConficts(row, column, stateOfBoard);
            }
        }
    }
    return countOfBoardConflicts;
}

int counterOfMoves = 0;

vector<vector<string>> minConflictsAlgorithm(int numberOfQueens) {
    
    vector<vector<string>> stateOfBoard = initialBoardFilling(numberOfQueens);
    while (counterOfMoves <= pow(numberOfQueens, numberOfQueens / log2(numberOfQueens / 2))) {
        counterOfMoves++;

        vector<int> maxConflictedQueen = getMaxConflictedQueen(stateOfBoard);
        // the board is solved6
        if (countConficts(maxConflictedQueen[0],maxConflictedQueen[1],stateOfBoard) == 0) {
            return stateOfBoard;
        }
        vector<int> minConflictedPlace = getMinConflictedFreePlace(maxConflictedQueen[0], maxConflictedQueen[1], stateOfBoard);
        
        stateOfBoard = moveQueen(maxConflictedQueen[0], maxConflictedQueen[1], minConflictedPlace[0], minConflictedPlace[1], stateOfBoard);

        if (getConflictsOfWholeBoard(stateOfBoard) == 0) {
            return stateOfBoard;
        }
    }
    if (getConflictsOfWholeBoard(stateOfBoard) > 0) {
        counterOfMoves = 0;
        minConflictsAlgorithm(numberOfQueens);
    }
}

int main()
{
    int numberOfQueens = 0;
    cin >> numberOfQueens;

    if (numberOfQueens <= 100) {
        if (numberOfQueens == 1) {
            cout << "[0]";
            //cout << "*";
        }
        else if (numberOfQueens == 2 || numberOfQueens == 3) {
            cout << "-1";
        }
        else {
            //auto start = std::chrono::high_resolution_clock::now();
            vector<vector<string>> finalBoard = minConflictsAlgorithm(numberOfQueens);
           // auto stop = std::chrono::high_resolution_clock::now();
            //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / static_cast<double>(1000000);
            //std::cout << std::fixed << std::setprecision(2) << duration;
            for (int row = 0; row < numberOfQueens; row++) {
                for (int column = 0; column < numberOfQueens; column++) {
                    cout << finalBoard[row][column] << " ";
                }
                cout << endl;
            }
            cout << "[";
            vector<int>columns;
            for (int column = 0; column < numberOfQueens; column++) {
                for (int row = 0; row < numberOfQueens; row++) {
                    if (finalBoard[row][column] == "*") {
                        columns.push_back(row);
                    }
                }   
            }
            std::ostringstream oss;
            std::copy(columns.begin(), columns.end(), std::ostream_iterator<int>(oss, ", "));

            // Get the result as a string
            std::string result = oss.str();

            // Remove the trailing separator
            if (!result.empty()) {
                result.pop_back();
                result.pop_back();
            }
            cout << result << "]";
        }
    }
    else {
        auto start = std::chrono::high_resolution_clock::now();

        vector<vector<string>> finalBoard = minConflictsAlgorithm(numberOfQueens);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / static_cast<double>(1000000);
        std::cout << std::fixed << std::setprecision(2) << duration;
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
