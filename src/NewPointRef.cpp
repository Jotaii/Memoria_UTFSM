#include "TriMesh.h"
#include "FEMesh.h"
#include "Services.h"
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


using Clobscode::Point3D;

using namespace std;
using std::atoi;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;

/*
NewPointRef
Resumen: Clase que permite almacenar la relacion entre nodos padre-hijo para facilitar la construccion de nuevos elementos de volumen.
Inputs:
        - Node_index: Indice del nodo a almacenar.
        - father_index: Indice del nodo padre correspondiente.
        - proj: normal correspondiente a la proyeccion.
*/
NewPointRef::NewPointRef(unsigned int Node_index, unsigned int father_index, Point3D proj){
    this ->father_index = father_index;
    this ->node_index = Node_index;
    this ->projection = proj;
    this -> was_projected = true; // quizas este booleano no es necesario
}


// Getters
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


