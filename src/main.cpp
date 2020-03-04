#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "ClassifierBase.h"
#include "ClassifierCase1.h"
#include "ClassifierCase3.h"
#include "Eigen/Dense"
#include "BoxMuller.h"
#include "gnuplot.h"
#include "Plot.h"
#include "Image.h"

using namespace Eigen;

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier);

int main(int argc, char *argv[])
{
    Image<RGB> image1(argv[1]);
    image1.WriteToFile(argv[2]);
    
    return 0;
}

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier){
    PlotParams plotParams = classifier->GetPlotParams();
    std::vector<MisclassificationData> misclassifications = classifier->GetMisclassification(data);
    int totalMisclassified = 0;
    for(unsigned int i = 0; i < misclassifications.size(); ++i){
        totalMisclassified += misclassifications[i].falsePositives;
        std::cout << "  Class " << i << ":" << std::endl;
        std::cout << "      False positives: " << misclassifications[i].falsePositives << " - (" << 200.0f * misclassifications[i].falsePositives / data.size() << "%)" << std::endl;
        std::cout << "      False negatives: " << misclassifications[i].falseNegatives << " - (" << 200.0f * misclassifications[i].falseNegatives / data.size() << "%)" << std::endl;
    }
    std::cout << "  Total model misclassifications: " << totalMisclassified << " - (" << 100.0f * totalMisclassified / data.size() << "%)" << std::endl;
    std::cout << "  Bhattacharyya error bound: " << 100.0f * classifier->GetErrorBound() << "%" << std::endl;
    plotCompare(name, data, plotParams, false);
}