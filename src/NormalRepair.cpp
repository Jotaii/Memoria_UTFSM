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
    // unsigned int idx = 0;
    // Point3D pto_cercano = puntos[0];
    // for(int i=1; i<puntos.size();i++){
    //     if (out_bbox.distance(puntos[i]) < out_bbox.distance(pto_cercano)){
    //         pto_cercano = puntos[i];
    //         idx = i;
    //     }
    // }

    // // unsigned int fdx;
    // vector<Face> face_cercana;
    // vector<unsigned int> fdx;
    // //Seleccionar todas las caras que contengan el punto
    // for(int i=0; i < fv.size(); i++){
    //     if(fv[i].hasPoint(idx)){
    //         Point3D normal_cara;
    //         face_cercana.push_back(fv[i]);
    //         fdx.push_back(i);
    //     }
    // }
    
    vector<vector<unsigned int>> fvInt;
    for(int i=0; i < fv.size(); i++){
        fvInt.push_back(fv[i].getPoints());
            
    }
    
    //TODO: calcular la normal de la cara
    TriMesh tMesh(puntos, fvInt);
    
    //cout << "testing point " << caca;
		
    // index of the closest triangle
    unsigned int closestTriangle = 0;
    // closest point on the triangle (on triangle face, on edge, or vertice)
    Point3D pProjP;
    // distance to this closest point (always positive)
    double pDist = 0;
    //current closest distance: positive infinity
    double closestDist = numeric_limits<double>::infinity();
    // true if this node is inside the surface
    bool pIsIn = false;
    bool bIsIn = false;
    // 0 if close to a face, 1 if close to an edge, 2 if close to a vertice
    int faceEdgeNode = 0;
    int iFaceEdgeNode = 0;
    
    if (tMesh.getFaces().empty()) {
        this->faces = {}; // no se devuelven caras dado que no hay triangulos en el mesh.
    }
    
    else{
        // browsing all the surface faces for min distance.
        for (unsigned int iSurfF = 0; iSurfF < tMesh.getFaces().size(); iSurfF++) {
            // computing the distance for this face (triangle)
            // tMesh.SignedDistToTriangle(out_bbox,iSurfF,closestDist,pDist,pProjP,pIsIn,faceEdgeNode);
            // bool TriMesh::SignedDistToTriangle(const Point3D & pP, const unsigned int &iT, 
            // 							   const double &current_min_dist, double & pDist, 
            // 							   Point3D & pProjP, bool & pIsIn, int & faceEdgeNode)
        
            {
                // compute the shortest distance between pP and the triangle (index iT).
                
                // inputs: 
                // Point3D pP, the point to be projected
                // uInt iT, the index of the triangle to test
                
                // outputs
                // double & pDist, the signed distance between the point and the triangle
                // VPoint3D & pProjP, the projected point
                // bool pIsIn, boolean expressing if the node P is inside or outside the surface
                
                
                // declare points and normals of this triangle.
                // p* the vertices of the triangle
                // pN*, the pseudo normals at each vertices of the triangle
                // pN**, the pseudo normals at each edges of the triangle		
                Point3D pA = tMesh.getPoints()[tMesh.getFaces()[iSurfF][0]];
                Point3D pB = tMesh.getPoints()[tMesh.getFaces()[iSurfF][1]];
                Point3D pC = tMesh.getPoints()[tMesh.getFaces()[iSurfF][2]];
                
                Point3D pNA = tMesh.getVerticePseudoNormals()[tMesh.getFaces()[iSurfF][0]];
                Point3D pNB = tMesh.getVerticePseudoNormals()[tMesh.getFaces()[iSurfF][1]];
                Point3D pNC = tMesh.getVerticePseudoNormals()[tMesh.getFaces()[iSurfF][2]];
                
                Point3D pNab = tMesh.getmEdgePseudoNormals()[iSurfF*3];
                Point3D pNbc = tMesh.getmEdgePseudoNormals()[iSurfF*3+1];
                Point3D pNac = tMesh.getmEdgePseudoNormals()[iSurfF*3+2];
                
                // Triangle vectors
                Point3D AB = pB - pA;
                Point3D AC = pC - pA;
                Point3D BC = pC - pB;
                
                // Normal vector pointing towards positive half-space, assuming (ABC) is CCW
                Point3D N  = AB^AC;
                double N2  = N * N;
                
                // normal used to compute the sign
                Point3D pProjN;
                
                //True if the given node is co-planar to this face
                bool coplanar = false;
                
                //------------------------------
                // Projection on plane (ABC)
                //------------------------------
                
                // P1 = P projected on plane
                Point3D P1;
                Point3D AP1;
                bool isInsideTriag = true;
                {
                    // Projection of P on ABC plane
                    Point3D AP = out_bbox - pA;
                    double k = ( AP * N ) / N2;
                    
                    // do not trust this face when the node is co-planar 
                    // to it and the result is "outside", unless all 
                    // faces be co-planar to the node.
                    if (fabs(k)<1E-8) {
                        coplanar = true;
                    }
                    
                    P1 = out_bbox - k*N;
                                
                    // Check if P1 is in triangle
                    AP1 = P1 - pA;
                    double x = (AP1 ^  AC) * N / N2;
                    double y = (AB  ^ AP1) * N / N2;
                    
                    // AP1 = x*AB + y*AC
                    if( x>=0 && y>=0 && x+y<=1 )
                    {
                        // Normal interpolation
                        // real normal of the triangle
                        pProjN = N;
                        
                        // Signed distance
                        double lAbsDist = out_bbox.DistanceTo(P1);
                        double lSgn = pProjN * ( out_bbox - P1 );
                        
                        // Projection params
                        pProjP = P1;
                        pDist = lSgn<0 ? -lAbsDist : lAbsDist ;
                        
                        // computing if the node is inside the surface
                        pIsIn = lSgn < 0;
                        
                        faceEdgeNode = 0;
                        
                        // return false as the node is inside the triangle
                        isInsideTriag = false;
                    }
                }
                

                //-------------------------------------------------------------------
                // P1 is out of the triangle : compute projections on triangle edges
                //-------------------------------------------------------------------
                
                //-------------------------------
                // Distance to segment AB
                //-------------------------------
                {
                    double AB2 = AB * AB;
                    double t = ( AP1 * AB ) / AB2;
                    
                    if( t < 0 )
                    {
                        pProjP = pA;
                        pProjN = pNA;
                        
                        faceEdgeNode = 2;
                    }
                    else
                        if( t > 1 )
                        {
                            pProjP = pB;
                            pProjN = pNB;
                            
                            faceEdgeNode = 2;
                        }
                        else
                        {
                            pProjP = pA + t * AB;
                            // pseudo normal of the edge AB
                            pProjN = pNab;
                            faceEdgeNode = 1;
                        }
                    
                    pDist = pProjP.DistanceTo( out_bbox );
                }
                
                //-------------------------------
                // Distance to segment AC
                //-------------------------------
                {
                    double AC2 = AC * AC;
                    double t = ( AP1 * AC ) / AC2;
                    
                    Point3D P2;
                    Point3D P2N;
                    if( t < 0 )
                    {
                        P2 = pA;
                        P2N = pNA;
                    }
                    else
                        if( t > 1 )
                        {
                            P2 = pC;
                            P2N = pNC;
                        }
                        else
                        {
                            P2 = pA + t * AC;
                            // pseudo normal of the edge AC
                            P2N = pNac;
                        }
                    
                    double d = P2.DistanceTo( out_bbox );
                    
                    // Better ?
                    if( d < pDist )
                    {
                        // Update
                        pDist = d;
                        pProjP = P2;
                        pProjN = P2N;
                        
                        if ((t>=0) && (t<=1)) faceEdgeNode = 1;
                        else faceEdgeNode = 2;
                    }
                }
                
                //-------------------------------
                // Distance to segment BC
                //-------------------------------
                {
                    double BC2 = BC * BC;
                    double t = ( (P1-pB) * BC ) / BC2;
                    
                    Point3D P2;
                    Point3D P2N;
                    if( t < 0 )
                    {
                        P2 = pB;
                        P2N = pNB;
                    }
                    else
                        if( t > 1 )
                        {
                            P2 = pC;
                            P2N = pNC;
                        }
                        else
                        {
                            P2 = pB + t * BC;
                            // pseudo normal of the edge BC
                            P2N = pNbc;	
                        }
                    
                    double d = P2.DistanceTo( out_bbox );
                    
                    // Better ?
                    if( d < pDist )
                    {
                        // Update
                        pDist = d;
                        pProjP = P2;
                        pProjN = P2N;
                        
                        if ((t>=0) && (t<=1)) faceEdgeNode = 1;
                        else faceEdgeNode = 2;
                    }
                }
                
                //-------------------------------
                // Orientation
                //-------------------------------
                
                // Change distance sign
                if( pProjN * ( out_bbox - pProjP ) < 0)
                {
                    pDist = -pDist;
                }
                
                pIsIn = pDist < 1E-8;
                
                // return coplanar; //check en que afecta esto de coplanar
                
            }
            
            pDist = fabs(pDist);
            
            if (pDist < closestDist) {
                closestTriangle = iSurfF;
                closestDist = pDist;
                bIsIn = pIsIn;
                iFaceEdgeNode = faceEdgeNode;
            }
        }
    }
    
    // if bIsIn is true, then the normal is inverted, repair needed
    if (true == true){
        
        unsigned int initial_face_index = closestTriangle;
        // cout << bIsIn << "\tfdsafadsf\n";
        // cout << closestDist << "\tfdsafadsf\n";
       
        if (bIsIn == true){
            cout << "Reparacion de la primera cara con indice " << initial_face_index << "\n";
            for (int i=0; i < fv[initial_face_index].getPoints().size(); i++){
                cout << fv[initial_face_index].getPoints()[i] << "\t";
            }
            cout << "\n";
            repair_face(fv[initial_face_index].getPoints());
        }
        faceChecked.at(initial_face_index) = 1;

        // CODIGO VIEJO, CREO QUE NO SE VA A UTILIZAR -----------------------------
        
        // for (int pointIndex=0; pointIndex < fv[initial_face_index].getPoints().size(); pointIndex++){
        //     if (pointIndex+1 == fv[initial_face_index].getPoints().size()){
        //         RecursiveRepair(fv[initial_face_index].getPoints()[pointIndex], fv[initial_face_index].getPoints()[0], fv, faceChecked, facesContainsPoint);
        //     }
        //     else {
        //         RecursiveRepair(fv[initial_face_index].getPoints()[pointIndex], fv[initial_face_index].getPoints()[pointIndex+1], fv, faceChecked, facesContainsPoint);
        //     }
        // }
        // ------------------------------------------------------------------------

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
        for (int i=0; i<fv.size(); i++){
            if (initial_face_index + i >= fv.size()){
                idxCheck = i;
            }
            else {
                idxCheck = initial_face_index + i;
            }
            // cout << "\n\n" << idxCheck << "\n\n" << initial_face_index << "\n\n";
            for (int j=0; j < fv[idxCheck].getPoints().size(); j++){
                unsigned int p1,p2;
                if (allchecked(faceChecked)==true){
                    // print(fv);
                    i = fv.size();
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
                // cout << prueba_reparacion_rec(p1,p2, idxCheck,fv,faceChecked) << "]";
                // print(facesChecked);

            }

        }
        this -> faces = fv;

    }


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

// void print(vector<vector<unsigned int>> V){
//     for (int i=0; i<V.size(); i++){
//         cout << i <<" | [ ";
//         for (int j=0; j < V[i].size(); j++){
//             cout << V[i][j];
//             if (j != V[i].size()-1){
//                 cout << ", ";
//             }
//         }
//         cout << "]\n";
//     }
// }

// void print(vector <unsigned int>V){
//     cout << "[ ";
//     for (int j=0; j < V.size(); j++){
//         cout << V[j];
//         if (j != V.size()-1){
//             cout << ", ";
//         }
//     }
//     cout << "]\n";
// }

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
        cout << "Tiene 3\n";
        unsigned int temp_index = pointsTemp[2];
        faceToBeRepaired.replacePoint(pointsTemp[2], pointsTemp[1]);
        faceToBeRepaired.replacePoint(pointsTemp[1], temp_index);
    }
    else if (faceToBeRepaired.numberOfPoints() == 4){
        cout << "Tiene 4\n";
        unsigned int temp_index = pointsTemp[3];
        faceToBeRepaired.replacePoint(pointsTemp[3], pointsTemp[1]);
        faceToBeRepaired.replacePoint(pointsTemp[1], temp_index);
    }
}

