// KnapsackProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <random>
#include <ctime>

using namespace std;

struct Item {
    int kilograms;
    int value;
};

int numberOfObjects;
vector<Item> itemsToChooseFrom;
int numberOfPopulations = 750;
int populationCount = 100;
int maximumBackpackWeight;
int maxFoundFitness = INT_MIN;
double crossoverRate = 0.6;
double mutationRate = 0.0025;
int tournamentSize = 12;

class Backpack {
private:
    int totalValue = 0;
    int totalWeight = 0;
    vector<bool> itemsBitArray;
public:
    Backpack() {
        itemsBitArray = vector<bool>();
        calculateValueAndWeight();
    }
    Backpack(vector<bool> genotype) {
        itemsBitArray = move(genotype);
        calculateValueAndWeight();
    }
    void calculateValueAndWeight() {
        totalValue = 0;
        totalWeight = 0;
        for (int i = 0; i < itemsBitArray.size(); i++) {
            if (itemsBitArray[i]) {
                totalValue += itemsToChooseFrom[i].value;
                totalWeight += itemsToChooseFrom[i].kilograms;
            }
        }
    }
    /*int getValue() const {
        return totalValue;
    }
    int getWeight() const {
        return totalWeight;
    }*/
    int getFitness() const {
        if (totalWeight <= maximumBackpackWeight) {
            return totalValue;
        }
        return 0;
    }
    const vector<bool>& getGenotype() const {
        return itemsBitArray;
    }
};

vector<Backpack> backpacks;

Backpack tournamentSelection(const vector<Backpack>& population, const vector<int>& fitnessScores, int tournamen“Size) {
    int populationSize = population.size();

    Backpack selectedBackpack;
    int bestFitness = INT_MIN;
    for (int i = 0; i < tournamen“Size; i++) {
        int randomIndex = rand() % populationSize;
        int currentFitness = fitnessScores[randomIndex];

        if (currentFitness > bestFitness) {
            bestFitness = currentFitness;
            selectedBackpack = population[randomIndex];
        }
    }
    return selectedBackpack;
}

vector<int> getPopulationFitnessScores(const vector<Backpack>& population) {
    vector<int> fitnessScores(population.size());
    for (int i = 0; i < population.size(); i++) {
        fitnessScores[i] = population[i].getFitness();
    }
    return fitnessScores;
}

void makeItemsWeightsValid(vector<bool>& items) {
    vector<int> indexesOfTrueAllels;
    int totalWeight = 0;
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i])
        {
            totalWeight += itemsToChooseFrom[i].kilograms;
            indexesOfTrueAllels.push_back(i);
        }
    }
    while (totalWeight > maximumBackpackWeight)
    {
        int indexOfIndexToRemove = rand() % indexesOfTrueAllels.size();
        int randomIndexToRemove = indexesOfTrueAllels[indexOfIndexToRemove];
        items[randomIndexToRemove] = false;
        totalWeight -= itemsToChooseFrom[randomIndexToRemove].kilograms;
        indexesOfTrueAllels.erase(indexesOfTrueAllels.begin() + indexOfIndexToRemove);
    }
}

void initialisation(vector<Item>& itemsVector) {
    cin >> maximumBackpackWeight >> numberOfObjects;
    int currentKilograms = 0;
    int currentValue = 0;
    for (int i = 0; i < numberOfObjects; i++) {
        Item currentItem=Item();
        cin >> currentKilograms >> currentValue;
        currentItem.kilograms = currentKilograms;
        currentItem.value = currentValue;
        itemsVector.push_back(currentItem);
    }
}

void mutation(vector<bool>& chromosome, double mutationRatePerGene) {
    for (auto gene : chromosome) {
        if ((rand() % 10000) / 10000.0 <= mutationRatePerGene) {
            gene = !gene;
        }
    }
}

void initialisationOfRandomBackpacks() {
    for (int i = 0; i < populationCount; i++)
    {
        vector<bool> items(itemsToChooseFrom.size());
        int weightReached = 0;

        while (weightReached <= maximumBackpackWeight)
        {
            int randomItemIndex = 0 + (std::rand() % itemsToChooseFrom.size());
            Item randomItem = itemsToChooseFrom[randomItemIndex];
            if (weightReached + randomItem.kilograms < maximumBackpackWeight)
            {
                items[randomItemIndex] = true;
                weightReached += randomItem.kilograms;
            }
            else
            {
                break;
            }
        }
        //makeItemsWeightsValid(items);
        backpacks.push_back(Backpack(items));
    }
}

vector<bool> twoPointCrossover(const vector<bool>& firstParent, const vector<bool>& secondParent, double crossOverRate) {
    vector<bool> kid;
    if ((rand() % 100) / 100.0 <= crossOverRate) {
        int sizeOfParents = firstParent.size();
        int firstCrossoverIndex = rand() % sizeOfParents;
        int secondCrossoverIndex = rand() % sizeOfParents;
        if (firstCrossoverIndex > secondCrossoverIndex) {
            swap(firstCrossoverIndex, secondCrossoverIndex);
        }
        kid.insert(kid.end(), firstParent.begin(), firstParent.begin() + firstCrossoverIndex);
        kid.insert(kid.end(), secondParent.begin()+firstCrossoverIndex, secondParent.begin() + secondCrossoverIndex);
        kid.insert(kid.end(), firstParent.begin()+secondCrossoverIndex, firstParent.end());
    }
    else {
        int selectedParent = rand() % 2;
        kid = (selectedParent == 0) ? firstParent : secondParent;
    }
    return kid;
}
vector<Backpack> generateNewPopulation(const vector<Backpack>& oldPopulation) {
    vector<int> fitnessScores = getPopulationFitnessScores(backpacks);
    int totalFitness = 0;
    int maxFitness = INT_MIN;
    //Best fitness
    for (int i = 0; i < fitnessScores.size(); i++)
    {
        totalFitness += fitnessScores[i];
        if (fitnessScores[i] > maxFitness)
        {
            maxFitness = fitnessScores[i];
            if (maxFitness > maxFoundFitness)
            {
                maxFoundFitness = maxFitness;
            }
        }
    }
    vector<Backpack> newPopulation(oldPopulation.size());
    for (int i = 0; i < populationCount; i++)
    {
        const Backpack& firstParent = tournamentSelection(oldPopulation, fitnessScores, tournamentSize);
        const Backpack& secondParent = tournamentSelection(oldPopulation, fitnessScores, tournamentSize);
        auto newItems = twoPointCrossover(firstParent.getGenotype(), secondParent.getGenotype(), crossoverRate);
        mutation(newItems, mutationRate);
        makeItemsWeightsValid(newItems);
        newPopulation[i] = Backpack(newItems);
    }
    //std::cout<< "Population max fitness" <<maxFitness << "\n";
    cout << maxFitness << "\n";
    return newPopulation;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    initialisation(itemsToChooseFrom);
    initialisationOfRandomBackpacks();
    for (int i = 0; i < numberOfPopulations; i++) {
        backpacks = generateNewPopulation(backpacks);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / static_cast<double>(1000000);
    std::cout << std::fixed << std::setprecision(2) << duration;
    cout << "Maximum fitness across all populations: " << maxFoundFitness << "\n"; 
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
