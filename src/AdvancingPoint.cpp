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



AdvancingPoint::AdvancingPoint(vector<Clobscode::Point3D> &Puntos, vector<vector<unsigned int>> &VUI, float dist, unsigned int num_layers){
    vector <Face> FVector;
    for (int i=0;i<VUI.size(); i++){
        Face Ftemp(VUI[i]);
        FVector.push_back(Ftemp);
    }
    
    // std::cout << "Numero de Nodos: " << Puntos.size() << "\n";
    // std::cout << "Numero de caras: " << FVector.size() << "\n";

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
    

    vector <Face> NRF = NR.getFaces();


    vector <NodeProjection> NodeProjectionVector;
    NodeProjectionVector.reserve(Puntos.size());
    int deb = 0;

    vector <NewPointRef> NPR_arr;
    
    // Funcion que inicializa un objeto del tipo NodeProjection
    for (unsigned int i=0; i<Puntos.size(); i++){
        if (deb==0){
            
            
            if (nodesInSurface[i] == 1){
                NodeProjection NP(i, Puntos[i], NRF);
                
                if (NP.getFacesInvolved().size() > 0){
                    
                    
                    NP.CalcPreNormal(Puntos, 0);

                    
                    NP.Normalize();
                    
                    NodeProjectionVector.push_back(NP);

                    if(isnan(NP.getNormal().X()) || isnan(NP.getNormal().Y()) || isnan(NP.getNormal().Z())){
                        
                        NP.CalcPreNormal(Puntos,1);
                        
                    }
                    
                    
                }
            }
            
            
        }
        else{
            std::cout << "index punto :" << i << "\n";
            
            std::cout << "Inicializacion ...";
            NodeProjection NP(i, Puntos[i], NRF);
            std::cout << "OK\n";
            if (NP.getFacesInvolved().size() > 0){
                // funcion que calcula la normal acumulada de las caras que involucran al nodo NP
                std::cout << "Calculo de Prenormales...";
                NP.CalcPreNormal(Puntos, 1U);
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

    //generacion de puntos nuevos
    for (unsigned int layer=1; layer <= num_layers; layer++){
        for (unsigned int pointIdx = 0; pointIdx < Puntos.size(); pointIdx++){
            if (nodesInSurface[pointIdx] == 1){
                Point3D newPoint(Puntos[pointIdx].X()+layer*dist*NodeProjectionVector[pointIdx].getNormal().X(),   //si no funciona sacar el layer*
                            Puntos[pointIdx].Y()+layer*dist*NodeProjectionVector[pointIdx].getNormal().Y(),        //si no funciona sacar el layer*
                            Puntos[pointIdx].Z()+layer*dist*NodeProjectionVector[pointIdx].getNormal().Z());       //si no funciona sacar el layer*
                this -> new_points.push_back(newPoint); //posibilidad de tener que borrar este arreglo para dejarle la carga a la estructura NewPointRef
                if (layer == 1){
                    NewPointRef NewNode(Puntos.size()*layer+pointIdx, pointIdx, newPoint);
                    NPR_arr.push_back(NewNode);
                }
                else {
                    NewPointRef NewNode(Puntos.size()*layer+pointIdx, Puntos.size()*(layer-1)+pointIdx, newPoint);
                    NPR_arr.push_back(NewNode);
                }

                
            }
        }
    }
    // cout << NPR_arr.size() << "\n";
    this -> normals = NodeProjectionVector;
    this -> arr_points = Puntos;
    this -> arr_faces = VUI;

    // cout << "--OLD--\n";
    // for(int i=0; i< this->arr_faces.size(); i++){
    //     cout << "[";
    //     for(int j=0; j < this->arr_faces[i].size(); j++){
    //         cout << this->arr_faces[i][j] << " ";
    //     }
    //     cout << "]\n";
    // }
    // ------------------------ EXPERIMENTAL ----------------------------------
    //si layer pendientes > 0, crear nuevo elemento en arr_faces 
    //con los puntos iniciales correspondiente a la cara anterior
    
    // for (unsigned int checkedLayers=1; checkedLayers<=num_layers; checkedLayers++){
    //     vector <unsigned int> newElemsByLayer;
    //     cout << "newlayer \n";
    //     for (unsigned int i=NPR_arr.size()*(checkedLayers-1)/num_layers;
    //             i < NPR_arr.size()*checkedLayers/num_layers;i++){
    //         for (unsigned int k=0; k<NPR_arr.size(); k++){
    //             if(NPR_arr[k].getIndex()== i+Puntos.size()){
    //                 cout << i+Puntos.size() << "->" << NPR_arr[k].getFIndex() << "\n";
    //             }
    //         }
            
    //     }
    // }
    //-----------------------------------------------------------------------------------


    //TODO: TERMINAR LA PROYECCION PARA MAS DE UNA CAPA
    // layer_check = 1
    // Si layer_num > layer_check:
    // despues de agregar los nodos a una nueva cara

    //primera proyeccion
    unsigned int layer_check = 1;
    unsigned int initial_faces_offset = this->arr_faces.size();
    vector <unsigned int> newfacePerLayer;
    for (;layer_check<=num_layers;layer_check++){
        // cout << "[DEBUG] Layer: " << layer_check << "\n";
        // cout << "[DEBUG] ArrSize: " << this->arr_faces.size() << "\n";
        // cout << "[DEBUG] init next for: " << initial_faces_offset*(layer_check-1) << "\n";
        for (unsigned int face_idx=initial_faces_offset*(layer_check-1); face_idx < initial_faces_offset*layer_check; face_idx++){
            // cout << "(" << arr_faces.size() << ")\n";
            // cout << "Revisando la cara " << face_idx << " \n";
            unsigned int n_nodes = this->arr_faces[face_idx].size();
            // cout << "Para cara de indice " << face_idx << " Se revisan los " << n_nodes << " nodos de la misma\n";
            newfacePerLayer = {};
            for(unsigned int node_face_rel=0; node_face_rel < n_nodes; node_face_rel++){
                //esto es SUPER ineficiente, corregir para disminuir el computo
                for(unsigned int i=0; i< NPR_arr.size();i++){
                    if (NPR_arr[i].getFIndex() == arr_faces[face_idx][node_face_rel]){
                        // cout << "    Adding: " << NPR_arr[i].getIndex() << " to face " << face_idx << "\n";
                        this->arr_faces[face_idx].push_back(NPR_arr[i].getIndex());
                        // cout << "    Numlayers: " << num_layers << " | LayerCheck: " << layer_check << "\n";
                        //revisar si este condicional va aca o va en otro lado
                        if (layer_check < num_layers){
                            // cout << "    ** NewFaceIndex: " << NPR_arr[i].getIndex()+(initial_faces_offset*layer_check-1) << "\n";
                            newfacePerLayer.push_back(NPR_arr[i].getIndex());
                            
                            // layer_check++;
                        }

                    }     
                    
                }
                
                // layer_check = 1;
                // layer_check ++;
                
            }

            if (newfacePerLayer.size()>0 && layer_check < num_layers){
                // cout<<"[DEBUG] NewFaceSize: "<<newfacePerLayer.size()<<"\n";
                this->arr_faces.push_back(newfacePerLayer);
            }

            // if (newfacePerLayer.size() > 0){
            //     cout << "*Agregando ("<< newfacePerLayer.size() <<")-> [";
            //     for(int i=0; i< newfacePerLayer.size(); i++){
            //         cout << newfacePerLayer[i] << " ";
            //     }
            //     cout << "]\n";
            // }
            
            
            // if (newfacePerLayer.size()>0){
            //     // cout << "newfacePerLayer -> " << newfacePerLayer.size() << "\n";
            //     this->arr_faces.push_back(newfacePerLayer);
            // }
            
            
        }
        
    }
    
    

    // cout << "--NEW--\n";
    // for(int i=0; i< this->arr_faces.size(); i++){
    //     cout << "[";
    //     for(int j=0; j < this->arr_faces[i].size(); j++){
    //         cout << this->arr_faces[i][j] << " ";
    //     }
    //     cout << "]\n";
    // }
    
    
    
}

vector <vector<unsigned int>> AdvancingPoint::getFaces(){ return this->arr_faces;}
vector <Point3D> AdvancingPoint::getPoints(){return this->arr_points;}
vector <Point3D> AdvancingPoint::getNewPoints(){return this->new_points;}
vector <NodeProjection> AdvancingPoint::getNormals(){return this->normals;}