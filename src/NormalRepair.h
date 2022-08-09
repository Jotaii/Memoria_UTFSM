#ifndef NormalRepair_h
#define NormalRepair_h 1
#define POINT_OFFSET 40

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
using Clobscode::TriMesh;



// using namespace std;
// using std::atoi;
// using std::cout;
// using std::endl;
// using std::cerr;
// using std::vector;
// using std::string;


class NormalRepair{
    private:
        vector <Point3D> points; // quizas no es necesario almacenar puntos
        vector <Face> faces;
        Point3D min, max;
        vector <vector <unsigned int>> facesContainsPoint;
        // Point3D NodeSrc;
        // unsigned int Node_index;
        // Point3D Normal;
        // vector< vector<unsigned int> > FacesInvolved;

    public:
        NormalRepair(vector <Point3D> &puntos, vector<Face> &caras);

        void RepairFace(Face FaceToBeRepaired);
        // bool isRepairNeeded(Face faceToBeChecked, unsigned int pointA, unsigned int pointB);

        // void print();

        // //devuelve el indice del elemento siguiente segun el nodo en cuestion
        // unsigned int NextNode(vector <unsigned int> FaceNodes);
        
        // unsigned int PreviousNode(vector <unsigned int> FaceNodes);

        // void CalcPreNormal(vector <Point3D> Puntos, unsigned int debug=0);

        // void MeanNormal();

        // void Normalize();

        // Point3D getNormal();

        // vector<vector<unsigned int>> getFacesInvolved();

        // void print(vector <unsigned int>Fv);

        bool repair_needed(unsigned int p1, unsigned int p2, vector<unsigned int> Face);

        void repair_face(vector<unsigned int> &F);

        bool allchecked(vector<unsigned int> Checked);

        int buscar_cara_comun(unsigned int p1, unsigned int p2, unsigned int faceIdx, vector<Face>Faces);

        void print(vector<vector<unsigned int>> V);

        void print(vector <unsigned int>V, unsigned int nl=0);

        int prueba_reparacion_rec(unsigned int p1, unsigned int p2, unsigned int faceIdx, 
                                vector<Face> &Faces, 
                                vector<unsigned int> &Checked);
        
        vector <Face> getFaces();

};  
#endif