#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "ClassifierBase.h"
#include "ClassifierCase1.h"
#include "Eigen/Dense"
#include "BoxMuller.h"
#include "gnuplot.h"
#include "Plot.h"

using namespace Eigen;

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
    covariance2[0] << 4, 0,
                      0, 8;

    //Prior probabilities
    std::vector<float> priorProb1;
    priorProb1.push_back(0.5f);
    priorProb1.push_back(0.5f);

    std::vector<float> priorProb2;
    priorProb2.push_back(0.2f);
    priorProb2.push_back(0.8f);

    long numberOfPoints = 100000;
    std::vector<Data> points1a;
    points1a.reserve(numberOfPoints);
    std::vector<Data> points1b;
    points1b.reserve(numberOfPoints);
    std::vector<Data> points2a;
    points2a.reserve(numberOfPoints);
    std::vector<Data> points2b;
    points2b.reserve(numberOfPoints);
    for(long int i = 0; i < numberOfPoints; ++i)
    {
        //Data set 1
        Data d1(2);
        float x = box_muller(mean1[0](0, 0), covariance1[0](0, 0));
        float y = box_muller(mean1[0](1, 0), covariance1[0](1, 1));
        d1.feature(0, 0) = x;
        d1.feature(1, 0) = y;
        d1.label = 0;
        points1a.push_back(d1);

        Data d2(2);
        x = box_muller(mean1[1](0, 0), covariance1[0](0, 0));
        y = box_muller(mean1[1](1, 0), covariance1[0](1, 1));
        d2.feature(0, 0) = x;
        d2.feature(1, 0) = y;
        d2.label = 1;
        points1b.push_back(d2);

        //Data set 2
        Data d3(2);
        x = box_muller(mean2[0](0, 0), covariance2[0](0, 0));
        y = box_muller(mean2[0](1, 0), covariance2[0](1, 1));
        d3.feature(0, 0) = x;
        d3.feature(1, 0) = y;
        d3.label = 0;
        points2a.push_back(d3);

        Data d4(2);
        x = box_muller(mean2[1](0, 0), covariance2[1](0, 0));
        y = box_muller(mean2[1](1, 0), covariance2[1](1, 1));
        d4.feature(0, 0) = x;
        d4.feature(1, 0) = y;
        d4.label = 1;
        points2b.push_back(d4);
    }

    ClassifierCase1 classifier1(mean1, covariance1, priorProb1);
    ClassifierCase1 classifier2(mean1, covariance1, priorProb2);
    //ClassifierCase3 classifier3(mean2, covariance2, priorProb1);
    //ClassifierCase3 classifier4(mean2, covariance2, priorProb2);
    //ClassifierCase1 classifier5(mean2, covariance2, priorProb2);

    PlotParams points1Params_a = classifier1.GetPlotParams();
    plotCompare("Plot1a", points1a, points1b, points1Params_a.a, points1Params_a.b, points1Params_a.c);
    std::cout << "Error bound 1a: " << classifier1.GetErrorBound() << std::endl;

    PlotParams points1Params_b = classifier2.GetPlotParams();
    plotCompare("Plot1b", points1a, points1b, points1Params_b.a, points1Params_b.b, points1Params_b.c);
    std::cout << "Error bound 1b: " << classifier2.GetErrorBound() << std::endl;

}