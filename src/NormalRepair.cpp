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
#include "NormalRepair.h"
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


using namespace std;
using std::atoi;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;


NormalRepair::NormalRepair(vector<Clobscode::Point3D> &puntos, vector<Face> &fv){
    //create relation between points and faces
    // vector <vector <unsigned int>> facesContainsPoint;
    for (int i=0; i< puntos.size(); i++){
        vector <unsigned int> vecTemp;
        for (int j=0; j<fv.size(); j++){
            for (int k=0; k<fv[j].getPoints().size(); k++){
                if (i == fv[j].getPoints()[k]){
                    vecTemp.push_back(j);
                }
            }
        }
        if (vecTemp.size()>0){
            this->facesContainsPoint.push_back(vecTemp);
        }
    }
    
    //initialize checked faces list
    vector <unsigned int> faceChecked;
    for (int f=0; f<fv.size(); f++){
        faceChecked.push_back(0);
    }

    vector <vector <unsigned int>> caras_seccionadas;
    vector <unsigned int> caras_ref;
    unsigned int ancla = 0;
    // std::cout << "Caras originales: \n";
    for(unsigned int cara_index=0; cara_index < fv.size(); cara_index++){
        // print(fv[cara_index].getPoints(),1);
        for (unsigned int tridx = 0; tridx < fv[cara_index].getPoints().size()-2; tridx++){
            vector <unsigned int> tmp;
            tmp.push_back(fv[cara_index].getPoints()[ancla]);
            tmp.push_back(fv[cara_index].getPoints()[ancla+1+tridx]);
            tmp.push_back(fv[cara_index].getPoints()[ancla+2+tridx]);
            caras_seccionadas.push_back(tmp);
            caras_ref.push_back(cara_index);
            tmp.clear();
        }
    }

    
    //initialize and set min/max points of bbox
    this->min.X() = this->max.X() = puntos[0].X();
    this->min.Y() = this->max.Y() = puntos[0].Y();
    this->min.Z() = this->max.Z() = puntos[0].Z();
    for(int i=1; i<puntos.size();i++){
        if (puntos[i].X() > max.X()){
            this->max.X() = puntos[i].X();
        }
        if (puntos[i].X() < min.X()){
            this->min.X() = puntos[i].X();
        }
        if (puntos[i].Y() > max.Y()){
            this->max.Y() = puntos[i].Y();
        }
        if (puntos[i].Y() < min.Y()){
            this->min.Y() = puntos[i].Y();
        }
        if (puntos[i].Z() > max.Z()){
            this->max.Z() = puntos[i].Z();
        }
        if (puntos[i].Z() < min.Z()){
            this->min.Z() = puntos[i].Z();
        }

    }
    // bbox definida con punto minimo y punto maximo

    // se define punto al exterior de la bounding box
    Point3D out_bbox(this->max.X()+POINT_OFFSET, this->max.Y()+POINT_OFFSET, this->max.Z()+POINT_OFFSET);

    
    vector<vector<unsigned int>> fvInt;
    for(int i=0; i < fv.size(); i++){
        fvInt.push_back(fv[i].getPoints());
            
    }
    
    TriMesh tMesh(puntos, caras_seccionadas);
    
    // std::cout << "Puntos TriMesh:" << tMesh.getPoints().size() << "\n";
    // std::cout << "Caras TriMesh:" << tMesh.getFaces().size() << "\n";

    vector <unsigned int> output_isinMesh = tMesh.pointIsInMeshIdx(out_bbox);
    // if (output_isinMesh.size() == 3){
    //     std::cout << "InMesh? " << output_isinMesh[0] << "\nFaceIndex: " 
    //          << caras_ref[output_isinMesh[1]] << "\nNeedRepair? " << output_isinMesh[2] << "\n";
    // }


    unsigned int initial_face_index = caras_ref[output_isinMesh[1]];//closestTriangle;
    
    
    if (output_isinMesh[2] == 1){
        // cout << "Reparacion de la primera cara con indice " << initial_face_index << "\n";
        // for (int i=0; i < fv[initial_face_index].getPoints().size(); i++){
        //     cout << fv[initial_face_index].getPoints()[i] << "\t";
        // }
        // cout << "\n";
        repair_face(fv[initial_face_index].getPoints());
        // for (int i=0; i < fv[initial_face_index].getPoints().size(); i++){
        //     cout << fv[initial_face_index].getPoints()[i] << "\t";
        // }
        // cout << "\n";
    }
    faceChecked.at(initial_face_index) = 1;

    // arma arreglo de puntos segun caras
    for (int i=0; i< points.size(); i++){
        vector <unsigned int> vecTemp;
        for (int j=0; j<fv.size(); j++){
            for (int k=0; k<fv[j].getPoints().size(); k++){
                if (i == fv[j].getPoints()[k]){
                    vecTemp.push_back(j);
                }
            }
        }
        if (vecTemp.size()>0){
            facesContainsPoint.push_back(vecTemp);
        }
    }

    //Toda la logica de la correccion de normales, al final el arreglo VUI tiene las caras con normales arregladas
    int idxCheck;
    // por cada cara en el arreglo, comenzando desde la cara inicial
    for (int i=0; i<fv.size(); i++){
        if (initial_face_index + i >= fv.size()){
            idxCheck = (initial_face_index +i)%fv.size();
        }
        else {
            idxCheck = initial_face_index + i;
        }
        

        for (int j=0; j < fv[idxCheck].getPoints().size(); j++){
            unsigned int p1,p2;
            if (allchecked(faceChecked)==true){
                i = fv.size();
                break;
            }

            if (j == fv[idxCheck].getPoints().size()-1){
                p1 = fv[idxCheck].getPoints()[j];
                p2 = fv[idxCheck].getPoints()[0];
            }
            else {
                p1 = fv[idxCheck].getPoints()[j];
                p2 = fv[idxCheck].getPoints()[j+1];
            }
            // cout << "(" << p1 << "," << p2 << ")@" << idxCheck << "[";
            prueba_reparacion_rec(p1,p2, idxCheck,fv,faceChecked);
            // cout << prueba_reparacion_rec(p1,p2, idxCheck,fv,faceChecked) << "]\n";
            // print(faceChecked); cout << "\n";

        }

    }
    this -> faces = fv;

    // for (unsigned int fvidx=0; fvidx<this->faces.size(); fvidx++){
    //     for(unsigned int FFF=0; FFF<this->faces[fvidx].getPoints().size(); FFF++){
    //         cout << faces[fvidx].getPoints()[FFF] << " ";
    //     }
    //     cout << "\n";
    // }


}

