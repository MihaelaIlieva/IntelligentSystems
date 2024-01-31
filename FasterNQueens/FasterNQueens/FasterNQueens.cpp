// FasterNQueens.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

bool isStillUnsolved;
int numberOfQueens;
int* queensPositions;
int* rowConflicts;
int* mainDiagonalConflicts;
int* oppositeDiagonalConflicts;

void initialiseBoard()
{
	for (int column = 0; column < numberOfQueens; column++) {
		int row = rand() % numberOfQueens;
		// one queen per column
		queensPositions[column] = row;
		rowConflicts[row]++;
		mainDiagonalConflicts[column - row + numberOfQueens - 1]++;
		oppositeDiagonalConflicts[column + row]++;
	}
}

int getQueenColumnWithMaxConflicts()
{
	int maxConflicts = INT_MIN;
	vector<int> columnsWithMaxConflicts;
	int currentRow;
	int currentConflicts;

	for (int currentColumn = 0; currentColumn < numberOfQueens; currentColumn++) {
		currentRow = queensPositions[currentColumn];
		const int CURRENT_QUEEN_COUNTED = 3;
		currentConflicts = rowConflicts[currentRow] + mainDiagonalConflicts[currentColumn - currentRow + numberOfQueens - 1] + oppositeDiagonalConflicts[currentColumn + currentRow] - CURRENT_QUEEN_COUNTED;
		if (currentConflicts == maxConflicts) {
			columnsWithMaxConflicts.push_back(currentColumn);
		}
		else if (currentConflicts > maxConflicts) {
			maxConflicts = currentConflicts;
			columnsWithMaxConflicts.clear();
			columnsWithMaxConflicts.push_back(currentColumn);
		}
	}
	if (maxConflicts == 0) {
		isStillUnsolved = false;
	}
	int randIndex = rand() % columnsWithMaxConflicts.size();
	return columnsWithMaxConflicts[randIndex];
}

int getRowWithMinConflicts(int columnGiven)
{
	int minConflicts = INT_MAX;
	vector<int> rowsWithMinConflicts;
	int currentConflicts;

	for (int currentRow = 0; currentRow < numberOfQueens; currentRow++) {
		// if there is a queen
		if (queensPositions[columnGiven] == currentRow) {
			currentConflicts = rowConflicts[currentRow] + mainDiagonalConflicts[columnGiven - currentRow + numberOfQueens - 1] + oppositeDiagonalConflicts[columnGiven + currentRow] - 3;
		}
		else {
			currentConflicts = rowConflicts[currentRow] + mainDiagonalConflicts[columnGiven - currentRow + numberOfQueens - 1] + oppositeDiagonalConflicts[columnGiven + currentRow];
		}

		if (currentConflicts == minConflicts) {
			rowsWithMinConflicts.push_back(currentRow);
		}
		else if (currentConflicts < minConflicts) {
			minConflicts = currentConflicts;
			rowsWithMinConflicts.clear();
			rowsWithMinConflicts.push_back(currentRow);
		}
	}
	int randIndex = rand() % rowsWithMinConflicts.size();
	return rowsWithMinConflicts[randIndex];
}

void moveQueen(int row, int column)
{
	//decrease number of queens for previous position
	int previousRow = queensPositions[column];
	rowConflicts[previousRow]--;
	mainDiagonalConflicts[column - previousRow + numberOfQueens - 1]--;
	oppositeDiagonalConflicts[column + previousRow]--;

	//increase number of queens for new position
	queensPositions[column] = row;
	rowConflicts[row]++;
	mainDiagonalConflicts[column - row + numberOfQueens - 1]++;
	oppositeDiagonalConflicts[column + row]++;
}

void printTheWholeBoard()
{
	for (int i = 0; i < numberOfQueens; i++) {
		for (int j = 0; j < numberOfQueens; j++) {
			if (queensPositions[j] == i) {
				cout << " * ";
			}
			else {
				cout << " _ ";
			}
		}
		cout << "\n";
	}
	cout << "\n";
}

void minConflictsAlgorithm()
{
	//initialiseBoard();
	int counterOfMoves = 0;
	int row;
	int column;

	while (counterOfMoves <= 2*numberOfQueens) {
		counterOfMoves++;
		column = getQueenColumnWithMaxConflicts();
		if (!isStillUnsolved) {
			break;
		}
		row = getRowWithMinConflicts(column);
		moveQueen(row, column);
	}

	if (isStillUnsolved) {
		minConflictsAlgorithm();
	}
}

int main()
{
	cin >> numberOfQueens;

	queensPositions = new int[numberOfQueens];
	rowConflicts = new int[numberOfQueens] {0};
	mainDiagonalConflicts = new int[2 * numberOfQueens - 1]{0};
	oppositeDiagonalConflicts = new int[2 * numberOfQueens - 1]{0};
	isStillUnsolved = true;
	
	initialiseBoard();
	if (numberOfQueens <= 100) {
		if (numberOfQueens == 1) {
			cout << "[0]";
		}
		else if (numberOfQueens == 2 || numberOfQueens == 3) {
			cout << "-1";
		}
		else {
			minConflictsAlgorithm();
			//printTheWholeBoard();
			vector<int> columns;
			cout << "[";
			for (int i = 0; i < numberOfQueens; i++) {
				columns.push_back(queensPositions[i]);
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
		minConflictsAlgorithm();
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / static_cast<double>(1000000);
		std::cout << std::fixed << std::setprecision(2) << duration<<"\n";
	}
}

// Run program : Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
