#include "Eigenface.h"
#include "CMC.h"

namespace CMC{


void GenerateCMC(std::string testingImagesDirectory, const Eigenface &eigenface, float infoRatio){
	std::vector<std::string> testImageNames;
	Image<GreyScale> exampleImage;
	MatrixXf testingImages = eigenface.GetImageMatrix(testingImagesDirectory, testImageNames, exampleImage);
	std::vector<std::vector<EigenfaceError>> matches = eigenface.GetClosestMatches(testingImages, 50, infoRatio);
	for(int n = 1; n <= 50; ++n){
		float numMatches = 0;
		for(unsigned int testIndex = 0; testIndex < matches.size(); ++testIndex){
			bool found = false;
			for(int trainingIndex = 0; trainingIndex < n; ++ trainingIndex){
				std::string trainingName = eigenface.imageNames[ matches[testIndex][trainingIndex].index ];
				std::string testName = testImageNames[testIndex];
				if(eigenface.ImageNamesEqual(testName, trainingName)){
					found = true;
					break;
				}
			}
			if(found){
				numMatches++;
			}
		}
		float accuracy = numMatches / matches.size();
		std::cout << "n: " << n << ", accuracy: " << accuracy << std::endl;
	}
	
}


}