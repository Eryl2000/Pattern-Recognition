#ifndef PLOT_H_
#define PLOT_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include "gnuplot.h"
#include "Data.h"

namespace Plot
{

    class PlotParams{
        public:
            bool linear;
            float m;
            float b;
            
            float p1;
            float p2;
            float p3;
            float p4;
    };

    void plotCompare(std::string plotName, std::vector<Data> data, PlotParams params, bool verbose = false);
    void createDataFile(std::string plotFileName, std::vector<Data> data, int label);

    void CreatePairDataFile(std::string fileName, std::vector<std::pair<float, float>> data);
    void PlotPairs(std::string chartName,
                   std::pair<std::string, std::string> axisNames,
                   std::vector<std::string> plotNames,
                   std::vector<std::vector<std::pair<float, float>>> data,
                   std::string outputType = "lines",
                   bool verbose = false);
    void createROCDataFile(std::string plotFileName, const std::vector<MisclassificationData> & rocValues);

}

#endif