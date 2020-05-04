#ifndef SVM_HELP_H_
#define SVM_HELP_H_

#include "svm.h"
#include "Data.h"

#include <vector>

namespace SVMHelp
{
    svm_problem GetSVMProblem(const std::vector<Data> & data);
    svm_node * GetSVMNode(const Data & dataPoint);
    void TearDownSVMProblem(svm_problem & problem);

    std::vector<svm_parameter> GetTrainingParameters(const std::vector<double> & C, const std::vector<double> & polynomalDegree, const std::vector<double> & rbfGamma);
    std::string GetParameterString(svm_parameter param);

    svm_model * TrainModel(svm_parameter parameter, const std::vector<Data> & trainingData);
    std::vector<std::vector<svm_model *>> TrainModels(const std::vector<std::vector<std::vector<Data>>> & data,
                                                      const std::vector<double> & C,
                                                      const std::vector<double> & polynomalDegree,
                                                      const std::vector<double> & rbfGamma);
    std::vector<std::vector<svm_model *>> TrainModels(const std::vector<std::vector<std::vector<Data>>> & data,
                                                      const std::vector<svm_parameter> & parameters);

    float GetSVMErrorRate(svm_model * model, const std::vector<Data> & testingData);
    std::vector<float> GetSVMErrorRate(const std::vector<std::vector<svm_model *>> & models, const std::vector<std::vector<std::vector<Data>>> & data);
}

#endif