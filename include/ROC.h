#ifndef ROC_H_
#define ROC_H_

#include <vector>
#include <stdexcept>
#include <cassert>
#include <algorithm>

#define assertm(exp, msg) assert(((void)msg, exp))

#include "ClassifierBase.h"
#include "ClassifierCase3Thresholded.h"
#include "gnuplot.h"
#include "Plot.h"

using std::vector;

class ThresholdRange;

void createROCDataFile(std::string plotFileName, const std::vector<MisclassificationData> & rocValues);

ThresholdRange GetThresholdRange(const vector<Data> & data, const ClassifierCase3Thresholded & classifier, int numThresholds);
vector<MisclassificationData> GetROCValues(ThresholdRange tRange, const vector<Data> & data, const ClassifierCase3Thresholded & classifier);
float GetBestThreshold(ThresholdRange tRange, const vector<MisclassificationData> & rocValues);
void PlotROC(std::string name, const vector<vector<MisclassificationData>> & rocValues, vector<std::string> classifierNames, bool verbose);


class ThresholdRange
{
    public:
        ThresholdRange()
        {
            min = 0;
            max = 0;
            step = 0;
        }
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

/*
Input: vector of data points, classifer
Returns min, max, step
*/
ThresholdRange GetThresholdRange(const vector<Data> & data, const ClassifierCase3Thresholded & classifier, int numThresholds)
{
    if(data.size() == 0)
    {
        throw std::invalid_argument("GetThresholdRange cannot compute on empty data!");
    }

    vector<float> discriminants(data.size());
    for(unsigned int i = 0; i < data.size(); i++)
    {
        discriminants[i] = classifier.GetDiscriminantValue(data[i].feature);
    }

    ThresholdRange ret;
    ret.min = *std::min_element(discriminants.begin(), discriminants.end());
    ret.max = *std::max_element(discriminants.begin(), discriminants.end());
    ret.step = (ret.min + ret.max) / numThresholds;

    return ret;
}

/*
Input: min, max, step, vector of data points
Generate ROC values (False Positive/False Negative) for each threshold value
*/
vector<MisclassificationData> GetROCValues(ThresholdRange tRange, const vector<Data> & data, ClassifierCase3Thresholded & classifier)
{
    vector<MisclassificationData> ret;
    ret.reserve(int((tRange.max - tRange.min) / tRange.step));
    // Should generate for [tRange.min, tRange.max] with step tRange.step
    for(float thresh = tRange.min; thresh <= tRange.max; thresh += tRange.step)
    {
        classifier.SetThreshold(thresh);
        ret.push_back(classifier.GetMisclassification(data)[0]);
    }

    return ret;
}

/*
Input, min, max, step, vector of ROC values
Returns threshold with smallest magnitude
*/
float GetBestThreshold(ThresholdRange tRange, const vector<MisclassificationData> & rocValues)
{
    if(rocValues.size() == 0)
    {
        throw std::invalid_argument("GetBestThreshold cannot compute on an empty ROC vector!");
    }

    float currentThreshold = tRange.min;
    float bestThreshold = currentThreshold;
    float bestEqualError = abs(rocValues[0].falseNegatives - rocValues[0].falsePositives);

    currentThreshold += tRange.step;
    for(unsigned int i = 1; i < rocValues.size(); i++)
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

    return bestThreshold;
}

/*
Plot static helper function
Plots vector of ROC values
*/
void PlotROC(std::string name, const vector<vector<MisclassificationData>> & rocValues, vector<std::string> classifierNames, bool verbose=false)
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