bool NormalRepair::repair_needed(unsigned int p1, unsigned int p2, vector<unsigned int> Face){
    for (int pts=0; pts<Face.size(); pts++){
        if (pts == Face.size()-1){
            if (Face[pts] == p1 && Face[0] == p2)return true;
        }
        else{
            if (Face[pts] == p1 && Face[pts+1] == p2)return true;
        }
    }
    return false;
}

void NormalRepair::repair_face(vector<unsigned int> &F){
    unsigned int temp;
    //creo que siempre se cambia el ultimo con el segundo, verificar!
    if (F.size() == 3){
        temp = F[2];
        F.at(2) = F[1];
        F.at(1) = temp;
    }
    else if (F.size() == 4){
        temp = F[3];
        F.at(3) = F[1];
        F.at(1) = temp;
    }
}

bool NormalRepair::allchecked(vector<unsigned int> Checked){
    for (int i=0; i<Checked.size();i++){
        if (Checked[i] == 0){
            return false;
        }
    }
    return true;
}

int NormalRepair::buscar_cara_comun(unsigned int p1, unsigned int p2, unsigned int faceIdx, vector<Face>Faces){
    int contador = 0;
    for (int faceidx=0; faceidx < Faces.size(); faceidx++){
        int contador = 0;
        if (faceIdx != faceidx){
            for (int punto_en_cara_idx=0; punto_en_cara_idx < Faces[faceidx].getPoints().size(); punto_en_cara_idx++){
                if (Faces[faceidx].getPoints()[punto_en_cara_idx] == p1) contador++;
                if (Faces[faceidx].getPoints()[punto_en_cara_idx] == p2) contador++;
            }
            if (contador == 2) return faceidx;
        }
    }
    return -1;
}

void NormalRepair::print(vector<vector<unsigned int>> V){
    for (int i=0; i<V.size(); i++){
        cout << i <<" | [ ";
        for (int j=0; j < V[i].size(); j++){
            cout << V[i][j];
            if (j != V[i].size()-1){
                cout << ", ";
            }
        }
        cout << "]\n";
    }
}

void NormalRepair::print(vector <unsigned int>V, unsigned int nl){
    cout << "[ ";
    for (int j=0; j < V.size(); j++){
        cout << V[j];
        if (j != V.size()-1){
            cout << ", ";
        }
    }
    cout << "]";
    if(nl==1){
        cout << "\n";
    }
}

int NormalRepair::prueba_reparacion_rec(unsigned int p1, unsigned int p2, unsigned int faceIdx, 
                        vector<Face> &Faces, vector<unsigned int> &Checked){
    if (allchecked(Checked)==true){
        // cout << "allcheck true\n"; 
        return 1;
    }
    else{
        int aux = buscar_cara_comun(p1,p2,faceIdx,Faces);
        // cout << aux << " " << p1 << " " << p2 << " " << faceIdx << "\n";
        if(aux != -1){ //si es todo conexo siempre habran dos caras que posean los mismos puntos (presuncion fija para memoria)
            if (Checked[aux] == 1){
                return 1;
            }
            else{
                if (repair_needed(p1,p2,Faces[aux].getPoints())==true){
                    repair_face(Faces[aux].getPoints());
                   
                }
                Checked.at(aux) = 1;
                // prueba_reparacion_rec(Faces[aux][0], Faces[aux][1], aux, Faces, Checked);
            }
        }
    }
    return -1;
}


// virtual vector<unsigned int> &getPoints();
// virtual int numberOfPoints();
void NormalRepair::RepairFace(Face faceToBeRepaired){
    vector <unsigned int> pointsTemp = faceToBeRepaired.getPoints();

    if (faceToBeRepaired.numberOfPoints() == 3){
        // cout << "Tiene 3\n";
        unsigned int temp_index = pointsTemp[2];
        faceToBeRepaired.replacePoint(pointsTemp[2], pointsTemp[1]);
        faceToBeRepaired.replacePoint(pointsTemp[1], temp_index);
    }
    else if (faceToBeRepaired.numberOfPoints() == 4){
        // cout << "Tiene 4\n";
        unsigned int temp_index = pointsTemp[3];
        faceToBeRepaired.replacePoint(pointsTemp[3], pointsTemp[1]);
        faceToBeRepaired.replacePoint(pointsTemp[1], temp_index);
    }
}


vector <Face>  NormalRepair::getFaces(){
    return this->faces;
}
