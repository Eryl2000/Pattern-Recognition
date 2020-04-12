#include <iostream>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"

int main(int argc, char *argv[])
{
    std::cout << "Project 3" << std::endl;
    std::cout << "Training Eigenfaces" << std::endl;
    Eigenface eigenface("./Faces_FA_FB/fa_H/");
    std::cout << "Outputting Average Face" << std::endl;
    Image<GreyScale> averageFace = eigenface.GetAverageImage();
    averageFace.WriteToFile("averageFace.pgm");

    return 0;
}