//if the 2-Points are in the same order in the face to be checked, need repair.
// bool isRepairNeeded(Face faceToBeChecked, unsigned int pointA, unsigned int pointB){
//     for (int i=0; i<faceToBeChecked.getPoints().size();i++){
//         if (faceToBeChecked.getPoints()[i] == pointA){
//             if ((i+1) == faceToBeChecked.getPoints().size() && faceToBeChecked.getPoints()[0] == pointB){
//                 return true;
//             }
//             else {
//                 if (faceToBeChecked.getPoints()[i+1] == pointB){
//                     return true;
//                 }
//             }
//         }
//     }
//     return false;
// }

// int faceContainsPoints(vector<vector<unsigned int>> facesByPoints, unsigned int pointA, unsigned int pointB){
//     for (int i=0; i<facesByPoints.size(); i++){
//         unsigned int contains = 0;
//         for (int j=0; j<facesByPoints[i].size(); j++){
//             if (pointA == facesByPoints[i][j]){
//                 contains++;
//             }
//             else if (pointB == facesByPoints[i][j]){
//                 contains++;
//             }
//         }
//         if (contains == 2){
//             return i;
//         }
//     }
//     return -1; //no face with the points given
// }

// int RecursiveRepair(unsigned int pointA, unsigned int pointB, vector<Face> faceV, vector<unsigned int> faceChecked, vector<vector<unsigned int>> facesByPoints){
//     //check if are unchecked faces
//     bool all_checked = true;
//     for(int i=0; i<faceChecked.size();i++){
//         if (faceChecked[i] == 0){
//             all_checked = false;
//             break;
//         }
//     }
//     if (all_checked == true) return 1;

//     else{
//         int auxIndex = faceContainsPoints(facesByPoints, pointA, pointB);
//         if ( auxIndex != -1 && faceChecked[auxIndex] == 0 && isRepairNeeded(faceV[auxIndex],pointA, pointB)==true){
//             RepairFace(faceV[auxIndex]);
//             faceChecked.at(auxIndex) = 1;
//         }
//     }
// }

vector <Face>  NormalRepair::getFaces(){
    return this->faces;
}
// };