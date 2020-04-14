#ifndef PLOT_H_
#define PLOT_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
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
    void createROCDataFile(std::string plotFileName, const std::vector<MisclassificationData> & rocValues);

}

#endif