#ifndef ROC_H_
#define ROC_H_

#include <vector>
#include <stdexcept>
#include <algorithm>

#include "Data.h"
#include "gnuplot.h"
#include "Plot.h"

using std::vector;

namespace ROC
{
    class ThresholdRange;

    ThresholdRange GetThresholdRange(vector<float> discriminants, int numThresholds);
    vector<MisclassificationData> GetROCValues(ThresholdRange tRange, vector<float> discriminants, vector<bool> data);
    float GetBestThreshold(ThresholdRange tRange, const vector<MisclassificationData> & rocValues, float fpToFnRatio);
    void PlotROC(std::string name, const vector<vector<MisclassificationData>> & rocValues, vector<std::string> classifierNames, bool verbose = false);
    std::vector<int> ClassfyPixels(float thresh, vector<float> discriminants);

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
}

#endif