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
    MatrixXd m = MatrixXd::Random(3,3);
    m = (m + MatrixXd::Constant(3,3,1.2)) * 50;
    std::cout << "m =" << std::endl << m << std::endl;
    VectorXd v(3);
    v << 1, 2, 3;
    std::cout << "m * v =" << std::endl << m * v << std::endl;
    std::cout << "Hello world" << std::endl;

    std::vector<VectorXf> mean1;
    mean1.push_back(VectorXf(2));
    mean1[0] << 1.0f, 1.0f;
    mean1.push_back(VectorXf(2));
    mean1[1] << 4.0f, 4.0f;
    
    std::vector<MatrixXf> covariance1;
    covariance1.push_back(MatrixXf(2, 2));
    covariance1[0] << 1, 0,
                      0, 1;

    std::vector<float> priorProb1;
    priorProb1.push_back(0.5f);
    priorProb1.push_back(0.5f);

    ClassifierCase1(mean1, covariance1, priorProb1);

    std::vector<Data> points1;
    points1.reserve(100);
    std::vector<Data> points2;
    points2.reserve(100);

    for(int i = 0; i < 100; i++)
    {
        Data d(2);
        float x = box_muller(1, 1);
        float y = box_muller(1, 1);

        d.feature[0] = x;
        d.feature[1]  = y;

        points1.push_back(d);

        x = box_muller(4, 1);
        y = box_muller(4, 1);

        d.feature[0] = x;
        d.feature[1]  = y;

        points2.push_back(d);
    }

    plotCompare("Test Plot", points1, points2, 0, 1, 0);
}