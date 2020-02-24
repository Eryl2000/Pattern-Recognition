#include <iostream>
#include <vector>
#include <fstream>
#include "ClassifierBase.h"
#include "Eigen/Dense"
#include "BoxMuller.h"
#include "gnuplot.h"

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




    /*

    std::ofstream outFile;
    outFile.open("scatter.dat");

    for(int i = 0; i < 100; i++)
    {
        float x = box_muller(0, 1);
        float y = box_muller(0, 1);
        outFile << x << " " << y << std::endl;

    }

    GnuplotPipe gp;
    gp.sendLine("plot 'scatter.dat' with points pt 7, x with lines");

    outFile.close();
    */
}