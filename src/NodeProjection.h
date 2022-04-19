#ifndef NodeProjection_h
#define NodeProjection_h 1


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



// using namespace std;
// using std::atoi;
// using std::cout;
// using std::endl;
// using std::cerr;
// using std::vector;
// using std::string;


class NodeProjection{
    private:
        Point3D NodeSrc;
        unsigned int Node_index;
        Point3D Normal;
        vector< vector<unsigned int> > FacesInvolved;

    public:
        NodeProjection(unsigned int Node_index, Point3D Node, vector<Face> &fv, vector <unsigned int> Whitelist_faces = {});

        void print();

        //devuelve el indice del elemento siguiente segun el nodo en cuestion
        unsigned int NextNode(vector <unsigned int> FaceNodes);
        
        unsigned int PreviousNode(vector <unsigned int> FaceNodes);

        void CalcPreNormal(vector <Point3D> Puntos, unsigned int debug=0);

        void MeanNormal();

        void Normalize();

        Point3D getNormal();

        void setNormal(float X, float Y, float Z);

        vector<vector<unsigned int>> getFacesInvolved();

        void print(vector <unsigned int>Fv);

        unsigned int getNodeIndex();

};
#endif