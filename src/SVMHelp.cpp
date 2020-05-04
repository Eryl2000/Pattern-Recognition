#include "SVMHelp.h"
#include <new>
#include <iostream>

namespace SVMHelp
{
    svm_problem GetSVMProblem(const std::vector<Data> & data)
    {
        svm_problem problem;

        problem.l = data.size();
        problem.y = new double[problem.l];
        problem.x = new svm_node*[problem.l];
        for(unsigned int i = 0; i < data.size(); i++)
        {
            problem.x[i] = GetSVMNode(data[i]);
            problem.y[i] = data[i].label;
            //std::cout << problem.y[i] << ", " << data[i].label << " ";
        }

        return problem;
    }

    svm_node * GetSVMNode(const Data & dataPoint)
    {
        svm_node end;
        end.index = -1;

        unsigned int featureSize = dataPoint.feature.size();
        svm_node * nodeVect = new svm_node[featureSize + 1];
        for(unsigned int j = 0; j < featureSize; j++)
        {
            svm_node node;
            node.index = j;
            node.value = dataPoint.feature[j];
            nodeVect[j] = node;
        }
        nodeVect[featureSize] = end;

        return nodeVect;
    }

    void TearDownSVMProblem(svm_problem & problem)
    {
        if (problem.y != NULL)
        {
            delete[] problem.y;
        }

        if(problem.x != NULL)
        {
            for(int i = 0; i < problem.l; i++)
            {
                if(problem.x[i] != NULL)
                {
                    delete[] problem.x[i];
                }
            }

            delete[] problem.x;
        }
    }


    std::vector<svm_parameter> GetTrainingParameters(const std::vector<double> & C, const std::vector<double> & polynomalDegree, const std::vector<double> & rbfGamma)
    {
        std::vector<svm_parameter> parameters;
        parameters.reserve(C.size() * (polynomalDegree.size() + rbfGamma.size()));
        for(unsigned int i = 0; i < C.size(); i++)
        {
            svm_parameter paramBase;
            paramBase.svm_type = C_SVC;
            paramBase.eps = 0.001;
            paramBase.nr_weight = 0;
            paramBase.C = C[i];
            for(unsigned int j = 0; j < polynomalDegree.size(); j++)
            {
                svm_parameter param = paramBase;
                param.kernel_type = POLY;
                param.degree = polynomalDegree[j];
                param.gamma = 1;
                param.coef0 = 0;
                parameters.push_back(param);
            }

            for(unsigned int j = 0; j < rbfGamma.size(); j++)
            {
                svm_parameter param = paramBase;
                param.kernel_type = RBF;
                param.gamma = rbfGamma[j];
                parameters.push_back(param);
            }
        }

        return parameters;
    }

    std::string GetParameterString(svm_parameter param)
    {
        std::string paramString = "";
        if(param.kernel_type == POLY)
        {
            paramString += "POLY:";
            paramString += " C=" + std::to_string(param.C);
            paramString += " degree=" + std::to_string((int)param.degree);
        } else if(param.kernel_type == RBF)
        {
            paramString += "RBF:";
            paramString += " C=" + std::to_string((int)param.C);
            paramString += " gamma=" + std::to_string(param.gamma);
        } else
        {
            paramString = "INVALID PARAM KERNEL TYPE";
        }
        
        return paramString;
    }

    svm_model * TrainModel(svm_parameter parameter, const std::vector<Data> & trainingData)
    {
        svm_problem problem = GetSVMProblem(trainingData);
        svm_model * model = svm_train(&problem, &parameter);

        // TODO: Teardown SVMproblem after model is finished
        //TearDownSVMProblem(problem);

        return model;
    }

    std::vector<std::vector<svm_model *>> TrainModels(const std::vector<std::vector<std::vector<Data>>> & data,
                                                      const std::vector<double> & C,
                                                      const std::vector<double> & polynomalDegree,
                                                      const std::vector<double> & rbfGamma)
    {
        std::vector<svm_parameter> parameters = GetTrainingParameters(C, polynomalDegree, rbfGamma);
        return TrainModels(data, parameters);
    }

    std::vector<std::vector<svm_model *>> TrainModels(const std::vector<std::vector<std::vector<Data>>> & data,
                                                      const std::vector<svm_parameter> & parameters)
    {
        std::vector<std::vector<svm_model *>> models(data.size());

        for(unsigned int i = 0; i < data.size(); i++)
        {
            models[i] = std::vector<svm_model*>(parameters.size());
            for(unsigned int j = 0; j < parameters.size(); j++)
            {
                std::cout << std::endl << "TRAINING - " << GetParameterString(parameters[j]) << std::endl;
                models[i][j] = TrainModel(parameters[j], data[i][0]);
            }
        }

        return models;
    }

    float GetSVMErrorRate(svm_model * model, const std::vector<Data> & testingData)
    {
        int errorCount = 0;
        for(unsigned int i = 0; i < testingData.size(); i++)
        {
            svm_node * testingNodes = GetSVMNode(testingData[i]);

            double label = svm_predict(model, testingNodes);
            if(label != testingData[i].label)
            {
                errorCount++;
            }


            if(testingNodes != NULL)
            {
                delete[] testingNodes;
            }
        }

        return (float)errorCount / testingData.size();
    }

    // models - fold by svm type 2d vector of trained models
    // data - fold by training/testing data 3d vector of data
    // Returns error of each parameter averaged for each fold
    std::vector<float> GetSVMErrorRate(const std::vector<std::vector<svm_model *>> & models, const std::vector<std::vector<std::vector<Data>>> & data)
    {
        if(models.size() == 0)
        {
            throw std::invalid_argument("SVMHelp GetSVMErrorRate passed empty models");
        }
        std::vector<float> errorRate(models[0].size(), 0);
        for(unsigned int i = 0; i < models.size(); i++)
        {
            for(unsigned j = 0; j < models[i].size(); j++)
            {
                // Add the error rate for a given parameter for each fold
                errorRate[j] += GetSVMErrorRate(models[i][j], data[i][1]);
            }
        }


        for(unsigned i = 0; i < errorRate.size(); i++)
        {
            // Divide each error rates by the number of folds
            errorRate[i] /= models.size();
        }

        return errorRate;
    }
}