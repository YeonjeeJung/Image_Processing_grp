//
//  main.cpp
//  Image_Processing
//
//  Created by Yeonjee Jung€ on 24/11/2018.
//  Copyright 2018 Yeonjee Jung€. All rights reserved.
//
#include "getMatrix.hpp"
#include "makeResult.hpp"
#include "corner.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <assert.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>

using namespace std;

void testInverse(int size);
void testGetMatrix();
void testMakeResult();
void cornerTest();

void onMouseEvent(int event, int x, int y, int flags, void* cornerVoidPtr)
{
	vector<pair<int, int> >* cornerArr = (vector<pair<int, int> >*) cornerVoidPtr;

	if(event == 1){
		if (cornerArr->size() < 4) {
			cout << "point" << cornerArr->size() + 1 << " : " << "[" << x << ", " << y << "]" << endl;
			cornerArr->push_back(make_pair(x, y));
		}
		else {
			cout << "Already 4 corners, Press Any key to finish selecting" << endl;
		}
	}
}

int main() {
	clock_t begin, end;
	begin = clock();

	cornerTest();

	end = clock();
	cout << "Executing time : " << (end - begin) / (double)CLOCKS_PER_SEC << endl;

	return 0;
}

void cornerTest()
{
	string filename = "te.jpg";
	cv::Mat img = imread(filename, 1);
	// ¿¹¿ÜÃ³¸®
	assert(("image not loaded", !img.empty()));

	cout << "Detecting Corners..." << endl;
	vector<pair<int, int> > corners = findCorner(img);

	cout << "Detected Corners are :" << endl;
	for (int i = 0; i < corners.size(); i++) cout << "point" << i + 1 << " : " << "[" << corners[i].first << ", " << corners[i].second << "]" << endl;

	cout << endl;
	cout << "Do you like the corners?" << endl;
	cout << "Press (C) to pick the corners by yourself" << endl;
	cout << "Else, Press any key except (C)" << endl;

	// °ËÃâµÈ ÄÚ³Ê ÀÌ¹ÌÁö ¶ç¿ì±â (ÄÚ³Ê´Â »¡°£»öÀ¸·Î º¸¿©Áü)
	cv::Mat cornerImg = imread(filename, 1);
	for (int k = 0; k < 4; k++) {
		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				cornerImg.at<cv::Vec3b>(corners[k].second + j, corners[k].first + i)[0] = 0;
				cornerImg.at<cv::Vec3b>(corners[k].second + j, corners[k].first + i)[1] = 0;
				cornerImg.at<cv::Vec3b>(corners[k].second + j, corners[k].first + i)[2] = 255;
			}
		}
	}
	namedWindow("Corner Image", WINDOW_NORMAL);
	imshow("Corner Image", cornerImg);

	// (C)¸¦ ´©¸£¸é »ç¿ëÀÚ°¡ »çÁø¿¡ Á¡À» Âï¾î¼­ ÄÚ³Ê¸¦ ÁöÁ¤ÇÒ ¼ö ÀÖÀ½
	// 1 2
	// 3 4
	// ¼øÀ¸·Î ÄÚ³Ê¸¦ Âï¾î¾ß ÇÔ
	if (waitKey(0) == 'c') {
		cout << "Selecting Corners by yourself..." << endl;
		cout << "corner index :" << endl;
		cout << "1  2" << endl;
		cout << "3  4" << endl;

		vector<pair<int, int> >* cornerArr = new vector<pair<int, int> >(0);
		imshow("Corner Image", img);
		cv::setMouseCallback("Corner Image", onMouseEvent, (void*)cornerArr);
		waitKey(0);

		assert(("Less than 4 corners!", cornerArr->size() == 4));
		corners = *cornerArr;
	}
	
	destroyWindow("Corner Image");

	// getMatrixÇÔ¼ö¿¡¼­ ¾µ output ÁÂÇ¥µé ÁöÁ¤
	vector<pair<int, int> > outPoint(4);
	outPoint[0] = make_pair(0, 0);
	outPoint[1] = make_pair(420, 0);
	outPoint[2] = make_pair(0, 580);
	outPoint[3] = make_pair(420, 580);

	cout << endl;
	cout << "Finding Transform Matrix..." << endl;
	vector<vector<double> > transformMat = getMatrix(corners, outPoint);

	cout << "Making Result Image..." << endl;
	cv::Mat resultImg = MakeResult(img, transformMat, 2);

	namedWindow("OpenCV_Test", WINDOW_NORMAL);
	resizeWindow("OpenCV_Test", 420, 580);
	imshow("OpenCV_Test", resultImg);
	waitKey(0);
	cout << "Press any key to continue" << endl;
}

void testGetMatrix()
{
    vector<pair<int,int> > inPoints(4);
    vector<pair<int,int> > outPoints(4);
    
    inPoints[0] = make_pair(3, 4);
    inPoints[1] = make_pair(7, 4);
    inPoints[2] = make_pair(3, 10);
    inPoints[3] = make_pair(10, 12);
    
    outPoints[0] = make_pair(0, 0);
    outPoints[1] = make_pair(3, 0);
    outPoints[2] = make_pair(0, 3);
    outPoints[3] = make_pair(3, 3);
    
    vector<vector<double> > transformMatrix = getMatrix(inPoints, outPoints);
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            cout<<transformMatrix[i][j] << " ";
        }
        cout<<endl;
    }
    
    double inp[3];
    inp[0] = inPoints[0].first;
    inp[1] = inPoints[0].second;
    inp[2] = 1;
    
    double x=0, y=0, c=0;
    for(int i=0; i<3; i++) x += transformMatrix[0][i] * inp[i];
    for(int i=0; i<3; i++) y += transformMatrix[1][i] * inp[i];
    for(int i=0; i<3; i++) c += transformMatrix[2][i] * inp[i];
    
    cout<<x/c<<" "<<y/c<<endl;
}

void testInverse(int size)
{
    vector<vector<double> > tmp(size);
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            tmp[i].push_back(rand()%10);
        }
    }
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            cout<<tmp[i][j]<<" ";
        }
        cout<<endl;
    }
    
    vector<vector<double> > invMatrix;
    invMatrix = inverse(tmp);
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            cout<<invMatrix[i][j]<<" ";
        }
        cout<<endl;
    }
}

void testMakeResult() {
	cv::Mat img = cv::imread("note.jpg");
	cout << img.cols << " " << img.rows << endl;
	//cv::imshow("Window", img);
	//cv::waitKey(0);
	
	vector<pair<int,int> > inPoint(4);
	vector<pair<int,int> > outPoint(4);

	inPoint[0] = make_pair(3, 100);
	inPoint[1] = make_pair(600, 3);
	inPoint[2] = make_pair(300, 890);
	inPoint[3] = make_pair(1000, 550);

	outPoint[0] = make_pair(0, 0);
	outPoint[1] = make_pair(420, 0);
	outPoint[2] = make_pair(0, 580);
	outPoint[3] = make_pair(420, 580);

	vector<vector<double> > transformMat = getMatrix(inPoint, outPoint);
	//cv::Mat resultImg = MakeResult(img, transformMat, 0);
	//cv::Mat resultImg = MakeResult(img, transformMat, 1);
	cv::Mat resultImg = MakeResult(img, transformMat, 2);

	cv::imshow("Window", resultImg);
	cv::waitKey(0);
}

