#include "Plot.h"

namespace Plot
{
    // Plots data points and fit line
    void plotCompare(std::string plotName, std::vector<Data> data, PlotParams params, bool verbose)
    {
        std::string class1FileName = "plots/" + plotName + "_c1.dat";
        std::string class2FileName = "plots/" + plotName + "_c2.dat";

        createDataFile(class1FileName, data, 0);
        createDataFile(class2FileName, data, 1);

        GnuplotPipe gp;
        std::string plotString;
        plotString += "set title '" + plotName + "' font ',20'\n";
        plotString += "set xlabel 'Feature 1'\n";
        plotString += "set ylabel 'Feature 2'\n";
        plotString += "plot '" + class2FileName + "' with points pt 7 ps 0.01 lc rgb 'red', ";
        plotString += "'" + class1FileName + "' with points pt 7 ps 0.01 lc rgb 'black', ";

        if(params.linear)
        {
            plotString += std::to_string(params.m) + " * x + " + std::to_string(params.b) + " with lines lw 3 lc rgb 'orange'";
        } else //quadratic
        {
            plotString += std::to_string(params.p1) + " + sqrt(" + std::to_string(params.p2) + " * x**2 + " + std::to_string(params.p3) + " * x + " + std::to_string(params.p4) + ") with lines lw 3 lc rgb 'orange', ";
            plotString += std::to_string(params.p1) + " - sqrt(" + std::to_string(params.p2) + " * x**2 + " + std::to_string(params.p3) + " * x + " + std::to_string(params.p4) + ") with lines lw 3 lc rgb 'orange'";
        }

        if(verbose)
        {
            std::cout << plotString << std::endl;
        }

        gp.sendLine(plotString);
        std::cout << "Class 0: black,  Class 1: red" << std::endl;
    }

    // Outputs data points to plot file so that it can be used by gnuplot
    void createDataFile(std::string plotFileName, std::vector<Data> data, int label)
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

    // Outputs ROC points to plot file so that it can be used by gnuplot
    void createROCDataFile(std::string plotFileName, const std::vector<MisclassificationData> & rocValues)
    {
        std::ofstream outFile(plotFileName);
        for(unsigned int i = 0; i < rocValues.size(); i++)
        {
            outFile << rocValues[i].falsePositives << " " << rocValues[i].falseNegatives << std::endl;
        }
        outFile.close();
    }
}