#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ClassifierBase.h"
#include "gnuplot.h"

static void plotCompare(std::string plotName, std::vector<Data> class1, std::vector<Data> class2, int aFit, int bFit, int cFit, bool verbose);
static void createDataFile(std::string plotFileName, std::vector<Data> data);

static void plotCompare(std::string plotName, std::vector<Data> class1, std::vector<Data> class2, int aFit, int bFit, int cFit, bool verbose=false)
{
    std::string class1FileName = "plots/" + plotName + "_c1.dat";
    std::string class2FileName = "plots/" + plotName + "_c2.dat";

    createDataFile(class1FileName, class1);
    createDataFile(class2FileName, class2);

    GnuplotPipe gp;
    std::string plotString;
    plotString += "set title '" + plotName + "' font ',20'\n";
    plotString += "set xlabel 'Feature 1'\n";
    plotString += "set ylabel 'Feature 2'\n";
    plotString += "plot '" + class1FileName + "' with points pt 7, ";
    plotString += "'" + class2FileName + "' with points pt 7, ";
    plotString += std::to_string(aFit) + " * x**2 + " + std::to_string(bFit) + " * x + " + std::to_string(cFit) + " with lines";

    if(verbose)
    {
        std::cout << plotString << std::endl;
    }

    gp.sendLine(plotString);
}

static void createDataFile(std::string plotFileName, std::vector<Data> data)
{
    std::ofstream outFile(plotFileName);

    for(unsigned int i = 0; i < data.size(); i++)
    {
        for(unsigned int j = 0; j < data[i].feature.size(); j++)
        {
            outFile << data[i].feature[j] << " ";
        }
        outFile << std::endl;
    }

    outFile.close();
}