#include <iostream>
#include "ClassifierCase1.h"
#include "Eigen/Dense"

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

}