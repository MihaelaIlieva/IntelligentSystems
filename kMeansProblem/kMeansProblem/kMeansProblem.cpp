// kMeansProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <numeric>

using namespace std;

struct SampleData {
    double xCoord;
    double yCoord;
    int centroidIndex;

    SampleData(double xCoord, double yCoord) {
        this->xCoord = xCoord;
        this->yCoord = yCoord;
        this->centroidIndex = -1;
    }
};

vector<SampleData*> allData;
vector<SampleData*> centroids;
set<int> indexesOfCentroids;
vector<double> distances;

const int RESIZE_SCALE_COEFFICIENT = 1000;
double maxXCoord = 0;
double minXCoord = 0;
double maxYCoord = 0;
double minYCoord = 0;
double scaledMaxXCoord = 0;
double scaledMinXCoord = 0;
double scaledMaxYCoord = 0;
double scaledMinYCoord = 0;

int width = 1920;
int height = 1080;

void initialiseXCoord() {
    auto comparison = [](const SampleData* first, const SampleData* second) {
        return first->xCoord < second->xCoord;
    };

    auto max = max_element(allData.begin(), allData.end(), comparison);
    auto min = min_element(allData.begin(), allData.end(), comparison);
    SampleData* maxPoint = allData[distance(allData.begin(), max)];
    SampleData* minPoint = allData[distance(allData.begin(), min)];

    maxXCoord = maxPoint->xCoord;
    scaledMaxXCoord = maxPoint->xCoord * RESIZE_SCALE_COEFFICIENT;
    minXCoord = minPoint->xCoord;
    scaledMinXCoord = minPoint->xCoord * RESIZE_SCALE_COEFFICIENT;
}
void initialiseYCoord() {
    auto comparison = [](const SampleData* first, const SampleData* second) {
        return first->yCoord < second->yCoord;
    };
    auto max = max_element(allData.begin(), allData.end(), comparison);
    auto min = min_element(allData.begin(), allData.end(), comparison);
    SampleData* maxPoint = allData[distance(allData.begin(), max)];
    SampleData* minPoint = allData[distance(allData.begin(), min)];

    maxYCoord = maxPoint->yCoord;
    scaledMaxYCoord = maxPoint->yCoord * RESIZE_SCALE_COEFFICIENT;
    minYCoord = minPoint->yCoord;
    scaledMinYCoord = minPoint->yCoord * RESIZE_SCALE_COEFFICIENT;
}

double getEuclideanDistance(SampleData* first, SampleData* second) {
    double distance = pow(first->xCoord - second->xCoord, 2) + pow(first->yCoord - second->yCoord, 2);
    return distance;
}

// for k-Means
void initialiseRandomCentroids(int numberOfClusters) {
    double currentXCoord;
    double currentYCoord;
    srand(time(0));
    for (int currentCluster = 0; currentCluster < numberOfClusters; currentCluster++) {
        currentXCoord = ((double)(rand() % RESIZE_SCALE_COEFFICIENT) / RESIZE_SCALE_COEFFICIENT) * (scaledMaxXCoord - scaledMinXCoord) + scaledMinXCoord;
        currentYCoord = ((double)(rand() % RESIZE_SCALE_COEFFICIENT) / RESIZE_SCALE_COEFFICIENT) * (scaledMaxYCoord - scaledMinYCoord) + scaledMinYCoord;
        SampleData* currentCentroid = new SampleData(currentXCoord / RESIZE_SCALE_COEFFICIENT, currentYCoord / RESIZE_SCALE_COEFFICIENT);
        centroids.push_back(currentCentroid);
    }
}

double getMinDistanceToCentroid(int indexOfPoint) {
    double minDistance = getEuclideanDistance(allData[indexOfPoint], allData[0]);
    double currentDistance = 0;

    for (int indexOfCentroid : indexesOfCentroids) {
        currentDistance = getEuclideanDistance(allData[indexOfPoint], allData[indexOfCentroid]);
        if (currentDistance < minDistance) {
            minDistance = currentDistance;
        }
    }
    return minDistance;
}

int chooseNewCentroid() {
    double sumOfAllDistances = accumulate(distances.begin(), distances.end(), 0.0);
    double randomValue = (double)rand() / RAND_MAX;
    double currentSum = 0;
    for (int i = 0; i < distances.size(); i++) {
        currentSum += distances[i];
        if (randomValue < currentSum / sumOfAllDistances) {
            return i;
        }
    }
}

int getNearestCentroidIndex(SampleData* dataGiven) {
    double minDistance = getEuclideanDistance(dataGiven, centroids[0]);
    double currentDistance = 0;
    int nearestCentroidIndex = 0;

    for (int i = 0; i < centroids.size(); i++) {
        currentDistance = getEuclideanDistance(dataGiven, centroids[i]);
        if (currentDistance < minDistance) {
            minDistance = currentDistance;
            nearestCentroidIndex = i;
        }
    }
    return nearestCentroidIndex;
}

