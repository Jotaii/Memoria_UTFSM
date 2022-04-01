#include "Mesher.h"
#include "TriMesh.h"
#include "FEMesh.h"
#include "Services.h"
#include "RefinementCubeRegion.h"
#include "RefinementSurfaceRegion.h"
#include "RefinementInputSurfaceRegion.h"
#include "RefinementAllRegion.h"
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


using Clobscode::RefinementRegion;
using Clobscode::RefinementCubeRegion;
using Clobscode::RefinementSurfaceRegion;
using Clobscode::Point3D;


/* 
Funcion para filtrar malla de volumen
input: archivo.oct
output: archivo de formato conveniente
funcionamiento:
    1. obtener las caras de la malla de volumen
    2. usar funcion getFaces de Services.h para obtener las caras de la malla
    3. filtrar las caras que solo pertenezcan a un elemento geometrico
    4. retornar los vectores de nodos y los vectores de caras.
*/

/*
Funcion para calcular las normales por nodo:
input: vector de nodos y vector de caras
output: vector con las normales de cada nodo
funcionamiento:
    1. para cada nodo, buscar todas las caras que lo contienen (almacenarlas en un vector dentro de un nuevo objeto creado para la memoria)
    2. por cada nodo, almacenar en un vector las normales preliminares (correspondiente a las normales por cara para cada nodo)
    3. para cada nodo, promediar las normales preliminares (sum(normales)/cantidad_normales)
    4. para cada nodo, normalizar el vector anterior.
    5. regresar el vector con la estructura nueva que tiene la normal de cada nodo
*/

/*
Funcion para proyectar los nuevos puntos
input: vector de estructuras nuevas, vector de puntos, vector de caras
output: vector de nodos, vector de vectores con elementos de volumen
funcionamiento:
    1. para cada normal en la estructura de nodos, agregar a la lista de puntos los nodos nuevos
    2. para cada nodo de ... 
*/

using namespace std;
using std::atoi;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;


// class NodeProjection{
//     public:
//         Point3D NodeSrc;
//         unsigned int Node_index;
//         Point3D Normal;
//         vector< vector<unsigned int> > FacesInvolved;

NodeProjection::NodeProjection(unsigned int Node_index, Point3D Node, vector<Face> &fv){
    // std::vector<vector<unsigned int >>::iterator it=fv.begin();
    // for (it; it != fv.end(); it++){
    //     std::cout << it << endl;
    // }
    this->NodeSrc = Node;
    this->Node_index = Node_index;
    vector <vector <unsigned int>> FacesIndex;
    // FacesIndex.reserve(1000000);
    // std::cout << "["<< this->NodeSrc << "]@"<< this->Node_index <<"\t";
    for(unsigned int i=0; i < fv.size(); i++){
        // std::cout << "Checking face " << i << " with points size array" << fv[i].getPoints().size() << "\n";
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
    for (int i=0; i < this->FacesInvolved.size(); i++){
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
unsigned int NodeProjection::NextNode(vector <unsigned int> FaceNodes){
    for (int i = 0 ; i < FaceNodes.size(); i++){
        // cout << i << " " << this->Node_index << "\n";
        if(FaceNodes[i] == this->Node_index){
            if (i == FaceNodes.size()-1){
                return (FaceNodes[0]);
            }
            return (FaceNodes[i+1]);
        }
    }
    return(NULL);
}

unsigned int NodeProjection::PreviousNode(vector <unsigned int> FaceNodes){
    // cout <<"Central Node index: " << this->Node_index << "\n";
    for (int i = FaceNodes.size()-1 ; i >= 0; i--){
        if(FaceNodes[i] == this->Node_index){
            if (i == 0){
                return (FaceNodes[FaceNodes.size()-1]); //revisar si es FaceNodes.size() -1
            }

            return (FaceNodes[i-1]);
        }
    }
    return(NULL);
}

void NodeProjection::CalcPreNormal(vector <Point3D> Puntos, unsigned int debug){
    Point3D pointTemp;
    if (debug != 0){
            cout << "\n\n[DEBUG] Calculo de las productos vectoriales por cara para el nodo " << this->NodeSrc << " de indice " <<  this->Node_index << "\n";
        }
    for (int i=0; i < this->FacesInvolved.size(); i++){
        if (debug != 0){
            cout << "[DEBUG] Cara " << i << "|"; 
            for(unsigned k=0; k<FacesInvolved[i].size(); k++){
                cout << FacesInvolved[i][k] << " ";
            }
            cout << "|";
        }
        pointTemp = (Puntos[NextNode(this->FacesInvolved[i])]-this->NodeSrc) ^ (Puntos[PreviousNode(this->FacesInvolved[i])]-this->NodeSrc);
        if (debug != 0){
            cout << "(" << Puntos[NextNode(this->FacesInvolved[i])] << ")-(" << this->NodeSrc << ")" << " X " << "(" << Puntos[PreviousNode(this->FacesInvolved[i])] << ")-(" << this->NodeSrc << ") = (" << pointTemp << ") |"; 
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
    double a = this->Normal.Norm();
    this -> Normal = this->Normal/this->Normal.Norm();
    // std::cout << "[DEBUG] Vector unitario: (" << this -> Normal << ") Aplicando la norma: " << a << "\n\n\n";
}

void NodeProjection::print(vector <unsigned int>Fv){
    cout << "[";
    for (int i=0; i < Fv.size(); i++){
        cout << Fv[i] << " ";
    }
    cout << "]";
}

Point3D NodeProjection::getNormal(){
    return this->Normal;
}

vector<vector<unsigned int >> NodeProjection::getFacesInvolved(){
    return this->FacesInvolved;
}
// };