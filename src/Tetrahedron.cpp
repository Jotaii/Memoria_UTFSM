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

#include "Tetrahedron.h"

Tetrahedron::Tetrahedron(vector<unsigned int> &pts):Element(pts) {
    type="T";
    jens_constant = 1/sqrt(2);
}

Tetrahedron::~Tetrahedron(){
    
}

void Tetrahedron::getPointsIndexInEdge(int index, int &pindex1, int &pindex2){
    
    if(index<0 || index>5)
        return;
    switch(index){
        case 0:
            pindex1=points[0];
            pindex2=points[1];
            break;
        case 1:
            pindex1=points[2];
            pindex2=points[0];
            break;
        case 2:
            pindex1=points[3];
            pindex2=points[0];
            break;
        case 3:
            pindex1=points[1];
            pindex2=points[2];
            break;
        case 4:
            pindex1=points[3];
            pindex2=points[1];
            break;
        case 5:
            pindex1=points[3];
            pindex2=points[2];
            break;
        default:
            return;
    }
}

double Tetrahedron::getAspectRatio(vector<Point3D> &points) {

    int eip1,eip2;
    Point3D p1,p2,p;
    double aspectratio = 0;
    
    for(int i=0;i<6;i++){
        getPointsIndexInEdge(i,eip1,eip2);
        p1 = points[eip1];
        p2 = points[eip2];
        p = p1-p2;
        aspectratio += p.dot(p);
    }
    
    aspectratio /= 6;
    aspectratio = pow(aspectratio,1.5);
    aspectratio /= 8.48528*getVolume(points);
    return 1/aspectratio;
    
    /*
     int eip1,eip2;
     Point p1,p2;
     double aspectratio = 0;
     
     for(int i=0;i<6;i++){
     getPointsIndexInEdge(i,eip1,eip2);
     p1 = points[eip1].getPoint();
     p2 = points[eip2].getPoint();
     aspectratio += pow(p1.distance(p2),2);
     }
     
     aspectratio /= 6;
     aspectratio = pow(aspectratio,1.5);
     aspectratio /= 8.48528*getVolume(points);
     return 1/aspectratio;
     */
}

double Tetrahedron::getVolume(vector<Point3D> &pts){
    Point3D v1,v2,v3,v4,dif1,dif2,dif3,crossr;
    double vol;
    
    v1=pts[points[0]];
    v2=pts[points[1]];
    v3=pts[points[2]];
    v4=pts[points[3]];
    dif1=v2-v1;
    dif2=v3-v1;
    dif3=v4-v1;
    crossr=dif1.cross(dif2);
    vol=(dif3.dot(crossr));
    return vol/6;
}

vector<vector<unsigned int> > Tetrahedron::getTetras() {
    
    /*note: due to vector noramlization, the value of JENS
     is not the same for every node, therfore we need to compute
     the quality for each node. This is a different behavior from
     Jacobian computation*/
    
    vector<vector<unsigned int> > tetras;
    tetras.reserve(4);
    
    vector<unsigned int> t0(4,0);
    t0[0]= points[0];
    t0[1]= points[1];
    t0[2]= points[2];
    t0[3]= points[3];
    tetras.push_back(t0);
    
    vector<unsigned int> t1(4,0);
    t1[0]= points[1];
    t1[1]= points[0];
    t1[2]= points[3];
    t1[3]= points[2];
    tetras.push_back(t1);
    
    vector<unsigned int> t2(4,0);
    t2[0]= points[2];
    t2[1]= points[0];
    t2[2]= points[1];
    t2[3]= points[3];
    tetras.push_back(t2);
    
    vector<unsigned int> t3(4,0);
    t3[0]= points[3];
    t3[1]= points[0];
    t3[2]= points[2];
    t3[3]= points[1];
    tetras.push_back(t3);
    
    return tetras;
}

vector<unsigned int> Tetrahedron::getFacePoints(const unsigned int &index){
    
    vector<unsigned int> aux;
    aux.reserve(3);
    
    switch(index){
        case 0:
            aux.push_back(points[0]);
            aux.push_back(points[2]);
            aux.push_back(points[1]);
            break;
        case 1:
            aux.push_back(points[0]);
            aux.push_back(points[1]);
            aux.push_back(points[3]);
            break;
        case 2:
            aux.push_back(points[1]);
            aux.push_back(points[2]);
            aux.push_back(points[3]);
            break;
        case 3:
            aux.push_back(points[2]);
            aux.push_back(points[0]);
            aux.push_back(points[3]);
            break;
        default:
            break;
    }
    return aux;
}

/*double Tetrahedron::getDet(vector<Point3D> &pts, int index){
    vector<int> pos = element->getPoints();
    Point n  = points[pos[0]].getPoint(),
    n1 = points[pos[1]].getPoint(),
    n2 = points[pos[2]].getPoint(),
    n3 = points[pos[3]].getPoint();

    double dx1 = n1[0]-n[0], dy1=n1[1]-n[1], dz1 = n1[2]-n[2];
    double dx2 = n2[0]-n[0], dy2=n2[1]-n[1], dz2 = n2[2]-n[2];
    double dx3 = n3[0]-n[0], dy3=n3[1]-n[1], dz3 = n3[2]-n[2];
    
    double s1 = dx1 * (dy2*dz3 - dy3*dz2);
    double s2 = dx2 * (dy1*dz3 - dy3*dz1);
    double s3 = dx3 * (dy1*dz2 - dy2*dz1);
    
    double normalize = 1/(s1-s2+s3);
    return normalize;
}*/
