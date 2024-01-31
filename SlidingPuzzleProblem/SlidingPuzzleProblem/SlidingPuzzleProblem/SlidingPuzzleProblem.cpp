// SlidingPuzzleProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <queue>
#include <iomanip>
#include <ctime>
#include <chrono>

using namespace std;

int dimensionSizeOfMatrix = 0;
vector<vector<int>> goalStateOfPuzzle;

int manhattanDistance(vector<vector<int>> currentState, vector<vector<int>> goalState) {

    int manhattanDistance = 0;
    for (int row = 0; row < dimensionSizeOfMatrix; row++) {
        for (int column = 0; column < dimensionSizeOfMatrix; column++) {
            if (currentState[row][column] != goalState[row][column]) {
                if (currentState[row][column] != 0) {
                    int goalRow = currentState[row][column] / dimensionSizeOfMatrix; // 2/3 = 0 
                    int goalColumn = currentState[row][column] % dimensionSizeOfMatrix; //2/3 = 2
                    if (currentState[row][column] == goalState[goalRow][goalColumn]) {
                        manhattanDistance += abs(row - goalRow) + abs(column - goalColumn);
                    }
                    else {
                        if (goalColumn == 0) {
                            goalRow -= 1;
                            goalColumn = dimensionSizeOfMatrix - 1;
                        }
                        else {
                            goalColumn -= 1;
                        }
                        manhattanDistance += abs(row - goalRow) + abs(column - goalColumn);
                    }
                }
            }
        }
    }
    return manhattanDistance;
}

vector<vector<int>> moveBlankPart(vector<vector<int>> currentState, int currentRow, int currentColumn, int newRow, int newColumn) {
    int otherPart = currentState[newRow][newColumn];
    currentState[currentRow][currentColumn] = otherPart;
    currentState[newRow][newColumn] = 0;
    return currentState;
}

map<string,vector<vector<int>>> possibleMoves(vector<vector<int>> currentState) {
    map<string, vector<vector<int>>> possibleMoves;
    int rowOfBlankPart = 0;
    int columnOfBlankPart = 0;
    for (int row = 0; row < dimensionSizeOfMatrix; row++) {
        for (int column = 0; column < dimensionSizeOfMatrix; column++) {
            if (currentState[row][column] == 0) {
                rowOfBlankPart = row;
                columnOfBlankPart = column;
            }
        }
    }
    if (rowOfBlankPart != 0) {
        possibleMoves.insert({ "down",moveBlankPart(currentState, rowOfBlankPart, columnOfBlankPart, rowOfBlankPart - 1, columnOfBlankPart) });
    }
    if (rowOfBlankPart != dimensionSizeOfMatrix - 1) {
        possibleMoves.insert({ "up",moveBlankPart(currentState, rowOfBlankPart, columnOfBlankPart, rowOfBlankPart + 1, columnOfBlankPart) });
    }
    if (columnOfBlankPart != 0) {
        possibleMoves.insert({ "right",moveBlankPart(currentState, rowOfBlankPart, columnOfBlankPart, rowOfBlankPart, columnOfBlankPart - 1)});
    }
    if (columnOfBlankPart != dimensionSizeOfMatrix - 1) {
        possibleMoves.insert({ "left",moveBlankPart(currentState, rowOfBlankPart, columnOfBlankPart, rowOfBlankPart, columnOfBlankPart + 1)});
    }
    return possibleMoves;
}

// State node of the puzzle that keeps the distance to it, the heuristic and thee previous state, leading to this one
struct Node {
    int distanceToState = INT_MAX;
    int heuristic = 0;
    Node* previousStateOfPuzzle;
    vector<vector<int>> stateOfPuzzle;

    int f() const {
        return distanceToState + heuristic;
    }
    string previousMovement;
};

#define STATE vector<vector<int>>
struct Comparator {
    bool operator()(const Node* first, const Node* second) const {
        return first->f() > second->f();
    }
};

Node* getOrCreateNode(map<STATE, Node*>& collectionOfNodes,const STATE& searchedState)
{
    Node* newNode = nullptr;
	if (collectionOfNodes.find(searchedState) == collectionOfNodes.end()) {
		newNode = new Node();
		newNode->stateOfPuzzle = searchedState;
        collectionOfNodes.insert({ searchedState,newNode });
	}
	else
	{
        newNode = collectionOfNodes[searchedState];
	}
    return newNode;
}

vector<string> reconstructPath(Node* lastNode)
{
    vector<string> commandToReach;
    while (lastNode->previousStateOfPuzzle != nullptr)
    {
        commandToReach.push_back(lastNode->previousMovement);
        lastNode = lastNode->previousStateOfPuzzle;
    }
    return commandToReach;
}

