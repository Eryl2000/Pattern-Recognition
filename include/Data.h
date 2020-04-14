#ifndef DATA_H_
#define DATA_H_

#include <string>
#include <Eigen/Dense>

using namespace Eigen;

class Data
{
public:
    Data(int featureSize)
    {
        feature.resize(featureSize);
    }
    Data()
    {
        
    }
    VectorXf feature;
    int label;
};

class MisclassificationData{
public:
    MisclassificationData(){
        falsePositives = 0;
        falseNegatives = 0;
    }
    MisclassificationData(const MisclassificationData & other)
    {
        falsePositives = other.falsePositives;
        falseNegatives = other.falseNegatives;
    }
    int falsePositives;
    int falseNegatives;
};

#endif