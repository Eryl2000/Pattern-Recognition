#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "ClassifierBase.h"
#include "ClassifierCase1.h"
#include "ClassifierCase3.h"
#include "Eigen/Dense"
#include "BoxMuller.h"
#include "gnuplot.h"
#include "Plot.h"

using namespace Eigen;

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier);

int main()
{
    //Data set 1
    std::vector<VectorXf> mean1;
    mean1.push_back(VectorXf(2));
    mean1[0] << 1.0f, 1.0f;
    mean1.push_back(VectorXf(2));
    mean1[1] << 4.0f, 4.0f;

    std::vector<MatrixXf> covariance1;
    covariance1.push_back(MatrixXf(2, 2));
    covariance1.push_back(MatrixXf(2, 2));
    covariance1[0] << 1, 0,
                      0, 1;
    covariance1[1] << 1, 0,
                      0, 1;

    //Data set 2
    std::vector<VectorXf> mean2;
    mean2.push_back(VectorXf(2));
    mean2[0] << 1.0f, 1.0f;
    mean2.push_back(VectorXf(2));
    mean2[1] << 4.0f, 4.0f;

    std::vector<MatrixXf> covariance2;
    covariance2.push_back(MatrixXf(2, 2));
    covariance2[0] << 1, 0,
                      0, 1;
    covariance2.push_back(MatrixXf(2, 2));
    covariance2[1] << 4, 0,
                      0, 8;

    //Prior probabilities
    std::vector<float> priorProb1;
    priorProb1.push_back(0.5f);
    priorProb1.push_back(0.5f);

    std::vector<float> priorProb2;
    priorProb2.push_back(0.2f);
    priorProb2.push_back(0.8f);

    long numberOfPoints = 200000;
    std::vector<Data> points1;
    points1.reserve(numberOfPoints);
    std::vector<Data> points2;
    points2.reserve(numberOfPoints);
    for(long int i = 0; i < numberOfPoints / 2; ++i)
    {
        for(int j = 0; j < 2; ++j){
            Data d(2);
            float x = box_muller(mean1[j](0, 0), sqrt(covariance1[j](0, 0)));
            float y = box_muller(mean1[j](1, 0), sqrt(covariance1[j](1, 1)));
            d.feature(0, 0) = x;
            d.feature(1, 0) = y;
            d.label = j;
            points1.push_back(d);
        }

        for(int j = 0; j < 2; ++j){
            Data d(2);
            float x = box_muller(mean2[j](0, 0), sqrt(covariance2[j](0, 0)));
            float y = box_muller(mean2[j](1, 0), sqrt(covariance2[j](1, 1)));
            d.feature(0, 0) = x;
            d.feature(1, 0) = y;
            d.label = j;
            points2.push_back(d);
        }
    }

    ClassifierBase *classifier1 = new ClassifierCase1(mean1, covariance1, priorProb1);
    ClassifierBase *classifier2 = new ClassifierCase1(mean1, covariance1, priorProb2);
    ClassifierBase *classifier3 = new ClassifierCase3(mean2, covariance2, priorProb1);
    ClassifierBase *classifier4 = new ClassifierCase3(mean2, covariance2, priorProb2);
    ClassifierBase *classifier5 = new ClassifierCase1(mean2, covariance1, priorProb1);
    std::cout << "\nProblem 1a" << std::endl;
    EvaluateData("Problem 1a", points1, classifier1);
    std::cout << "\nProblem 1b" << std::endl;
    EvaluateData("Problem 1b", points1, classifier2);
    std::cout << "\nProblem 2a" << std::endl;
    EvaluateData("Problem 2a", points2, classifier3);
    std::cout << "\nProblem 2b" << std::endl;
    EvaluateData("Problem 2b", points2, classifier4);
    std::cout << "\nProblem 3" << std::endl;
    EvaluateData("Problem 3", points2, classifier5);

    delete(classifier1);
    delete(classifier2);
    delete(classifier3);
    delete(classifier4);
    delete(classifier5);
    return 0;
}

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier){
    PlotParams plotParams = classifier->GetPlotParams();
    std::vector<MisclassificationData> misclassifications = classifier->GetMisclassification(data);
    int totalMisclassified = 0;
    for(unsigned int i = 0; i < misclassifications.size(); ++i){
        totalMisclassified += misclassifications[i].falsePositives;
        std::cout << "  Class " << i << ":" << std::endl;
        std::cout << "      False positives: " << misclassifications[i].falsePositives << " - (" << 200.0f * misclassifications[i].falsePositives / data.size() << "%)" << std::endl;
        std::cout << "      False negatives: " << misclassifications[i].falseNegatives << " - (" << 200.0f * misclassifications[i].falseNegatives / data.size() << "%)" << std::endl;
    }
    std::cout << "  Total model misclassifications: " << totalMisclassified << " - (" << 100.0f * totalMisclassified / data.size() << "%)" << std::endl;
    std::cout << "  Bhattacharyya error bound: " << 100.0f * classifier->GetErrorBound() << "%" << std::endl;
    plotCompare(name, data, plotParams, false);
}