vector<string> aStarAlgorithm(vector<vector<int>> startState, vector<vector<int>> wantedState) {
    //Nodes yet to be explored
    priority_queue<Node*, vector<Node*>, Comparator> unexploredStates;
    // Nodes already explored. Cannot search in a priority queue
    vector<STATE> exploredStates;
    //Reverse mapping from actual board state to the abstract Node if one exists
    map<STATE, Node*> statesToNodes;


    //Entering the start node with initial state
    Node* node= new Node();
    node->distanceToState = 0;
    node->heuristic = manhattanDistance(startState, goalStateOfPuzzle);
    node->stateOfPuzzle = startState;
    statesToNodes.insert({ node->stateOfPuzzle, node });
    unexploredStates.push(node);


    //Explore new node while the collection is not null
    while (!unexploredStates.empty()) {
        Node* currentState = unexploredStates.top();
        unexploredStates.pop();
        exploredStates.push_back(currentState->stateOfPuzzle);
        if (currentState->stateOfPuzzle == goalStateOfPuzzle) {
            return reconstructPath(currentState);
        }

        for (auto& possibleMove : possibleMoves(currentState->stateOfPuzzle)) {
            // check if we have the current state's child in the collection statesToNodes
            bool isExplored = (find(exploredStates.begin(),exploredStates.end(),possibleMove.second)!=exploredStates.end());
            int currentGScore = currentState->distanceToState + 1;

            // if the neighbour is not in the collection, insert it with default INFINITY value (INT_MAX)
            //else just gets the node from the map
            Node* newState = getOrCreateNode(statesToNodes, possibleMove.second);

            // if current distance to state is smaller than distance to kid
            if (currentGScore < statesToNodes[possibleMove.second]->distanceToState) {
                newState->previousStateOfPuzzle = currentState;
                newState->distanceToState = currentGScore;
                newState->heuristic =manhattanDistance(possibleMove.second, goalStateOfPuzzle);
                newState->previousMovement = possibleMove.first;
            }
            if (isExplored == false) {
                unexploredStates.push(newState);
            }
        }
    }
}

int main()
{

    int sizeOfPuzzle = 0;
    //cout << "Set the size of the puzzle: ";
    cin >> sizeOfPuzzle;
    int blocksOfPuzzle = sizeOfPuzzle + 1;
    dimensionSizeOfMatrix = sqrt(blocksOfPuzzle);
    int positionOfBlankPart = 0;
    //cout << "Insert position of blank part: ";
    cin >> positionOfBlankPart;
    int rowOfBlankPart=0;
    int columnOfBlankPart=0;
    if (positionOfBlankPart == -1) {
        rowOfBlankPart = dimensionSizeOfMatrix-1;
        columnOfBlankPart = dimensionSizeOfMatrix - 1;
    }
    else {
        rowOfBlankPart = positionOfBlankPart/dimensionSizeOfMatrix;
        columnOfBlankPart = positionOfBlankPart%dimensionSizeOfMatrix;
    }
    vector<vector<int>> startingStateOfPuzzle;
    vector<int> checkForInversions;
    int puzzleBlock = 1;

    for (int row = 0; row < dimensionSizeOfMatrix; row++) {
        vector<int> currentRow;
        for (int column = 0; column < dimensionSizeOfMatrix; column++) {
            if (row == rowOfBlankPart && column == columnOfBlankPart) {
                currentRow.push_back(0);
            }
            else {
                currentRow.push_back(puzzleBlock);
                puzzleBlock++;
            }
        }
        goalStateOfPuzzle.push_back(currentRow);
    }
    int currentNumber = 0;
    //cout << "Write starting Node of puzzle: ";
    for (int row = 0; row < dimensionSizeOfMatrix; row++) {
        vector<int> currentRow;
        for (int column = 0; column < dimensionSizeOfMatrix; column++) {
            cin >> currentNumber;
            currentRow.push_back(currentNumber);
            checkForInversions.push_back(currentNumber);
        }
        startingStateOfPuzzle.push_back(currentRow);
    }
    
    //Checking if the puzzle is solvable through number of inversions 
    int inversions = 0;
    for (int i = 0; i < blocksOfPuzzle; i++) {
        for (int j = i + 1; j < blocksOfPuzzle; j++) {
            if (checkForInversions[i] > checkForInversions[j] && checkForInversions[j] != 0) {
                inversions++;
            }
        }
    }
    if (inversions % 2 != 0) {
        //cout << "The puzzle is unsolvable!";
        cout << "-1"<<endl;
    }
    else {
        // If we count the inversions checking for finding a solution, we can put the start before the inversions declaration
        auto start = std::chrono::high_resolution_clock::now();

        auto commandsToReach =  aStarAlgorithm(startingStateOfPuzzle, goalStateOfPuzzle);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() / static_cast<double>(1000);
        //std::cout << std::fixed << std::setprecision(2) << duration<<"\n";
        
        cout << commandsToReach.size()<<endl;
        reverse(commandsToReach.begin(),commandsToReach.end());
        for (auto& command : commandsToReach)
        {
            cout << command << endl;
        }
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
