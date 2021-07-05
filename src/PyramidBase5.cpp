/**************************************************************************/
/*                                                                        */
/*                  Jens                                                  */
/*                                                                        */
/**************************************************************************/
/* Allows to display Jacobian Information of a mesh in .m3d format        */
/* Written by Claudio Lobos (clobos@inf.utfsm.cl) 2015					  */
/* UNIVERSIDAD TECNICA FEDERICO SANTA MARIA								  */
/**************************************************************************/

/*
 <JENS: this program computes the Element Normalized Scaled Jacobian (Jens)>
 
 Copyright (C) <2015>  <Claudio Lobos>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/gpl.txt>
 */

#include "PyramidBase5.h"

PyramidBase5::PyramidBase5(vector<unsigned int> &pts):Element(pts){
    type="P6";
    jens_constant = 0; //update value when found
}

PyramidBase5::~PyramidBase5(){
    
}

double PyramidBase5::getAspectRatio(vector<Point3D> &points) {
    return 0;
}

double PyramidBase5::getElementJS(vector<Point3D> &pts){
    
    double worst = 2;
    vector<double> auxjs = getJS(pts);
    
    for (unsigned int i=0; i<auxjs.size(); i++) {
        if (worst>auxjs[i]) {
            worst = auxjs[i];
        }
    }
    return worst;
    
}

double PyramidBase5::getElementJENS(vector<Point3D> &pts){
    
    double worst = 2;
    vector<double> auxjens = getJENS(pts);
    
    for (unsigned int i=0; i<auxjens.size(); i++) {
        if (worst>auxjens[i]) {
            worst = auxjens[i];
        }
    }
    return worst;
}

vector<double> PyramidBase5::getJS(vector<Point3D> &pts){
    
    vector<double> auxjs,js(6,0);
    
    auxjs = Element::getJS(pts);
    
    double minpos=2, maxneg=-2, apice =0;
    
    bool positive= false;
    
    for (unsigned int i=5; i<auxjs.size(); i++) {
        if (auxjs[i]>=0 && auxjs[i]<minpos) {
            minpos = auxjs[i];
            positive = true;
        }
        if (auxjs[i]<0 && auxjs[i]>maxneg) {
            maxneg = auxjs[i];
        }
    }
    
    if (positive) {
        js[5] = minpos;
    }
    else {
        js[5] = maxneg;
    }
    
    for (unsigned int i=0; i<5; i++) {
        js[i] = auxjs[i];
    }
    
    return js;
}

vector<double> PyramidBase5::getJENS(vector<Point3D> &pts){
    
    vector<double> auxjens,jens(6,0);
    
    auxjens = Element::getJENS(pts);
    
    double minpos=2, maxneg=-2, apice =0;
    
    bool positive= false;
    
    for (unsigned int i=0; i<5; i++) {
        jens[i] = auxjens[i];
    }
    
    for (unsigned int i=5; i<auxjens.size(); i++) {
        if (auxjens[i]>=0 && auxjens[i]<minpos) {
            minpos = auxjens[i];
            positive = true;
        }
        if (auxjens[i]<0 && auxjens[i]>maxneg) {
            maxneg = auxjens[i];
        }
    }
    
    if (positive) {
        jens[5] = minpos;
    }
    else {
        jens[5] = maxneg;
    }
    
    return jens;
}

