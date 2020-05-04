#include <iostream>
#include <vector>
#include "svm.h"
#include "MLEstimation.h"
#include "Eigenloader.h"
#include "Data.h"

using std::string, std::vector;

vector<std::pair<string, float>> PerformExperiment(vector<vector<vector<Data>>> data);
void OutputExperimentResults(vector<std::pair<string, float>> results);

int main(int argc, char *argv[])
{
    const int EIGENSPACE_COUNT = 30;
    const string DATA_FOLDER("genderdata/");
    const string LOW_RES_FOLDER("16_20/");
    const string HIGH_RES_FOLDER("48_60/");

    std::cout << "Project 4" << std::endl;

    std::cout << "  Loading High Res Data" << std::endl;
    vector<vector<vector<Data>>> dataHighRes = Eigenloader::LoadFullData(DATA_FOLDER + HIGH_RES_FOLDER, 3, EIGENSPACE_COUNT);
    std::cout << "  Performing High Res Experiment" << std::endl;
    vector<std::pair<string, float>> resultsHighRes = PerformExperiment(dataHighRes);
    OutputExperimentResults(resultsHighRes);


    std::cout << "  Loading Low Res Data" << std::endl;
    vector<vector<vector<Data>>> dataLowRes = Eigenloader::LoadFullData(DATA_FOLDER + LOW_RES_FOLDER, 3, EIGENSPACE_COUNT);
    std::cout << "  Performing Low Res Experiment" << std::endl;
    vector<std::pair<string, float>> resultsLowRes = PerformExperiment(dataLowRes);
    OutputExperimentResults(resultsLowRes);

    return 0;
}

// Pairs of classifier types and error rate
vector<std::pair<string, float>> PerformExperiment(vector<vector<vector<Data>>> data)
{
    // SVM Classifier
    // C = 1, 10, 100, 100
        // Polynomial degree = 1, 2, 3; gamma = 1, coef0 = 0
        // RBF gamma = 1, 10, 100

    // Bayes Classifier

    return {{"Nothing here", -1}};
}

void OutputExperimentResults(vector<std::pair<string, float>> results)
{
    for(unsigned int i = 0; i < results.size(); i++)
    {
        std::cout << "    " << results[i].first << ": " << std::to_string(results[i].second) << std::endl;
    }
}