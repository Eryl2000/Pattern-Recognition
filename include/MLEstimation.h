#include <vector>
#include <stdexcept>
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

    VectorXf mean(data[0].feature.size());
    for(unsigned int i = 0; i < data.size(); i++)
    {
        VectorXf currentPoint = data[i].feature;
        for(int j = 0; j < currentPoint.size(); j++)
        {
            mean[j] += currentPoint[j];
        }
    }

    for(int i = 0; i < mean.size(); i++)
    {
        mean[i] /= (float)data.size();
    }

    return mean;
}

VectorXf GetSampleCovariance(vector<Data> data)
{
    VectorXf mean = GetSampleMean(data);
}

vector<VectorXf> GetRandomSample(vector<Data> data)
{

}