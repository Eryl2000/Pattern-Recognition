#include <iostream>
#include "Eigen/Dense"

int main()
{
	using namespace Eigen;
    MatrixXd m = MatrixXd::Random(3,3);
    m = (m + MatrixXd::Constant(3,3,1.2)) * 50;
    std::cout << "m =" << std::endl << m << std::endl;
    VectorXd v(3);
    v << 1, 2, 3;
    std::cout << "m * v =" << std::endl << m * v << std::endl;
    std::cout << "Hello world" << std::endl;
}