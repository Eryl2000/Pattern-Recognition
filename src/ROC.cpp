# include "ROC.h"

namespace ROC
{
        /*
    Input: vector of data points, classifer
    Returns min, max, step
    */
    ThresholdRange GetThresholdRange(vector<float> discriminants, int numThresholds)
    {
        if(discriminants.size() == 0)
        {
            throw std::invalid_argument("GetThresholdRange cannot compute on empty data!");
        }

        ThresholdRange ret;
        ret.min = *std::min_element(discriminants.begin(), discriminants.end());
        ret.max = *std::max_element(discriminants.begin(), discriminants.end());
        ret.step = (ret.max - ret.min) / numThresholds;

        return ret;
    }

    /*
    Input: min, max, step, vector of data points
    Generate ROC values (False Positive/False Negative) for each threshold value
    */
    vector<MisclassificationData> GetROCValues(ThresholdRange tRange, vector<float> discriminants, vector<bool> label)
    {
        if(discriminants.size() != label.size())
        {
            throw std::invalid_argument("GetROCValues discriminants and data must be the same length!");
        }

        vector<MisclassificationData> ret;
        //std::cout << "Min: " << tRange.min << ", Max: " << tRange.max << ", Step: " << tRange.step << std::endl;
        ret.reserve(int((tRange.max - tRange.min) / tRange.step));
        // Should generate for [tRange.min, tRange.max] with step tRange.step
        for(float thresh = tRange.min; thresh <= tRange.max; thresh += tRange.step)
        {
            //std::cout << "Thresh: " << std::to_string(thresh) << std::endl;
            MisclassificationData currentMisclass;
            for(unsigned int i = 0; i < discriminants.size(); i++)
            {
                bool belongsToClass = discriminants[i] >= thresh;
                if(!belongsToClass && label[i])
                {
                    currentMisclass.falsePositives++;
                } else if(belongsToClass && !label[i])
                {
                    currentMisclass.falseNegatives++;
                }
            }
            
            ret.push_back(currentMisclass);
        }

        return ret;
    }

    /*
    Input, min, max, step, vector of ROC values
    Returns threshold with smallest magnitude
    */
    float GetBestThreshold(ThresholdRange tRange, const vector<MisclassificationData> & rocValues, float fpToFnRatio = 0.5f)
    {
        if(rocValues.size() == 0)
        {
            throw std::invalid_argument("GetBestThreshold cannot compute on an empty ROC vector!");
        }

        float currentThreshold = tRange.min;
        float bestThreshold = currentThreshold;
        float bestEqualError = abs((1 - fpToFnRatio) * rocValues[0].falseNegatives - fpToFnRatio * rocValues[0].falsePositives);
        int rocIndex = 0;

        currentThreshold += tRange.step;
        for(unsigned int i = 1; i < rocValues.size(); i++)
        {
            float errorDiff = abs((1 - fpToFnRatio) * rocValues[i].falseNegatives - fpToFnRatio * rocValues[i].falsePositives);
            if(errorDiff < bestEqualError)
            {
                bestEqualError = errorDiff;
                bestThreshold = currentThreshold;
                rocIndex = i;
            }
            currentThreshold += tRange.step;
        }

        std::cout << "Best threshold: " << std::endl;
        std::cout << "FN: " << rocValues[rocIndex].falseNegatives << ", FP: " << rocValues[rocIndex].falsePositives << std::endl;
        std::cout << "Equal error: " << bestEqualError << std::endl;

        return bestThreshold;
    }

    /*
    Plot helper function
    Plots vector of ROC values
    */
    void PlotROC(std::string name,
                 const vector<vector<MisclassificationData>> & rocValues,
                 vector<std::string> classifierNames,
                 std::pair<int, int> totalPostiveNegatives,
                 std::pair<bool, bool> invert, 
                 bool verbose)
    {
        if(rocValues.size() != classifierNames.size())
        {
            throw std::invalid_argument("PlotROC: rocValues and classifierNames must have the same size!");
        }

        vector<vector<MisclassificationData>> values = rocValues;
        vector<vector<std::pair<float, float>>> plotData(values.size());

        for(unsigned int i = 0; i < values.size(); i++)
        {
            plotData[i].resize(values[i].size());
            for(unsigned int j = 0; j < values[i].size(); j++)
            {
                std::pair<float, float> point(values[i][j].falsePositives, values[i][j].falseNegatives);
                point.first /= totalPostiveNegatives.first;
                point.second /= totalPostiveNegatives.second;

                if(invert.first)
                {
                    point.first = 1 - point.first;
                }

                if(invert.second)
                {
                    point.second = 1 - point.second;
                }

                plotData[i][j] = point;
            }
        }

        std::pair<std::string, std::string> axisNames;
        axisNames.first = !invert.first ? "False Positives" : "True Negatives";
        axisNames.second = !invert.second ? "False Negatives" : "True Positives";

        Plot::PlotPairs(name, axisNames, classifierNames, plotData, "lines", verbose);

    }

    std::vector<int> ClassfyPixels(float thresh, vector<float> discriminants)
    {
        std::vector<int> ret(discriminants.size());
        for(unsigned int i = 0; i < discriminants.size(); i++)
        {
            ret[i] = discriminants[i] >= thresh ? 1 : 0;
        }

        return ret;
    }
}