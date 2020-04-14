#ifndef CMC_H_
#define CMC_H_

#include <vector>
#include <utility>

namespace CMC{

    std::vector<float> GenerateCMC(std::string testingImagesDirectory, const Eigenface &eigenface, float infoRatio);
    std::vector<std::pair<float, float>> ConvertToPoints(const std::vector<float> & cmcData);

}

#endif