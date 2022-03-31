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
#include "NormalRepair.h"
#include "AdvancingPoint.h"
#include "NewPointRef.h"
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
using Clobscode::TriMesh;



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



AdvancingPoint::AdvancingPoint(vector<Clobscode::Point3D> &Puntos, vector<vector<unsigned int>> &VUI, float dist){
    // POR VERIFICAR ESTO
    vector <Face> FVector;
    for (int i=0;i<VUI.size(); i++){
        Face Ftemp(VUI[i]);
        FVector.push_back(Ftemp);
    }

    std::cout << "Numero de Nodos: " << Puntos.size() << "\n";
    std::cout << "Numero de caras: " << FVector.size() << "\n";

    vector <unsigned int> nodesInSurface;
    for (int i=0; i< Puntos.size();i++){
        nodesInSurface.push_back(0);
    }

    for (unsigned int fidx=0; fidx< FVector.size(); fidx++){        
        for (unsigned int nidx=0; nidx < nodesInSurface.size(); nidx++){
            if(FVector[fidx].hasPoint(nidx) && nodesInSurface[nidx]==0){
                nodesInSurface.at(nidx) = 1;
            }
        }
    }
    
    NormalRepair NR = NormalRepair(Puntos, FVector);
    
    // print(NR.getFaces());
    vector <Face> NRF = NR.getFaces();
    
    vector <NodeProjection> NodeProjectionVector;
    NodeProjectionVector.reserve(Puntos.size());
    int deb = 0;

    vector <NewPointRef> NPR_arr;
    
    // Funcion que inicializa un objeto del tipo NodeProjection
    for (unsigned int i=0; i<Puntos.size(); i++){
        // std::cout << "Control\t" << Puntos.size() << "\t" << nodesInSurface[i] << " \n";
        // std::cout << "Control\t" << i << "\n";
        if (deb==0){
            
            // NodeProjection(unsigned int Node_index, Point3D Node, vector<Face> &fv);
            if (nodesInSurface[i] == 1){
                NodeProjection NP(i, Puntos[i], NRF);
                
                if (NP.getFacesInvolved().size() > 0){
                    
                    // funcion que calcula la normal acumulada de las caras que involucran al nodo NP
                    NP.CalcPreNormal(Puntos, 0);

                    // Funcion que normaliza el valor en la normal acumulada de las caras que involucran al nodo NP
                    NP.Normalize();
                    //TODO: REVISAR por que algunas normales dan -nan, deberian solo dar valores numericos!!!!!!!!!!!!
                    NodeProjectionVector.push_back(NP);

                    if(isnan(NP.getNormal().X()) || isnan(NP.getNormal().Y()) || isnan(NP.getNormal().Z())){
                        // cout << "Control entrada if\n";
                        NP.CalcPreNormal(Puntos,1);
                        
                    }
                    // NP.print();
                    
                }
            }
            
            
        }
        else{
            std::cout << "index punto :" << i << "\n";
            // NodeProjection(unsigned int Node_index, Point3D Node, vector<Face> &fv);
            std::cout << "Inicializacion ...";
            NodeProjection NP(i, Puntos[i], NRF);
            std::cout << "OK\n";
            if (NP.getFacesInvolved().size() > 0){
                // funcion que calcula la normal acumulada de las caras que involucran al nodo NP
                std::cout << "Calculo de Prenormales...";
                NP.CalcPreNormal(Puntos);
                std::cout << "OK\n";
                // Funcion que normaliza el valor en la normal acumulada de las caras que involucran al nodo NP
                std::cout << "Normalizando Prenormal...";
                NP.Normalize();
                std::cout << "OK\n\n";
                // Funcion de utilidad que imprime toda la informacion en el objeto NP
                // NP.print();
                NodeProjectionVector.push_back(NP);
            }
        }
        
    }

    for (unsigned int pointIdx = 0; pointIdx < Puntos.size(); pointIdx++){
        if (nodesInSurface[pointIdx] == 1){
            Point3D newPoint(Puntos[pointIdx].X()+dist*NodeProjectionVector[pointIdx].getNormal().X(),
                         Puntos[pointIdx].Y()+dist*NodeProjectionVector[pointIdx].getNormal().Y(),
                         Puntos[pointIdx].Z()+dist*NodeProjectionVector[pointIdx].getNormal().Z());
            this -> new_points.push_back(newPoint); //posibilidad de tener que borrar este arreglo para dejarle la carga a la estructura NewPointRef
            NewPointRef NewNode(Puntos.size()+pointIdx, pointIdx, newPoint);
            NPR_arr.push_back(NewNode);
        }
    }
    this -> normals = NodeProjectionVector;
    this -> arr_points = Puntos;
    this -> arr_faces = VUI;
    
    // std::cout << Puntos.size() << "\t" << NPR_arr.size() << "\n";


    //Falta:
    //      1. agregar los puntos new_points al arreglo de puntos -- LISTO
    //      2. agregar los puntos (por indice) a la cara correspondiente del VUI
    //      3. crear la estructura para saber si un punto fue previamente expandido, para no expandirlo de nuevo.


    for (unsigned int face_idx=0; face_idx < this->arr_faces.size(); face_idx++){
        unsigned int n_nodes = this->arr_faces[face_idx].size();
        for(unsigned int node_face_rel=0; node_face_rel < n_nodes; node_face_rel++){
            //esto es SUPER ineficiente, corregir para disminuir el computo
            for(unsigned int i=0; i< NPR_arr.size();i++){
                if (NPR_arr[i].getFIndex() == arr_faces[face_idx][node_face_rel]){
                    this->arr_faces[face_idx].push_back(NPR_arr[i].getIndex());
                }
            }
        }
    }

    // for (unsigned int i=0; i< this->arr_faces.size(); i++){
    //     for (unsigned int j=0; j< this->arr_faces[i].size(); j++){
    //         std::cout << arr_faces[i][j] << "\t";
    //     }
    //     std::cout << "\n";
    // }

    // for (unsigned int i=0; i< this->arr_faces.size(); i++){
    //     for (unsigned int j=0; j< this->arr_faces[i].size(); j++){
    //         std::cout << arr_faces[i][j] << "\t";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\n";

}

vector <vector<unsigned int>> AdvancingPoint::getFaces(){ return this->arr_faces;}
vector <Point3D> AdvancingPoint::getPoints(){return this->arr_points;}
vector <Point3D> AdvancingPoint::getNewPoints(){return this->new_points;}
vector <NodeProjection> AdvancingPoint::getNormals(){return this->normals;}