#ifndef FacesPerNode_h
#define FacesPerNode_h 1


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
using std::vector;
// using std::string;


class FacesXNode{
    private:
        vector <vector <unsigned int>> FacesPerNode;

    public:
        FacesXNode(vector <Point3D> &puntos, vector<Face> &caras);

        

};
#endif