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

NewPointRef::NewPointRef(unsigned int Node_index, unsigned int father_index, Point3D proj){
    this ->father_index = father_index;
    this ->node_index = Node_index;
    this ->projection = proj;
    this -> was_projected = true; // quizas este booleano no es necesario
}

int getByFatherNode(vector <NewPointRef> NPR, unsigned int f_index, Point3D &P){
    for(unsigned int i=0; i< NPR.size();i++){
        if (NPR[i].getFIndex() == f_index){
            P = NPR[i].getProj();
            return 1;
        }
    }
    return -1;
}

unsigned int NewPointRef::getFIndex(){
    return this->father_index;
}

unsigned int NewPointRef::getIndex(){
    return this->node_index;
}

Point3D NewPointRef::getProj(){
    return this->projection;
}


