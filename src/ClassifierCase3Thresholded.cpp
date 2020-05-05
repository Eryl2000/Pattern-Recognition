#include <stdexcept>
#include "ClassifierCase3Thresholded.h"
#include "Plot.h"

ClassifierCase3Thresholded::ClassifierCase3Thresholded(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb)
    : ClassifierBase(_means, _covariances, _priorProb){

}

//Returns the index of the class the input belongs to
// 1 if skin, 0 if non-skin
int ClassifierCase3Thresholded::Classify(const VectorXf &input) const{
    if(priorProb.size() == 0){
        std::cerr << "No threshold set" << std::endl;
        exit(0);
    }
    if(GetDiscriminantValue(input) >= priorProb[0]){
        return 1;
    } else{
        return 0;
    }
}

float ClassifierCase3Thresholded::GetDiscriminantValue(const VectorXf &input) const{
    float a = 1.0f / (2 * 3.14f * powf(covariances[0].determinant(), 0.5f));
    float b = -0.5f * ((input - means[0]).transpose() * covariances[0].inverse() * (input - means[0]))(0, 0);
    return a * expf(b);
}

void ClassifierCase3Thresholded::SetThreshold(float threshold){
    priorProb.push_back(threshold);
}

Plot::PlotParams ClassifierCase3Thresholded::GetPlotParams()
{
    throw std::logic_error("ClassifierCase3Thresholded does not have an implementation for GetPlotParams");
}