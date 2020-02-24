#include <bits/stdc++.h>
#include "ClassifierCase3.h"


ClassifierCase3::ClassifierCase3(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb)
    : ClassifierBase(_means, _covariances, _priorProb){

    A0 = -0.5f * covariances[0].inverse();
    A1 = -0.5f * covariances[1].inverse();
    B0 = covariances[0].inverse() * means[0];
    B1 = covariances[1].inverse() * means[1];
    C0 = -0.5f * means[0].transpose() * covariances[0].inverse() * means[0] - 0.5f * logf(covariances[0].determinant()) + logf(priorProb[0]);
    C1 = -0.5f * means[1].transpose() * covariances[1].inverse() * means[1] - 0.5f * logf(covariances[1].determinant()) + logf(priorProb[1]);
}

//Returns the index of the class the input belongs to
int ClassifierCase3::Classify(const VectorXf &input){
    float G0 = input.dot(A0 * input) + input.dot(B0) + C0;
    float G1 = input.dot(A1 * input) + input.dot(B1) + C1;
    if(G0 >= G1){
        return 0;
    } else{
        return 1;
    }
}

//Returns the parameters for the decision boundary
PlotParams ClassifierCase3::GetPlotParams(){
    float c = A0(0, 1) - A1(0, 1);
    if(c != 0)
    {
        std::cerr << "COVARIANCES PER CLASS MUST BE EQUAL FOR THE CURRENT PLOTTING IMPLEMENTATION!" << std::endl;
    }

    float a = A0(1, 1) - A1(1, 1);
    float b = A0(0, 0) - A1(0, 0);
    float d = B0(0, 0) - B1(0, 0);
    float f = B0(1, 0) - B1(1, 0);
    float g = C0 - C1;

    PlotParams ret;
    ret.linear = false;
    ret.p1 = -f / (2 * a);
    ret.p2 = -b / a;
    ret.p3 = -d / a;
    ret.p4 = (f * f) / (4 * a * a) - g / a;
    return ret;
}
