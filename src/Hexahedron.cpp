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

#include "Hexahedron.h"

Hexahedron::Hexahedron(vector<unsigned int> &pts):Element(pts) {
    type = "H";
    jens_constant = 1;
}

Hexahedron::~Hexahedron(){
    
}

double Hexahedron::getAspectRatio(vector<Point3D> &points) {
    return 0;
}

vector<vector<unsigned int> > Hexahedron::getTetras() {
    
    vector<vector<unsigned int> > tets;
    tets.reserve(8);
    
    vector<unsigned int> t0(4,0);
    t0[0]=points[0];
    t0[1]=points[1];
    t0[2]=points[3];
    t0[3]=points[4];
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
    t2[3]=points[6];
    tets.push_back(t2);
    
    vector<unsigned int> t3(4,0);
    t3[0]=points[3];
    t3[1]=points[0];
    t3[2]=points[2];
    t3[3]=points[7];
    tets.push_back(t3);
    
    vector<unsigned int> t4(4,0);
    t4[0]=points[4];
    t4[1]=points[7];
    t4[2]=points[5];
    t4[3]=points[0];
    tets.push_back(t4);
    
    vector<unsigned int> t5(4,0);
    t5[0]=points[5];
    t5[1]=points[4];
    t5[2]=points[6];
    t5[3]=points[1];
    tets.push_back(t5);
    
    vector<unsigned int> t6(4,0);
    t6[0]=points[6];
    t6[1]=points[5];
    t6[2]=points[7];
    t6[3]=points[2];
    tets.push_back(t6);
    
    vector<unsigned int> t7(4,0);
    t7[0]=points[7];
    t7[1]=points[6];
    t7[2]=points[4];
    t7[3]=points[3];
    tets.push_back(t7);
    
    return tets;
}

vector<unsigned int> Hexahedron::getTetra(int index){
    
    vector<unsigned int> t(4,0);
    
    if(index == 0){
        t[0]=points[0];
        t[1]=points[1];
        t[2]=points[3];
        t[3]=points[4];
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
        t[3]=points[6];
    }
    else if(index==3){
        t[0]=points[3];
        t[1]=points[0];
        t[2]=points[2];
        t[3]=points[7];
    }
    else if(index==4){
        t[0]=points[4];
        t[1]=points[7];
        t[2]=points[5];
        t[3]=points[0];
    }
    else if(index==5){
        t[0]=points[5];
        t[1]=points[4];
        t[2]=points[6];
        t[3]=points[1];
    }
    else if(index==6){
        t[0]=points[6];
        t[1]=points[5];
        t[2]=points[7];
        t[3]=points[2];
    }
    else if(index==7){
        t[0]=points[7];
        t[1]=points[6];
        t[2]=points[4];
        t[3]=points[3];
    }
    
    return t;
}

vector<unsigned int> Hexahedron::getFacePoints(const unsigned int &index){
    vector<unsigned int> aux;
    aux.reserve(4);
    switch(index){
        case 0:
            aux.push_back(points[0]);
            aux.push_back(points[3]);
            aux.push_back(points[2]);
            aux.push_back(points[1]);
            break;
        case 1:
            aux.push_back(points[0]);
            aux.push_back(points[1]);
            aux.push_back(points[5]);
            aux.push_back(points[4]);
            break;
        case 2:
            aux.push_back(points[1]);
            aux.push_back(points[2]);
            aux.push_back(points[6]);
            aux.push_back(points[5]);
            break;
        case 3:
            aux.push_back(points[2]);
            aux.push_back(points[3]);
            aux.push_back(points[7]);
            aux.push_back(points[6]);
            break;
        case 4:
            aux.push_back(points[3]);
            aux.push_back(points[0]);
            aux.push_back(points[4]);
            aux.push_back(points[7]);
            break;
        case 5:
            aux.push_back(points[4]);
            aux.push_back(points[5]);
            aux.push_back(points[6]);
            aux.push_back(points[7]);
            break;
        default:
            break;
    }
    return aux;
}