bool changeCentroid(int indexOfCentroid) {
    double sumX = 0;
    double sumY = 0;
    int counterOfDataAssignedToCentroid = 0;

    for (int i = 0; i < allData.size(); i++) {
        if (allData[i]->centroidIndex == indexOfCentroid) {
            sumX += allData[i]->xCoord;
            sumY += allData[i]->yCoord;
            counterOfDataAssignedToCentroid++;
        }
    }
    if (counterOfDataAssignedToCentroid == 0) {
        return false;
    }
    SampleData* newCentroid = new SampleData(sumX / counterOfDataAssignedToCentroid, sumY / counterOfDataAssignedToCentroid);
    SampleData* oldCentroid = centroids[indexOfCentroid];
    if (oldCentroid->xCoord == newCentroid->xCoord && oldCentroid->yCoord == newCentroid->yCoord) {
        return false;
    }
    centroids[indexOfCentroid] = newCentroid;
    delete oldCentroid;
    return true;
}

void clusterise() {
    bool isStillNotDone = true;
    bool hasChangedCentroids;
    while (isStillNotDone) {
        isStillNotDone = false;
        for (int i = 0; i < allData.size(); i++) {
            allData[i]->centroidIndex = getNearestCentroidIndex(allData[i]);
        }
        for (int i = 0; i < centroids.size(); i++) {
            hasChangedCentroids = changeCentroid(i);
            isStillNotDone = hasChangedCentroids || isStillNotDone;
        }
    }
}

double getComputerScreenCoordinates(double coord, string typeOfCoord) {
    if (typeOfCoord.compare("x") == 0) {
        return (coord - minXCoord) * width / (maxXCoord - minXCoord);
    }
    else {
        return (maxYCoord - coord) * height / (maxYCoord - minYCoord);
    }
}

sf::Color getColor(int index) {
    std::vector<sf::Color> colors = { sf::Color::Magenta,sf::Color::Blue,sf::Color::Cyan,sf::Color::Green,sf::Color::Red,sf::Color::Yellow,sf::Color(255, 135, 196) };
    if (index < colors.size()) {
        return colors[index];
    }
    else {
        return sf::Color(236,151,49);
    }
}

void drawKMeans(string typeOfKMeans) {
    sf::RenderWindow window(sf::VideoMode(width, height), typeOfKMeans);
    vector<sf::CircleShape> circles;
    double x;
    double y;
    int index;
    for (int i = 0; i < allData.size(); i++) {
        x = getComputerScreenCoordinates(allData[i]->xCoord, "x");
        y = getComputerScreenCoordinates(allData[i]->yCoord, "y");
        index = allData[i]->centroidIndex;
        sf::CircleShape circle;
        circle.setRadius(3);
        circle.setFillColor(getColor(index));
        circle.setPosition(x, y);
        circles.push_back(circle);
    }
    vector<sf::CircleShape> triangles;
    for (int i = 0; i < centroids.size(); i++) {
        x = getComputerScreenCoordinates(centroids[i]->xCoord, "x");
        y = getComputerScreenCoordinates(centroids[i]->yCoord, "y");
        sf::CircleShape triangle(10, 3);
        // getting the opposite color of the cluster so the centroid can be visible
        sf::Color current = getColor(i);
        sf::Color opposite = sf::Color(255 - current.r, 255 - current.g, 255 - current.b);
        triangle.setFillColor(opposite);
        triangle.setPosition(x, y);
        triangles.push_back(triangle);
    }

    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color(255,250,243));
        for (int i = 0; i < circles.size(); i++) {
            window.draw(circles[i]);
        }
        for (int i = 0; i < triangles.size(); i++) {
            window.draw(triangles[i]);
        }
        window.display();
    }
}

void initialiseCentroidsForKMeansPlusPlus(int numberOfClusters) {
    // choosing one center uniformly at random among the data points
    int indexOfFirstPoint = rand() % allData.size();
    indexesOfCentroids.insert(indexOfFirstPoint);
    distances.resize(allData.size(), 0.0);

    int counterOfClusters = 0;
    int indexOfNewCentroid;
    // repeating until k(numberOfCusters) centers have been chosen
    while (counterOfClusters < numberOfClusters - 1) {
        // for each data point not chosen yet
        // compute the distance between it and the nearest center that has already been chosen
        for (int i = 0; i < allData.size(); i++) {
            if (indexesOfCentroids.find(i) == indexesOfCentroids.end()) {
                distances[i] = getMinDistanceToCentroid(i);
            }
        }
        // choosing one new data point at random as a new center
        // using a weighted probability distribution
        indexOfNewCentroid = chooseNewCentroid();
        // ensuring the chosen centroid is unique
        while (indexesOfCentroids.find(indexOfNewCentroid) != indexesOfCentroids.end()) {
            indexOfNewCentroid = chooseNewCentroid();
        }
        indexesOfCentroids.insert(indexOfNewCentroid);
        counterOfClusters++;
    }
    for (int index : indexesOfCentroids) {
        SampleData* centroid = new SampleData(allData[index]->xCoord, allData[index]->yCoord);
        centroids.push_back(centroid);
    }
}

