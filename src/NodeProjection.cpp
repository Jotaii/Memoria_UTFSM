
#include "TriMesh.h"
#include "FEMesh.h"
#include "Services.h"
#include "Point3D.h"
#include "OPoint3D.h"
#include "NodeProjection.h"
#include <string>
#include <cctype>
#include <time.h>
#include <chrono>

#include <list>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <iostream>


using Clobscode::Point3D;


using namespace std;
using std::atoi;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;



/*
NodeProjection
Resumen: Clase que se encarga de realizar los calculos asociados a las normales por nodo.
Inputs:
        - Node_index: indice del nodo en revision.
        - Node: Punto a realizar los calculos.
        - fv: vector de caras de la corteza superficial de la malla de entrada.
        - Whitelist_faces: vector que indica si las caras deben ser consideradas o no para los calculos.
*/
NodeProjection::NodeProjection(unsigned int Node_index, Point3D Node, vector<Face> &fv, vector <unsigned int> Whitelist_faces){

    this->NodeSrc = Node;
    this->Node_index = Node_index;
    vector <vector <unsigned int>> FacesIndex;
    
    for(unsigned int i=0; i < fv.size(); i++){
            for(unsigned int j=0; j < fv[i].getPoints().size(); j++){
                if(fv[i].getPoints()[j] == Node_index){ 
                    this->FacesInvolved.push_back(fv[i].getPoints());
                    break;
                }
            }
    }    
}

void NodeProjection::print(){
    cout << "-------------Node data--------------\n";
    cout << "Cords: " << this->NodeSrc << "\n";
    cout << "Index of NodeSrc in vector: " << this->Node_index << "\n";
    cout << "Node Normal: " << this->Normal << "\n";
    cout << "Faces involved: " << "\n";
    for (unsigned int i=0; i < this->FacesInvolved.size(); i++){
        cout << "Face No. " << i << ": {\t";
        for(unsigned int j=0; j < this->FacesInvolved[i].size(); j++){
            cout << this->FacesInvolved[i][j] << "\t";
        }
        cout << "}\n";
    }
    cout << "Qty PreNormals: " << this->FacesInvolved.size() << "\n";
    cout << "-----------End Data------------------\n\n\n";
}

//devuelve el indice del elemento siguiente segun el nodo en cuestion
unsigned int* NodeProjection::NextNode(vector <unsigned int> FaceNodes){
    for (unsigned int i = 0 ; i < FaceNodes.size(); i++){
        if(FaceNodes[i] == this->Node_index){
            if (i == FaceNodes.size()-1){
                return (&FaceNodes[0]);
            }
            return (&FaceNodes[i+1]);
        }
    }
    return(NULL);
}

unsigned int *NodeProjection::PreviousNode(vector <unsigned int> FaceNodes){
    for (int i = FaceNodes.size()-1 ; i >= 0; i--){
        if(FaceNodes[i] == this->Node_index){
            if (i == 0){
                return (&FaceNodes[FaceNodes.size()-1]); 
            }

            return (&FaceNodes[i-1]);
        }
    }
    return(NULL);
}

/*
CalcPreNormal
Resumen: Funcion que se encarga de realizar los calculos de las prenormales por punto-cara
Inputs:
        - Puntos: vector de puntos a los que se les calculara la prenormal
        - debug: entero que permite activar un modo con mayor grado de informacion en consola para fines de desarrollo.
*/
void NodeProjection::CalcPreNormal(vector <Point3D> Puntos, unsigned int debug){
    Point3D pointTemp;
    if (debug != 0){
            cout << "\n\n[DEBUG] Calculo de las productos vectoriales por cara para el nodo " << this->NodeSrc << " de indice " <<  this->Node_index << "\n";
        }
    for (unsigned int i=0; i < this->FacesInvolved.size(); i++){
        if (debug != 0){
            cout << "[DEBUG] Cara " << i << "|"; 
            for(unsigned k=0; k<FacesInvolved[i].size(); k++){
                cout << FacesInvolved[i][k] << " ";
            }
            cout << "|";
        }
        pointTemp = (Puntos[*NextNode(this->FacesInvolved[i])]-this->NodeSrc) ^ (Puntos[*PreviousNode(this->FacesInvolved[i])]-this->NodeSrc);
        if (debug != 0){
            cout << "(" << Puntos[*NextNode(this->FacesInvolved[i])] << ")-(" << this->NodeSrc << ")" << " X " << "(" << Puntos[*PreviousNode(this->FacesInvolved[i])] << ")-(" << this->NodeSrc << ") = (" << pointTemp << ") |"; 
        }
        pointTemp = pointTemp.normalize();
        if (debug != 0){
            cout << " Normalizado: (" << pointTemp << ")\n"; 
        }
        this->Normal = this->Normal + pointTemp;
        if (debug != 0){
            cout << "[DEBUG] Suma Acumulada de normales para el nodo:  " << this->Normal << "\n"; 
        }
    }
}

void NodeProjection::MeanNormal(){
    this->Normal = this->Normal/this->FacesInvolved.size();
}

void NodeProjection::Normalize(){
    this -> Normal = this->Normal/this->Normal.Norm();
}

void NodeProjection::print(vector <unsigned int>Fv){
    cout << "[";
    for (unsigned int i=0; i < Fv.size(); i++){
        cout << Fv[i] << " ";
    }
    cout << "]";
}

Point3D NodeProjection::getNormal(){
    return this->Normal;
}

void NodeProjection::setNormal(float X, float Y, float Z){
    Point3D P(X,Y,Z);
    this->Normal = P;
}

vector<vector<unsigned int >> NodeProjection::getFacesInvolved(){
    return this->FacesInvolved;
}

unsigned int NodeProjection::getNodeIndex(){
    return this->Node_index;
}
// };