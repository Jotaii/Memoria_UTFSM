#ifndef NewPointRef_h
#define NewPointRef_h 1


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



// using namespace std;
// using std::atoi;
// using std::cout;
// using std::endl;
// using std::cerr;
// using std::vector;
// using std::string;


class NewPointRef{
    private:
        Point3D projection;
        bool was_projected;
        unsigned int node_index;
        unsigned int father_index;

    public:
        NewPointRef(unsigned int Node_index, unsigned int father_index, Point3D proj);

        Point3D getByFatherNode(vector <NewPointRef> NPR, unsigned int f_index, Point3D &P);
        unsigned int getFIndex();
        unsigned int getIndex();
        Point3D getProj();

};
#endif