void processKMeans(int numberOfClusters) {
    initialiseRandomCentroids(numberOfClusters);
    clusterise();
    drawKMeans("k-Means");
}

void processKMeansPlusPlus(int numberOfClusters) {
    initialiseCentroidsForKMeansPlusPlus(numberOfClusters);
    clusterise();
    drawKMeans("k-Means++");
}

double calculateDistance(const SampleData* first, const SampleData* second) {
    return sqrt(pow(first->xCoord - second->xCoord, 2) + pow(first->yCoord - second->yCoord, 2));
}

// Function to calculate the intra-cluster distance (sum of distances within each cluster)
double calculateIntraClusterDistance(const vector<SampleData*>& allData, const vector<SampleData*>& centroids) {
    double intraClusterDistance = 0.0;

    for (const auto& dataPoint : allData) {
        int centroidIndex = dataPoint->centroidIndex;
        double distance = calculateDistance(dataPoint, centroids[centroidIndex]);
        intraClusterDistance += distance;
    }

    return intraClusterDistance;
}

// Function to calculate the inter-cluster distance (sum of distances between clusters)
double calculateInterClusterDistance(const vector<SampleData*>& centroids) {
    double interClusterDistance = 0.0;
    int numCentroids = centroids.size();

    for (int i = 0; i < numCentroids - 1; ++i) {
        for (int j = i + 1; j < numCentroids; ++j) {
            double distance = calculateDistance(centroids[i], centroids[j]);
            interClusterDistance += distance;
        }
    }

    return interClusterDistance;
}

// Function to evaluate the clusters based on a combination of intra-cluster and inter-cluster distances
double evaluateClusters(const vector<SampleData*>& allData, const vector<SampleData*>& centroids) {
    double intraClusterDistance = calculateIntraClusterDistance(allData, centroids);
    double interClusterDistance = calculateInterClusterDistance(centroids);

    // You can choose different methods for combining intra-cluster and inter-cluster distances.
    // Here is a simple one, but you can use other formulas, e.g., inverse of intra-cluster distance divided by inter-cluster distance.

    // Combining intra-cluster and inter-cluster distances
    // best out of the three
    double combinedDistance = intraClusterDistance / interClusterDistance;
    // medium out of the three
    //double combinedDistance = intraClusterDistance;
    // worst out of the three
    //double combinedDistance = interClusterDistance;

    return combinedDistance;
}

int main()
{
    string nameOfFile;
    int numberOfClusters;
    cout << "Enter name of file to read: ";
    cin >> nameOfFile;
    cout << "\n";
    cout << "Enter number of clusters: ";
    cin >> numberOfClusters;

    ifstream file(nameOfFile);

    string currentLine;
    double currentXCoord;
    double currentYCoord;

    if (file.is_open()) {
        while (getline(file, currentLine)) {
            stringstream strstream(currentLine);
            strstream << currentLine;
            strstream >> currentXCoord >> currentYCoord;

            SampleData* sample = new SampleData(currentXCoord, currentYCoord);
            allData.push_back(sample);
        }
        file.close();
    }
    else {
        cout << "Problem with the file!\n";
        return 0;
    }
    
    //int numberOfRestarts = 1000; // Пример: Изпълнявай Random Restart 5 пъти
    //double bestEvaluation = numeric_limits<double>::max();
    //vector<SampleData*> bestCentroids;
    //for (int restart = 0; restart < numberOfRestarts; ++restart) {
    //    // Изчистване на текущите центроиди за новия рестарт
    //    initialiseXCoord();
    //    initialiseYCoord();
    //    centroids.clear();
    //    indexesOfCentroids.clear();

    //    // Изпълнение на k-Means++ за текущия рестарт
    //    initialiseRandomCentroids(numberOfClusters);
    //    clusterise();

    //    // Оценка на качеството на кластерите за текущия рестарт
    //    double currentEvaluation = evaluateClusters(allData, centroids);

    //    // Проверка за по-добро решение
    //    if (currentEvaluation < bestEvaluation) {
    //        
    //        // Запазване на текущите центроиди като най-доброто решение
    //        bestCentroids = centroids;
    //        bestEvaluation = currentEvaluation;
    //        drawKMeans("k-Means with random restart");
    //    }
    //}
    //cout << bestEvaluation << "\n";
    
    
    initialiseXCoord();
    initialiseYCoord();
    processKMeans(numberOfClusters); 
    //processKMeansPlusPlus(numberOfClusters);
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
