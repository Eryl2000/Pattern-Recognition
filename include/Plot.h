#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ClassifierBase.h"
#include "gnuplot.h"

static void plotCompare(std::string plotName, std::vector<Data> data, float aFit, float bFit, float cFit, bool verbose);
static void createDataFile(std::string plotFileName, std::vector<Data> data, int label);

static void plotCompare(std::string plotName, std::vector<Data> data, float aFit, float bFit, float cFit, bool verbose=false)
{
    std::string class1FileName = "plots/" + plotName + "_c1.dat";
    std::string class2FileName = "plots/" + plotName + "_c2.dat";

    createDataFile(class1FileName, data, 0);
    createDataFile(class2FileName, data, 1);

    GnuplotPipe gp;
    std::string plotString;
    plotString += "set title '" + plotName + "' font ',20'\n";
    plotString += "set xlabel 'X'\n";
    plotString += "set ylabel 'Y'\n";
    plotString += "plot '" + class2FileName + "' with points pt 7 ps 0.1 lc rgb 'red', ";
    plotString += "'" + class1FileName + "' with points pt 7 ps 0.1 lc rgb 'black', ";
    plotString += std::to_string(aFit) + " * x**2 + " + std::to_string(bFit) + " * x + " + std::to_string(cFit) + " with lines";

    if(verbose)
    {
        std::cout << plotString << std::endl;
    }

    gp.sendLine(plotString);
    std::cout << "Class 0: black,  Class 1: red" << std::endl;
}

static void createDataFile(std::string plotFileName, std::vector<Data> data, int label)
{
    std::ofstream outFile(plotFileName);
    for(unsigned int i = 0; i < data.size(); i++)
    {
        if(data[i].label == label){
            for(int j = 0; j < data[i].feature.size(); j++)
            {
                outFile << data[i].feature[j] << " ";
            }
            outFile << std::endl;
        }
    }
    outFile.close();
}