#ifndef AdvancingPoint_h
#define AdvancingPoint_h 1
#define POINT_OFFSET 4

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


class AdvancingPoint{
    private:
        vector <Point3D> arr_points;
        vector <Point3D> new_points;
        vector <NodeProjection> normals;
        vector <vector <unsigned int>>  arr_faces;

    public:
        //probablemente a esta funcion haya que agregarle los parametros de whitelist de caras, distancia y cantidad de iteraciones.
        AdvancingPoint(vector <Point3D> &Puntos, vector<vector<unsigned int>> &VUI, float dist=1);
        
        vector <vector <unsigned int>> getFaces();
        vector <Point3D> getPoints();
        vector <Point3D> getNewPoints();
        vector <NodeProjection> getNormals();

};  
#endif