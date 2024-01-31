// NaiveBayesClassifierDemocratsAndRepublicansProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

vector<string> informationGiven;

//yes, no and ?
//doesn't really matter if we include the ? in that concrete dataset, there are small to non-existent changes in the final result
int democrats[3][16];
int republicans[3][16];

double countOfDemocrats = 0;
double countOfRepublicans = 0;
//012345678910
//republican,n, y, n, y, y, y, n, n, n, y, ? , y, y, y, n, y
//republican, n, y, n, y, y, y, n, n, n, n, n, y, y, y, n, ?
//democrat,? , y, y, ? , y, y, n, n, n, n, y, n, y, y, n, n

int sizeOfTestSet;

void changeValueInArray(bool isDemocrat, int rowOfArray, int columnOfArray, int valueToAdd) {
    if (isDemocrat) {
        democrats[rowOfArray][columnOfArray] += valueToAdd;
    }
    else {
        republicans[rowOfArray][columnOfArray] += valueToAdd;
    }
}

void updateData(string currentLine, int valueToAdd) {
    int startingPosition = 0;
    bool isDemocrat = false;
    if (currentLine[0] == 'd') {
        isDemocrat = true;
        countOfDemocrats += valueToAdd;
        startingPosition = 9;
    }
    else {
        countOfRepublicans += valueToAdd;
        startingPosition = 11;
    }
    int indexOfCurrentAttribute = 0;

    for (int positionInLine = startingPosition; positionInLine < currentLine.size(); positionInLine += 2) {
        if (currentLine[positionInLine] == 'y') {
            changeValueInArray(isDemocrat, 0, indexOfCurrentAttribute, valueToAdd);
        }
        else if (currentLine[positionInLine] == 'n') {
            changeValueInArray(isDemocrat, 1, indexOfCurrentAttribute, valueToAdd);
        }
        else if (currentLine[positionInLine] == '?') {
            changeValueInArray(isDemocrat, 2, indexOfCurrentAttribute, valueToAdd);
        }
        indexOfCurrentAttribute++;
    }
}

int classifyInstance(string currentLine) {
    bool isDemocrat = false;
    int startingPosition = 0;

    if (currentLine[0] == 'd') {
        isDemocrat = true;
        startingPosition = 9;
    }
    else {
        startingPosition = 11;
    }
    double democratsLogarithm = 0;
    double republicansLogarithm = 0;

    int indexOfCurrentAttribute = 0;
    //democrats and republicans
    const int NUMBER_OF_POSSIBLE_CLASSES = 2;
    const int LAMBDA = 1;
    for (int positionInLine = startingPosition; positionInLine < currentLine.size(); positionInLine += 2) {
        if (currentLine[positionInLine] == 'y') {
            democratsLogarithm += log((democrats[0][indexOfCurrentAttribute] + LAMBDA) / (countOfDemocrats + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
            republicansLogarithm += log((republicans[0][indexOfCurrentAttribute] + LAMBDA) / (countOfRepublicans + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
        }
        else if (currentLine[positionInLine] == 'n') {
            democratsLogarithm += log((democrats[1][indexOfCurrentAttribute] + LAMBDA) / (countOfDemocrats + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
            republicansLogarithm += log((republicans[1][indexOfCurrentAttribute] + LAMBDA) / (countOfRepublicans + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
        }
        else if (currentLine[positionInLine] == '?') {
            democratsLogarithm += log((democrats[2][indexOfCurrentAttribute] + LAMBDA) / (countOfDemocrats + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
            republicansLogarithm += log((republicans[2][indexOfCurrentAttribute] + LAMBDA) / (countOfRepublicans + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
        }
        indexOfCurrentAttribute++;
    }
    //adding the chance to be from the said class
    democratsLogarithm += log((countOfDemocrats + LAMBDA) / (countOfDemocrats + countOfRepublicans + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));
    republicansLogarithm += log((countOfRepublicans + LAMBDA) / (countOfDemocrats + countOfRepublicans + (NUMBER_OF_POSSIBLE_CLASSES * LAMBDA)));

    if (isDemocrat) {
        return (republicansLogarithm <= democratsLogarithm);
    }
    else {
        return (republicansLogarithm >= democratsLogarithm);
    }
}

double getAccuracy(int startOfTestSet) {
    double accurateClassifications = 0;
    for (int currentNumberOfLine = 0; currentNumberOfLine < sizeOfTestSet; currentNumberOfLine++) {
        accurateClassifications += classifyInstance(informationGiven[startOfTestSet + currentNumberOfLine]);
    }
    return accurateClassifications  / sizeOfTestSet * 100;
}

void getMean(vector<double> elements) {
    double sum = 0.0;
    for (int position = 0; position < elements.size(); position++) {
        sum += elements[position];
    }
    cout<<"The mean is: "<< (sum / elements.size())<<"%\n";
}

void shuffleTheInformationGiven() {
    std::srand(std::time(0));
    random_shuffle(informationGiven.begin(), informationGiven.end());
}

vector<double> allAccuracies;

void crossValidation() {
    const int NUMBER_OF_SETS = 10;
    sizeOfTestSet = informationGiven.size() / NUMBER_OF_SETS;
    double currentAccuracy=0.0;
    shuffleTheInformationGiven();
    for (int numberOfSet = 0; numberOfSet < NUMBER_OF_SETS; numberOfSet++) {
        int startOfTest = sizeOfTestSet * numberOfSet;
        int endOfTest = sizeOfTestSet * (numberOfSet + 1);
        //initialise and update data for training for all without the testSet
        for (int i = 0; i < informationGiven.size(); i++) {
            if (i < startOfTest || i >= endOfTest) {
                updateData(informationGiven[i], 1);
            }
        }
        //testing on the current testSet
        currentAccuracy = getAccuracy(startOfTest);
        cout << "Current accuracy: " << currentAccuracy << "%\n";
        allAccuracies.push_back(currentAccuracy);
        //reset for the next iteration
        for (int i = startOfTest; i < endOfTest; i++) {
            updateData(informationGiven[i], -1);
        }
    }
}

int main()
{
    string fileWithInformation = "house-votes-84_data.txt";

    ifstream file(fileWithInformation);

    string currentLine;

    if (file.is_open()) {
        while (getline(file, currentLine)) {
            informationGiven.push_back(currentLine);
            updateData(currentLine, 1);
        }
        file.close();
    }
    else {
        cout << "Problem with the file!\n";
    }

    crossValidation();
    getMean(allAccuracies);  
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