vector<vector<unsigned int> > PyramidBase5::getTetras() {
    
    vector<vector<unsigned int> > tets;
    tets.reserve(10);
    
    vector<unsigned int> t0(4,0);
    t0[0]=points[0];
    t0[1]=points[1];
    t0[2]=points[4];
    t0[3]=points[5];
    tets.push_back(t0);
    
    vector<unsigned int> t1(4,0);
    t1[0]=points[1];
    t1[1]=points[2];
    t1[2]=points[0];
    t1[3]=points[5];
    tets.push_back(t1);
    
    vector<unsigned int> t2(4,0);
    t2[0]=points[2];
    t2[1]=points[3];
    t2[2]=points[1];
    t2[3]=points[5];
    tets.push_back(t2);
    
    vector<unsigned int> t3(4,0);
    t3[0]=points[3];
    t3[1]=points[4];
    t3[2]=points[2];
    t3[3]=points[5];
    tets.push_back(t3);
    
    vector<unsigned int> t4(4,0);
    t3[0]=points[4];
    t3[1]=points[0];
    t3[2]=points[3];
    t3[3]=points[5];
    tets.push_back(t4);

    
    
    vector<unsigned int> t5(4,0);
    t4[0]=points[5];
    t4[1]=points[0];
    t4[2]=points[4];
    t4[3]=points[1];
    tets.push_back(t5);
    
    vector<unsigned int> t6(4,0);
    t5[0]=points[5];
    t5[1]=points[1];
    t5[2]=points[0];
    t5[3]=points[2];
    tets.push_back(t6);
    
    vector<unsigned int> t7(4,0);
    t6[0]=points[5];
    t6[1]=points[2];
    t6[2]=points[1];
    t6[3]=points[3];
    tets.push_back(t7);
    
    vector<unsigned int> t8(4,0);
    t7[0]=points[5];
    t7[1]=points[3];
    t7[2]=points[2];
    t7[3]=points[4];
    tets.push_back(t8);
    
    vector<unsigned int> t9(4,0);
    t7[0]=points[5];
    t7[1]=points[4];
    t7[2]=points[3];
    t7[3]=points[0];
    tets.push_back(t9);
    
    return tets;
}

vector<unsigned int> PyramidBase5::getTetra(int index){
    
    vector<unsigned int> t(4,0);
    if(index==0){
        t[0]=points[0];
        t[1]=points[1];
        t[2]=points[4];
        t[3]=points[5];
    }
    else if(index==1){
        t[0]=points[1];
        t[1]=points[2];
        t[2]=points[0];
        t[3]=points[5];
    }
    else if(index==2){
        t[0]=points[2];
        t[1]=points[3];
        t[2]=points[1];
        t[3]=points[5];
    }
    else if(index==3){
        t[0]=points[3];
        t[1]=points[4];
        t[2]=points[2];
        t[3]=points[5];
    }
    else if(index==4){
        t[0]=points[4];
        t[1]=points[0];
        t[2]=points[3];
        t[3]=points[5];
    }
    else if(index==5){
        t[0]=points[5];
        t[1]=points[0];
        t[2]=points[4];
        t[3]=points[1];
    }
    else if(index==6){
        t[0]=points[5];
        t[1]=points[1];
        t[2]=points[0];
        t[3]=points[2];
    }
    else if(index==7){
        t[0]=points[5];
        t[1]=points[2];
        t[2]=points[1];
        t[3]=points[3];
    }
    else if(index==8){
        t[0]=points[5];
        t[1]=points[3];
        t[2]=points[2];
        t[3]=points[4];
    }
    else if(index==9){
        t[0]=points[5];
        t[1]=points[4];
        t[2]=points[3];
        t[3]=points[0];
    }
    return t;
}

vector<unsigned int> PyramidBase5::getFacePoints(const unsigned int &index){
    
    vector<unsigned int> aux;;
    
    switch(index){
        case 0:
            aux.reserve(5);
            aux.push_back(points[0]);
            aux.push_back(points[4]);
            aux.push_back(points[3]);
            aux.push_back(points[2]);
            aux.push_back(points[1]);
            break;
        case 1:
            aux.reserve(3);
            aux.push_back(points[0]);
            aux.push_back(points[1]);
            aux.push_back(points[5]);
            break;
        case 2:
            aux.reserve(3);
            aux.push_back(points[1]);
            aux.push_back(points[2]);
            aux.push_back(points[5]);
            break;
        case 3:
            aux.reserve(3);
            aux.push_back(points[2]);
            aux.push_back(points[3]);
            aux.push_back(points[5]);
            break;
        case 4:
            aux.reserve(3);
            aux.push_back(points[3]);
            aux.push_back(points[4]);
            aux.push_back(points[5]);
            break;
        case 5:
            aux.reserve(3);
            aux.push_back(points[4]);
            aux.push_back(points[0]);
            aux.push_back(points[5]);
            break;
        default:
            break;
    }
    return aux;
}
