#include "Eigenloader.h"

#include <fstream>
#include <sstream>

namespace Eigenloader
{
    Eigenset GetSetFileNames(string fileDirectory, int foldNumber, string setType)
    {
        string reprString = setType + "PCA_0" + std::to_string(foldNumber) + ".txt";
        return Eigenset(fileDirectory + reprString, fileDirectory + "T" + reprString);
    }

    vector<vector<vector<Data>>> LoadFullData(string fileDirectory, int foldCount, int eigenspaceCount)
    {
        vector<vector<vector<Data>>> fullData(foldCount);
        for(int i = 0; i < foldCount; i++)
        {
            fullData[i].resize(2);
            vector<Data> trainingData = LoadTrainingData(fileDirectory, i + 1, eigenspaceCount);
            vector<Data> testingData = LoadTestingData(fileDirectory, i + 1, eigenspaceCount);

            vector<NormalizationFactors> normals;
            // Find normal factors with training set
            trainingData = NormalizeData(trainingData, normals, true);

            // Apply training normalization to testing set
            testingData = NormalizeData(testingData, normals);

            fullData[i][0] = trainingData;
            fullData[i][1] = testingData;
        }

        return fullData;
    }

    vector<Data> LoadTrainingData(string fileDirectory, int foldNumber, int eigenspaceCount)
    {
        return LoadData(fileDirectory, foldNumber, "tr", eigenspaceCount);
    }

    vector<Data> LoadTestingData(string fileDirectory, int foldNumber, int eigenspaceCount)
    {
        vector<Data> validation = LoadData(fileDirectory, foldNumber, "val", eigenspaceCount);
        vector<Data> testing = LoadData(fileDirectory, foldNumber, "ts", eigenspaceCount);

        validation.insert(validation.end(), testing.begin(), testing.end());
        return validation;
    }

    vector<Data> LoadData(string fileDirectory, int foldNumber, string setType, int eigenspaceCount)
    {
        return LoadData(GetSetFileNames(fileDirectory, foldNumber, setType), eigenspaceCount);
    }

    vector<Data> LoadData(Eigenset dataSet, int eigenspaceCount)
    {
        std::ifstream input(dataSet.reprFile);
        std::ifstream label(dataSet.labelFile);
        vector<Data> data;

        string line;
        for(int i = 0; i < eigenspaceCount; i++)
        {
            std::getline(input, line);
            std::istringstream lineStream(line);
            float value;
            int j = 0;
            while(lineStream >> value)
            {
                if(i == 0)
                {
                    // Initialize data
                    Data newData = Data(eigenspaceCount);

                    // Get label from label file
                    int labelValue;
                    label >> labelValue;
                    newData.label =  labelValue;

                    data.push_back(newData);
                }
                data[j].feature[i] = value;
                j++;
            }
        }

        input.close();
        label.close();

        return data;
    }

    vector<Data> NormalizeData(const vector<Data> & data, vector<NormalizationFactors> & normal, bool computeNormal)
    {
        if(computeNormal)
        {
            normal = GetNormalizationFactors(data);
        }

        vector<Data> normalData(data.size());

        for(unsigned int i = 0; i < data.size(); i++)
        {
            normalData[i] = Data(normal.size());
        }

        for(unsigned int i = 0; i < normal.size(); i++)
        {
            float min = normal[i].min;
            float max = normal[i].max;
            for(unsigned int j = 0; j < data.size(); j++)
            {
                float feature = data[j].feature[i];
                normalData[j].feature[i] = 2 * (feature - min) / (max - min) - 1;
            }
        }
        return normalData;
    }

    vector<NormalizationFactors> GetNormalizationFactors(const vector<Data> & data)
    {
        if(data.size() == 0)
        {
            throw std::invalid_argument("Eigenloader GetNormalizationFactors passed empty data");
        }

        int featureCount = data[0].feature.size();
        vector<NormalizationFactors> normals(featureCount);
        for(int i = 0; i < featureCount; i++)
        {
            normals[i] = NormalizationFactors(data[0].feature[i], data[0].feature[i]);
            for(unsigned int j = 1; j < data.size(); j++)
            {
                float feature = data[j].feature[i];
                if(feature < normals[i].min)
                {
                    normals[i].min = feature;
                }

                if(feature > normals[i].max)
                {
                    normals[i].max = feature;
                }
            }
        }

        return normals;
    }
}