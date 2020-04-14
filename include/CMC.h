#ifndef CMC_H_
#define CMC_H_

#include <vector>

namespace CMC{

std::vector<float> GenerateCMC(std::string testingImagesDirectory, const Eigenface &eigenface, float infoRatio);

}

#endif