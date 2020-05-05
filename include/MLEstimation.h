// TODO: Convert to namespace and create MLEstimation.cpp

#ifndef ML_ESTIMATION_H_
#define ML_ESTIMATION_H_

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>

#include "Eigen/Dense"
#include "Data.h"

using namespace Eigen;

using std::vector;

namespace MLEstimation
{
    VectorXf GetSampleMean(const vector<Data> & data);
    MatrixXf GetSampleCovariance(const vector<Data> & data);
    vector<Data> GetRandomSample(const vector<Data> & data, float sampleRatio);
}

#endif