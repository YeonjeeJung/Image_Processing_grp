//
//  getMatrix.hpp
//  Image_Processing
//
//  Created by Yeonjee Jung€ on 24/11/2018.
//  Copyright 2018 Yeonjee Jung. All rights reserved.
//

#ifndef getMatrix_hpp
#define getMatrix_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <math.h>

using namespace std;

vector<vector<double> > getMatrix(vector<pair<int,int> > oriPointVec, vector<pair<int,int> > newPointVec);
vector<vector<double> > inverse(vector<vector<double> > mat);

#endif /* getMatrix_hpp */
