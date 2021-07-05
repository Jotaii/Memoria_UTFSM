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


#ifndef Elementr_h
#define Elementr_h 1

#include <string>
#include <cctype>
#include <iostream>
#include <list>
#include <math.h>
#include "Point3D.h"
#include "FaceContainer.h"

using Clobscode::Point3D;

using namespace std;

class Element {
    
public:
    
    Element();
    
    Element(vector<unsigned int> &pts);
    
    virtual ~Element();
    
    virtual vector<unsigned int> &getPoints();
    
    virtual double getAspectRatio(vector<Point3D> &points)=0;
    
    virtual double getElementJS(vector<Point3D> &points);

    virtual double getElementJENS(vector<Point3D> &points);
                                
    virtual vector<double> getJS(vector<Point3D> &points);
    
    virtual vector<double> getJENS(vector<Point3D> &points);
    
    virtual string getType();
    
    virtual void setColor(string &c);
    
    virtual string getColor();
    
    virtual void applyColor(FaceContainer &fc);
    
    virtual void computeBbox(vector<Point3D> &coords);
    
    virtual vector<unsigned int> getFacePoints(const unsigned int &i)=0;
    
    virtual unsigned int numberOfFaces()=0;
    
    virtual void addFace(unsigned int idx);
    
    virtual list<unsigned int> &getFaces();
    
    virtual void remove(int axe, double start, double end);
    
    virtual bool wasRemoved();
    
    friend std::ostream& operator<<(std::ostream& o, Element &e);
    
protected:
    
    virtual double getJENSConstant();
    
    virtual vector<unsigned int> getTetra(int index)=0;
    
    virtual vector<vector<unsigned int> > getTetras()=0;
    
    vector<unsigned int> points;
    list<unsigned int> faces;
    double js, jens, jens_constant;
    string color,type;
    bool colored,removed;
    Point3D min, max;
    
};

inline void Element::addFace(unsigned int idx) {
    faces.push_back(idx);
}

inline list<unsigned int> &Element::getFaces() {
    return faces;
}

inline void Element::setColor(string &c) {
    color = c;
    colored = true;
}

inline string Element::getColor() {
    return color;
}

inline bool Element::wasRemoved(){
    return removed;
}


inline double Element::getJENSConstant() {
    return jens_constant;
}

inline vector<unsigned int> &Element::getPoints(){
    return points;
}

inline string Element::getType(){
    return type;
}

#endif

