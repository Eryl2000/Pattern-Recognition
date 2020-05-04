#ifndef EIGENLOADER_H_
#define EIGENLOADER_H_

#include <vector>
#include <string>

#include "Eigen/Dense"
#include "Data.h"

using namespace Eigen;
using std::vector, std::string;

namespace Eigenloader
{
    class NormalizationFactors
    {
        public:
            float min;
            float max;

            NormalizationFactors() : min(0), max(0) {}

            NormalizationFactors(float _min, float _max)
            {
                min = _min;
                max = _max;
            }
    };

    class Eigenset
    {
        public:
            string reprFile;
            string labelFile;

            Eigenset(string _reprFile, string _labelFile)
            {
                reprFile = _reprFile;
                labelFile = _labelFile;
            }
    };

    Eigenset GetSetFileNames(string fileDirectory, int foldNumber, string setType);

    // Returns 3d vector of Data
    // For each 3 folds
    // For training and testing sets
    vector<vector<vector<Data>>> LoadFullData(string fileDirectory, int foldCount, int eigenspaceCount);

    vector<Data> LoadTrainingData(string fileDirectory, int foldNumber, int eigenspaceCount);
    vector<Data> LoadTestingData(string fileDirectory, int foldNumber, int eigenspaceCount);

    vector<Data> LoadData(string fileDirectory, int foldNumber, string setType, int eigenspaceCount);
    vector<Data> LoadData(Eigenset dataSet, int eigenspaceCount);

    vector<Data> NormalizeData(const vector<Data> & data, vector<NormalizationFactors> & normal, bool computeNormal = false);

    vector<NormalizationFactors> GetNormalizationFactors(const vector<Data> & data);
}

#endif