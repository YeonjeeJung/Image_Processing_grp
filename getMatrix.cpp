//
//  getMatrix.cpp
//  Image_Processing
//
//  Created by Yeonjee Jung on 24/11/2018.
//  Copyright 2018 Yeonjee Jung€. All rights reserved.
//

#include "getMatrix.hpp"

double determinant(vector<vector<double> > mat);
double cofactor(vector<vector<double> > mat, int x, int y);
vector<vector<double> > transpose(vector<vector<double> > mat);
vector<vector<double> > adjointMatrix(vector<vector<double> > mat);
vector<vector<double> > inverse(vector<vector<double> > mat);
vector<double> solve(vector<vector<double> > A, vector<double> b);
vector<vector<double> > getMatrix(vector<pair<int,int> > oriPointVec, vector<pair<int,int> > newPointVec);

double determinant(vector<vector<double> > mat)
{
    double det = 0;
    int size = mat.size();

	// mat´Â Ç×»ó Á¤»ç°¢Çà·ÄÀÌ¾î¾ß ÇÔ
	for (int i = 0; i < size; i++) assert(("Matrix must be square", mat[i].size() == size));
    
    if(size == 2) det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
    else{
        for(int i=0; i<size; i++){
            det += mat[i][0]*cofactor(mat, i, 0);
        }
    }
    return det;
}

// Çà·Ä mat¿¡¼­ (i, j)ÀÇcofactor ±¸ÇÔ
double cofactor(vector<vector<double> > mat, int x, int y)
{   
	int size = mat.size();

	// mat´Â Ç×»ó Á¤»ç°¢Çà·ÄÀÌ¾î¾ß ÇÔ 
	for (int i = 0; i < size; i++) assert(("Matrix must be square", mat[i].size() == size));

    if(size == 1) return mat[0][0];
    
    vector<vector<double> > retMat(size-1);
    
    int cnt = 0;
    for(int i=0; i<size; i++){
        if(i == x) continue;
        
        for(int j=0; j<size; j++){
            if(j == y) continue;
            retMat[cnt].push_back(mat[i][j]);
        }
        cnt++;
    }
    
    if((x+y) % 2 == 0) return determinant(retMat);
    else return -determinant(retMat);
}

vector<vector<double> > transpose(vector<vector<double> > mat)
{
	int size = mat.size();

	// Á¤»ç°¢Çà·Ä °¡Á¤
	for (int i = 0; i < size; i++) assert(("Matrix must be square", mat[i].size() == size));

	// matrix ÃÊ±âÈ­
    vector<vector<double> > transMatrix(size);
    for(int i=0; i<size; i++){
        transMatrix[i].resize(size);
    }
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            transMatrix[j][i] = mat[i][j];
        }
    }
    
    return transMatrix;
}

// adjoint matrix:
// | c_11 c_12 c_13 ... |
// | c_21 c_22 c_23 ... |
// |...                 |
// ë¥?transpose???‰ë ¬
vector<vector<double> > adjointMatrix(vector<vector<double> > mat)
{  
	int size = mat.size();

	// Á¤»ç°¢Çà·Ä °¡Á¤
	for (int i = 0; i < size; i++) assert(("Matrix must be square", mat[i].size() == size));

	// matrix ÃÊ±âÈ­
    vector<vector<double> > cofMatrix(size);
    for(int i=0; i<size; i++){
        cofMatrix[i].resize(size);
    }
    
    vector<vector<double> > adjMatrix(size);
    for(int i=0; i<size; i++){
        adjMatrix[i].resize(size);
    }
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            cofMatrix[i][j] = cofactor(mat, i, j);
        }
    }
    adjMatrix = transpose(cofMatrix);
    
    return adjMatrix;
}

// A^-1 = 1/det * adjointMatrix
vector<vector<double> > inverse(vector<vector<double> > mat)
{
	int size = mat.size();

	// Á¤»ç°¢Çà·Ä °¡Á¤
	for (int i = 0; i < size; i++) assert(("Matrix must be square", mat[i].size() == size));
	
	// matrix ÃÊ±âÈ­
	vector<vector<double> > invMatrix(size);
    for(int i=0; i<size; i++){
        invMatrix[i].resize(size);
    }
    
    vector<vector<double> > adjMatrix;
    
    double det = determinant(mat);
    adjMatrix = adjointMatrix(mat);
    
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            invMatrix[i][j] = adjMatrix[i][j] / det;
        }
    }
    
    return invMatrix;
}

// Ax = b Ç®±â
// x = A^-1*b ·Î Ç® ¼ö ÀÖÀ½
vector<double> solve(vector<vector<double> > A, vector<double> b)
{
	int size = A.size();

	// Á¤»ç°¢Çà·Ä °¡Á¤
	for (int i = 0; i < size; i++) assert(("Matrix must be square", A[i].size() == size));
	
	vector<double> ret(size);
    vector<vector<double> > invMatrix;
    
    // ¿ªÇà·ÄÀ» ±¸ÇÔ
    invMatrix = inverse(A);
    
    // ¿ªÇà·ÄÀ» b¿Í °öÇÔ
    for(int i=0; i<size; i++){
        ret[i] = 0;
        for(int j=0; j<size; j++) ret[i] += invMatrix[i][j] * b[j];
    }
    
    return ret;
}

// ¸®ÅÏ°ªÀº 3x3 Çà·Ä
vector<vector<double> > getMatrix(vector<pair<int,int> > oriPointVec, vector<pair<int,int> > newPointVec)
{
    // ¿¹¿ÜÃ³¸®
	assert(("There must be 4 points", oriPointVec.size() == 4));
	assert(("There must be 4 points", newPointVec.size() == 4));
    
    // 8x8 ÀúÀåÇÏ´Â º¯¼ö
	vector<vector<double> > oriMatrix(8);
    vector<double> newxy(8);
    
    // A ¹è¿­ ¸¸µé±â
	for(int i=0; i<4; i++){
        oriMatrix[i] = vector<double>(8);
        oriMatrix[i+4] = vector<double>(8);
        
        oriMatrix[i][0] = oriMatrix[i+4][3] = (double)oriPointVec[i].first;
        oriMatrix[i][1] = oriMatrix[i+4][4] = (double)oriPointVec[i].second;
        oriMatrix[i][2] = oriMatrix[i+4][5] = 1.0;
        oriMatrix[i][3] = oriMatrix[i][4] = oriMatrix[i][5] = oriMatrix[i+4][0] = oriMatrix[i+4][1] = oriMatrix[i+4][2] = 0.0;
        oriMatrix[i][6] = -((double)oriPointVec[i].first * newPointVec[i].first);
        oriMatrix[i][7] = -((double)oriPointVec[i].second * newPointVec[i].first);
        oriMatrix[i+4][6] = -((double)oriPointVec[i].first * newPointVec[i].second);
        oriMatrix[i+4][7] = -((double)oriPointVec[i].second * newPointVec[i].second);
    }
    
    // b ¹è¿­ ¸¸µé±â
	for(int i=0; i<4; i++){
        newxy[i] = newPointVec[i].first;
        newxy[i+4] = newPointVec[i].second;
    }
    
    // Ax = b Ç®±â
	vector<double> x = solve(oriMatrix, newxy);
    
    // 1Â÷¿ø ¹è¿­À» 3x3 transform matrix·Î ¹Ù²ãÁÖ±â
    vector<double> tmp;
    for(int i=0; i<8; i++) tmp.push_back(x[i]);
    tmp.push_back(1.0);
    
    vector<vector<double> > ret(3);
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            ret[i].push_back(tmp[i*3+j]);
        }
    }
    
    return ret;
}