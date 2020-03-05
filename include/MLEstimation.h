#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>

#include "Eigen/Dense"
#include "ClassifierBase.h"

using namespace Eigen;

using std::vector;

VectorXf GetSampleMean(vector<Data> data)
{
    if(data.size() == 0)
    {
        throw std::invalid_argument("GetSampleMean cannot compute on empty data!");
    }

    VectorXf mean = VectorXf::Zero(data[0].feature.size());
    //std::cout << "Init mean:" << std::endl << mean << std::endl;
    for(unsigned int i = 0; i < data.size(); i++)
    {
        mean += data[i].feature;
    }

    mean /= (float)data.size();

    return mean;
}

MatrixXf GetSampleCovariance(vector<Data> data)
{
    VectorXf mean = GetSampleMean(data);
    //std::cout << "Mean:" << std::endl << mean << std::endl;
    MatrixXf covariance = MatrixXf::Zero(mean.size(), mean.size());
    MatrixXf centered = MatrixXf::Zero(mean.size(), data.size());
    for(unsigned int i = 0; i < data.size(); i++)
    {
        centered.block(0, i, mean.size(), 1) = data[i].feature - mean;
    }

    //std::cout << centered << std::endl;

    covariance = (centered * centered.adjoint()) / float(data.size() - 1);

    /*
    for(unsigned int i = 0; i < data.size(); i++)
    {
        covariance += ((data[i].feature - mean) * (data[i].feature - mean).transpose()) / (float)(data.size() - 1);

        if(covariance(0,0) > 2)
        {
            std::cout << "i: " << std::to_string(i) << std::endl;
        }
    }
    */

    //covariance /= (float)(data.size() - 1);

    return covariance;
}

vector<Data> GetRandomSample(vector<Data> data, float sampleRatio)
{
    int sampleSize = (int)(sampleRatio * data.size());
    vector<unsigned int> indices(data.size());
    std::iota(indices.begin(), indices.end(), 0);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(indices.begin(), indices.end(), std::default_random_engine(seed));

    vector<Data> sample(sampleSize);

    for(int i = 0; i < sampleSize; i++)
    {
        sample[i] = data[indices[i]];
    }

    return sample;
}