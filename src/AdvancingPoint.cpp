#include "TriMesh.h"
#include "FEMesh.h"
#include "Services.h"
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
#include<math.h>


using Clobscode::Point3D;
using Clobscode::TriMesh;

using namespace std;
using std::atoi;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;


/*
AdvancingPoint
Resumen: Funcion que consolida todo el proceso de expansion exterior enfocada en nodos
Inputs: 
        - Puntos: arreglo de Point3D entregados por archivo de entrada.
        - VUI: arreglo que contiene los vectores de puntos correspondientes a las caras superficiales de la malla de entrada
        - dist: utilidad opcional para modificar la distancia de la expansion (asociada a parametro -e)
        - num_layers: utilidad opcional para modificar la cantidad de capas expandidas (asociada a parametro -l)
        - Whitelist_faces: vector con los indices de las caras a expandir (asociada al parametro -f)
        - distance_multiplier: utilidad opcional para modificar tasa de distancia entre capas expandidas (asociada a parametro -x)
        - faces_whitelist_given: argumento opcional configurado automaticamente para realizar verificaciones de ejecucion.

Retorno: Se retorna un objeto que posee los arreglos conrrespondientes a toda la generacion de nuevos elementos asociados a la expansion.
*/
AdvancingPoint::AdvancingPoint(vector<Clobscode::Point3D> &Puntos, vector<vector<unsigned int>> &VUI, float dist, unsigned int num_layers, vector <unsigned int> Whitelist_faces, float distance_multiplier, bool faces_whitelist_given){
    

    vector <Face> FVector;
    for (unsigned int i=0;i<VUI.size(); i++){
        Face Ftemp(VUI[i]);
        FVector.push_back(Ftemp);
    }

    NormalRepair NR = NormalRepair(Puntos, FVector);

    vector < vector <unsigned int>> VUI2;
    
    vector <Face> NRF;
    if (faces_whitelist_given){
        FVector.clear();
        for (long unsigned int i=0; i < VUI.size(); i++){
            // filter whitelist v2
            if (Whitelist_faces[i] == 1){
                FVector.push_back(NR.getFaces()[i]);
                VUI2.push_back(VUI[i]);
            }
        }
        NRF = FVector;
    }
    
    else {
        VUI2 = VUI;
        NRF = NR.getFaces();
    }


    vector <unsigned int> nodesInSurface; //1 si el nodo i-esimo esta en la superficie, 0 en caso contrario
    vector <unsigned int> pointsInWhitelist; //1 si el nodo i-esimo esta en la whitelist, 0 en caso contrario
    
    for (unsigned int i=0; i< Puntos.size();i++){
        nodesInSurface.push_back(0);
        if (faces_whitelist_given){
            pointsInWhitelist.push_back(1);
        }
        else
        pointsInWhitelist.push_back(0);
    }

    for (unsigned int i=0; i<VUI2.size(); i++){
        for(unsigned int j=0; j<VUI2[i].size(); j++){
            if (pointsInWhitelist[VUI2[i][j]] == 0){
                pointsInWhitelist.at(VUI2[i][j]) = 1;
            }
        }
    }
    
    for (unsigned int fidx=0; fidx< FVector.size(); fidx++){        
        for (unsigned int nidx=0; nidx < nodesInSurface.size(); nidx++){
            if(FVector[fidx].hasPoint(nidx) && nodesInSurface[nidx]==0){
                nodesInSurface.at(nidx) = 1;
            }
        }
    }

    vector <NodeProjection> NodeProjectionVector;
    NodeProjectionVector.reserve(Puntos.size());
    int deb = 0;

    vector <NewPointRef> NPR_arr;
    
    // Inicializacion de objeto NodeProjection
    for (unsigned int i=0; i<Puntos.size(); i++){
        if (deb==0){
            if (nodesInSurface[i] == 1 && pointsInWhitelist[i] == 1){
                NodeProjection NP(i, Puntos[i], NRF, Whitelist_faces);
                
                if (NP.getFacesInvolved().size() > 0){
                    
                    NP.CalcPreNormal(Puntos, 0);
                    NP.Normalize();
                    NodeProjectionVector.push_back(NP);

                    //condicional en caso de problemas en normales 
                    if(isnan(NP.getNormal().X()) || isnan(NP.getNormal().Y()) || isnan(NP.getNormal().Z())){
                        NP.CalcPreNormal(Puntos,1);
                    }
                    
                }
            }
        }

        else{
            std::cout << "\n\nindex punto :" << i << "\n";
            
            std::cout << "Inicializacion ...";
            NodeProjection NP(i, Puntos[i], NRF, Whitelist_faces);
            std::cout << "OK\n";
            if (NP.getFacesInvolved().size() > 0){
                cout << "getFacesInvolvedSize(): " << NP.getFacesInvolved().size() << "\n";
                // funcion que calcula la normal acumulada de las caras que involucran al nodo NP
                std::cout << "Calculo de Prenormales...";
                NP.CalcPreNormal(Puntos, 0U);
                std::cout << "OK\n";
                // Funcion que normaliza el valor en la normal acumulada de las caras que involucran al nodo NP
                std::cout << "Normalizando Prenormal...";
                NP.Normalize();
                std::cout << "OK\n\n";
                // Funcion de utilidad que imprime toda la informacion en el objeto NP
                NodeProjectionVector.push_back(NP);
            }
            else {
                cout << "No hay caras para proyectar nodos.\n";
                cout << "getFacesInvolvedSize(): " << NP.getFacesInvolved().size() << "\n\n";
                NP.setNormal(0,0,0);
                NodeProjectionVector.push_back(NP);

            }
        }
        
    }

    //RECORRER PROYECCIONES, NO PUNTOS
    // generacion de puntos nuevos
    unsigned int offset = 0;
    unsigned int father_offset=0;
    for (unsigned int layer=1; layer <= num_layers; layer++){
        for (unsigned int pointIdx = 0; pointIdx < NodeProjectionVector.size(); pointIdx++){
                Point3D newPoint(
                    Puntos[NodeProjectionVector[pointIdx].getNodeIndex()].X()+
                        (layer*dist*NodeProjectionVector[pointIdx].getNormal().X()*pow(distance_multiplier,layer)),   
                    Puntos[NodeProjectionVector[pointIdx].getNodeIndex()].Y()+
                        (layer*dist*NodeProjectionVector[pointIdx].getNormal().Y()*pow(distance_multiplier,layer)),        
                    Puntos[NodeProjectionVector[pointIdx].getNodeIndex()].Z()+
                        (layer*dist*NodeProjectionVector[pointIdx].getNormal().Z()*pow(distance_multiplier,layer)));       
                this -> new_points.push_back(newPoint); //posibilidad de tener que borrar este arreglo para dejarle la carga a la estructura NewPointRef

                if (layer == 1){
                    NewPointRef NewNode(Puntos.size()+offset, NodeProjectionVector[pointIdx].getNodeIndex(), newPoint); 
                    NPR_arr.push_back(NewNode);
                }
                else{
                    NewPointRef NewNode(Puntos.size()+offset, Puntos.size()+father_offset, newPoint); 
                    NPR_arr.push_back(NewNode);
                    father_offset ++;
                }
                
                offset++;

                
         
        }
    }
    
    this -> normals = NodeProjectionVector;
    this -> arr_points = Puntos;
    this -> arr_faces = VUI2;


    
    unsigned int layer_check = 1;
    unsigned int initial_faces_offset = this->arr_faces.size();
    vector <unsigned int> newfacePerLayer;
    int layer_debug = 0;
    for (;layer_check<=num_layers;layer_check++){
        if (layer_debug == 1){
            cout << "[DEBUG] Layer: " << layer_check << "\n";
            cout << "[DEBUG] ArrSize: " << this->arr_faces.size() << "\n";
            cout << "[DEBUG] init next for: " << initial_faces_offset*(layer_check-1) << "\n";
        }
        for (unsigned int face_idx=initial_faces_offset*(layer_check-1); face_idx < initial_faces_offset*layer_check; face_idx++){
            
                unsigned int n_nodes = this->arr_faces[face_idx].size();
                
                newfacePerLayer = {};
                for(unsigned int node_face_rel=0; node_face_rel < n_nodes; node_face_rel++){
                    
                    for(unsigned int i=0; i< NPR_arr.size();i++){ 
                        if (NPR_arr[i].getFIndex() == arr_faces[face_idx][node_face_rel]){
                            
                            this->arr_faces[face_idx].push_back(NPR_arr[i].getIndex());
                           
                            if (layer_check < num_layers){
                                newfacePerLayer.push_back(NPR_arr[i].getIndex());
                            }
                            break;
                        }     
                    }
                }

                if (newfacePerLayer.size()>0 && layer_check < num_layers){
                    this->arr_faces.push_back(newfacePerLayer);
                }          

        }
        
    }
    
}

//Getters
vector <vector<unsigned int>> AdvancingPoint::getFaces(){ return this->arr_faces;}
vector <Point3D> AdvancingPoint::getPoints(){return this->arr_points;}
vector <Point3D> AdvancingPoint::getNewPoints(){return this->new_points;}
vector <NodeProjection> AdvancingPoint::getNormals(){return this->normals;}