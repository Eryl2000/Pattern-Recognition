#ifndef ROC_H_
#define ROC_H_

#include <vector>
#include <stdexcept>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

#include "ClassifierBase.h"
#include "gnuplot.h"
#include "Plot.h"

using std::vector;

class ThresholdRange;
class ROCValue;

void createROCDataFile(std::string plotFileName, const std::vector<ROCValue> & rocValues);

ThresholdRange GetThresholdRange(const vector<Data> & data, ClassifierBase * classifier);
vector<ROCValue> GetROCValues(ThresholdRange tRange, const vector<Data> & data, ClassifierBase * classifier);
float GetBestThreshold(ThresholdRange tRange, const vector<ROCValue> & rocValues);
void PlotROC(std::string name, const vector<vector<ROCValue>> & rocValues, vector<std::string> classifierNames, bool verbose);


class ThresholdRange
{
    public:
        ThresholdRange(float _min, float _max, float _step)
        {
            min = _min;
            max = _max;
            step = _step;
        }
        float min;
        float max;
        float step;
};

class ROCValue
{
    public:
        ROCValue(int _fPs, int _fNs)
        {
            falsePositives = _fPs;
            falseNegatives = _fNs;
        }
        int falsePositives;
        int falseNegatives;
};

/*
Input: vector of data points, classifer
Returns min, max, step
TODO: Changed ClassifierBase to ThresholdedClassifier
*/
ThresholdRange GetThresholdRange(const vector<Data> & data, ClassifierBase * classifier)
{
    // TODO: Implement
}

/*
Input: min, max, step, vector of data points
Generate ROC values (False Positive/False Negative) for each threshold value
TODO: Changed ClassifierBase to ThresholdedClassifier
*/
vector<ROCValue> GetROCValues(ThresholdRange tRange, const vector<Data> & data, ClassifierBase * classifier)
{
    // TODO: Implement
    // Should generate for [tRange.min, tRange.max] with step tRange.step
}

/*
Input, min, max, step, vector of ROC values
Returns threshold with smallest magnitude
*/
float GetBestThreshold(ThresholdRange tRange, const vector<ROCValue> & rocValues)
{
    if(rocValues.size() == 0)
    {
        throw std::invalid_argument("GetBestThreshold cannot compute on an empty ROC vector!");
    }

    float currentThreshold = tRange.min;
    float bestThreshold = currentThreshold;
    float bestEqualError = abs(rocValues[0].falseNegatives - rocValues[0].falsePositives);

    currentThreshold += tRange.step;
    for(int i = 1; i < rocValues.size(); i++)
    {
        float errorDiff = abs(rocValues[0].falseNegatives - rocValues[0].falsePositives);
        if(errorDiff < bestEqualError)
        {
            bestEqualError = errorDiff;
            bestThreshold = currentThreshold;
        }
        currentThreshold += tRange.step;
    }

    assertm(currentThreshold == tRange.max, "GetBestThreshold should end at tRange.max");
}

/*
Plot static helper function
Plots vector of ROC values
*/
void PlotROC(std::string name, const vector<vector<ROCValue>> & rocValues, vector<std::string> classifierNames, bool verbose=false)
{
    if(rocValues.size() != classifierNames.size())
    {
        throw std::invalid_argument("PlotROC: rocValues and classifierNames must have the same size!");
    }

    vector<std::string> fileNames(classifierNames.size());
    for(unsigned int i = 0; i < rocValues.size(); i++)
    {
        fileNames[i] = "plots/" + name + "_" + classifierNames[i] + "_roc.dat";
        createROCDataFile(fileNames[i], rocValues[i]);
    }

    GnuplotPipe gp;
    std::string plotString;
    plotString += "set title '" + name + "' font ',20'\n";
    plotString += "set xlabel 'False Positives'\n";
    plotString += "set ylabel 'False Negatives'\n";

    for(unsigned int i = 0; i < rocValues.size(); i++)
    {
        plotString += "plot '" + fileNames[i] + "' with lines lw 3 lc rgb 'red', ";
    }

    if(verbose)
    {
        std::cout << plotString << std::endl;
    }
    gp.sendLine(plotString);
}

#endif