#include "Mesher.h"
#include "TriMesh.h"
#include "FEMesh.h"
#include "Services.h"
#include "RefinementCubeRegion.h"
#include "RefinementSurfaceRegion.h"
#include "RefinementInputSurfaceRegion.h"
#include "RefinementAllRegion.h"
#include "Point3D.h"
#include "NodeProjection.h"
#include "NormalRepair.h"
#include "AdvancingPoint.h"
#include <string>
#include <cctype>
#include <time.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdlib>

using std::atoi;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
using std::ifstream;
using Clobscode::RefinementRegion;
using Clobscode::RefinementCubeRegion;
using Clobscode::RefinementSurfaceRegion;
using Clobscode::Point3D;

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void endMsg(){
	cout << "use: ./mesher [-d] input.mdl [-o] input.off [-u] output\n";
    cout << "              [-c] volume_mesh.oct (octant mesh to start from)\n";
    cout << "              [-s] ref_level [-a] ref_level [-b] file.reg\n";
    cout << "              [-r] input_surface rl [-g] [-v]\n";
	cout << "where:\n";
	cout << "  one of the parameters must be an input surface mesh in\n";
    cout << "  mdl or off format. If output name is not provided it\n";
	cout << "  will be saved in input_name.m3d. Options:\n";
	cout << "    -s Refine octants intersecting the input surface.\n";
    cout << "       Parameter ref_level is the refinement level\n";
    cout << "    -a Refine all elements in the input domain.\n";
    cout << "       Parameter ref_level is the refinement level\n";
	cout << "    -b Refine block regions provided in file file.reg\n";
    cout << "    -r Refine surface region. Will refine all the elements\n";
    cout << "       in the provided input_surface at level rl\n";
    cout << "    -g save output mesh in GetFem format (gmf)\n";
    cout << "    -v save output mesh in VTK ASCII format (vtk)\n";
    cout << "    -i save output mesh in MVM ASCII format (mvm)\n";
    cout << "    -m save output mesh in M3D ASCII format (m3d)\n";
}

// void print(vector<Face> V){
//     for (unsigned int i=0; i<V.size(); i++){
//         cout << i <<" | [ ";
//         for (int j=0; j < V[i].getPoints().size(); j++){
//             cout << V[i].getPoints()[j];
//             if (j != V[i].getPoints().size()-1){
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


int main(int argc,char** argv){
    
    if (argc<4) {
        endMsg();
        return 0;
    }
    
	string in_name = "", out_name = "";
	bool out_name_given = false, in_name_given = false;
    
	list<RefinementRegion *> all_regions;

    //inputs
    vector <Point3D> Puntos;
    vector <vector <unsigned int>> VUI;
    vector <unsigned int> cellTypes;

    vector<Clobscode::TriMesh> inputs;
    inputs.reserve(4);
    //Clobscode::Services io;
    
    bool getfem=false, vtkformat=false, octant_start=false;
    bool m3dfor=false, mvmfor=false, oneout=false, plyfor=false;
    
    //Default values for options
    int distance_between_layers, layers_qty;
    string faces_whitelist;
    bool distance_given=false, layers_given=false, faces_whitelist_given=false, distance_multiplier_given=false, show_previous_mesh=false;
    float distance_multiplier;

    //for reading an octant mesh as starting point.
    vector<MeshPoint> oct_points;
    vector<Octant> oct_octants;
    set<OctreeEdge> oct_edges;
    vector<unsigned int> oct_ele_link;
    GeometricTransform gt;
    
	for (int i=1; i<argc; i++) {
        
		if (argv[i][0]!='-') { //si la opcion no empieza con guion, entonces es invalida
			cout << "Error: expected option -X and got " << argv[i] << "\n";
			endMsg();
			return 0;
		}
        
        // bool inout = false;                                      *Commented for warning*
        switch (argv[i][1]) {
            case 'g': //si se generara archivo gmf
                getfem = true;
                oneout = true;
                continue;
                break;
            case 'v': //si se generara archivo vtk
                vtkformat = true;
                oneout = true;
                continue;
                break;
            case 'm': //si se generara archivo m3d
                m3dfor = true;
                oneout = true;
                continue;
            case 'i': //si se generara archivo mvm
                mvmfor = true;
                oneout = true;
                continue;
            case 'p': //si se generara archivo ply
                plyfor = true;
                oneout = true;
                continue;
            case 'k':
                show_previous_mesh = true;
                continue;
                break;
            default:
                break;
        }
        
		if (argc==i+1) {
			cout << "Error: expected argument for option " << argv[i] << "\n";
			endMsg();
			return 0;
		}
        

        switch (argv[i][1]) {
            case 'd': //para archivos .mdl
                in_name = argv[i+1];
                if (!Services::ReadMdlMesh(in_name,Puntos, VUI)) {
                    std::cerr << "couldn't read file " << argv[i+1] << std::endl;
                    return 1;
                }
                in_name_given = true;
                i++;
                break;
            case 'o': //para archivos .off
                in_name = argv[i+1];
                
                if (!Services::ReadOffMesh(in_name, Puntos, VUI)) {
                    std::cerr << "couldn't read file " << argv[i+1] << std::endl;
                    return 1;
                }
                in_name_given = true;
                i++;
                break;
            case 'V': //para archivos .vtk
                in_name = argv[i+1];
                if (!Services::readVtk(in_name, Puntos, VUI, cellTypes)) {
                    std::cerr << "couldn't read file " << argv[i+1] << std::endl;
                    return 1;
                }
                in_name_given = true;
                i++;
                break;

            case 'u': //nombre del archivo de salida
                out_name = argv[i+1];
                out_name_given = true;
                i++;
                break;


            // CONFIGURACIONES OPCIONALES PARA GENERACION -------------------------------
            case 'e': //para distancia entre capas de proyeccion
                distance_between_layers = atoi(argv[i+1]);
                distance_given = true;

                i++;
                break;

            case 'l': //para cantidad de capas
                layers_qty = atoi(argv[i+1]);
                layers_given = true;

                i++;
                break;
            
            case 'x': //para multiplicador
                distance_multiplier = atof(argv[i+1]);
                distance_multiplier_given = true;
                i++;
                break;
                
            case 'f': //para whitelist
                faces_whitelist = argv[i+1];
                faces_whitelist_given = true;
                i++;
                break;
            
            
            // --------------------------------------------------------------------------

            default:
                cerr << "Warning: unknown option " << argv[i] << " skipping\n";
                break;
        }

    }
    
    if (!in_name_given) {
        cerr << "No input domain surface mesh provided. Aborting\n";
        return 0;
    }

    // Se inicializa el arreglo que indica si las caras deben ser o no expandidas (se llena con ceros)
    vector <unsigned int> Whitelist_faces;
    
    for (unsigned int i=0; i < VUI.size(); i++){
        Whitelist_faces.push_back(0);
    }

    vector < vector <unsigned int>> VUI2;
    if (faces_whitelist_given){
        //lectura de archivo intento 1
        std::ifstream indata;
        unsigned int num; // variable for input value
        indata.open(faces_whitelist); // opens the file
        if(!indata) { // file couldn't be opened
            cerr << "Error: El archivo " << faces_whitelist << " no puede ser abierto o no existe" << endl;
            exit(1);
        }
        indata >> num;
        while ( !indata.eof() ) { // keep reading until end-of-file
            if (num < VUI.size()){
                Whitelist_faces.at(num) = 1;
            }
            // cout << "The next number is " << num << endl;
            indata >> num; // sets EOF flag if no value found
        }
        indata.close();
        cout << "Whitelist cargada correctamente.." << endl;
        // cout << "[";
        // for (unsigned int i=0; i < Whitelist_faces.size(); i++){
        //     cout << Whitelist_faces[i] << " ";
        // }
        // cout << "]\n";

        
        for (long unsigned int i=0; i < VUI.size(); i++){
            // filter whitelist v2
            if (Whitelist_faces[i] == 1){
                // VUI.erase(VUI.begin() + i);
                VUI2.push_back(VUI[i]);
            }
            
        }
        // cout << "[";
        // for (unsigned int i=0; i < VUI2.size(); i++){
        //     cout << "[";
        //     for (unsigned int j=0; j < VUI2[i].size(); j++){
        //         cout << VUI2[i][j] << " ";
        //     }
        //     cout << "], ";
        // }
        // cout << "]\n";
    }
    else {
        VUI2 = VUI;
    }

    if (distance_given == false) {
        distance_between_layers = 1;
    }

    if (layers_given == false){
        layers_qty = 1;
    }

    if (distance_multiplier_given == false){
            distance_multiplier = 1;
        }
    

    // else {
    //     // for (int i=0; i<Puntos.size(); i++){
    //     //     std::cout << Puntos[i] << "\n";
    //     // }
    //     vector <Face> FVector;
    //     for (int i=0;i<VUI.size(); i++){
    //         Face Ftemp(VUI[i]);
    //         FVector.push_back(Ftemp);
    //         // FVector[i].print();
    //     }

    //     std::cout << "Numero de Nodos: " << Puntos.size() << "\n";
    //     std::cout << "Numero de caras: " << FVector.size() << "\n";
    // }
	
    
	//give default output name if non is provided
	if (!out_name_given) {
		unsigned int last_point = in_name.find_last_of(".");
		out_name = in_name.substr(0,last_point);
	}
	
    // auto start_time = chrono::high_resolution_clock::now();
    
    // //Generate the mesh following the above constraints.
	// Clobscode::Mesher mesher;
    Clobscode::FEMesh output;



    /***************************** Codigo que genera las mallas nuevas **********************************/
    
    AdvancingPoint AP(Puntos, VUI2, distance_between_layers, layers_qty, Whitelist_faces, distance_multiplier); // Hasta aqui tengo las normales por punto, los puntos, las caras del cascaron con su normal arreglada.
    
    //BORRAR ESTE FOR, solo para control debuger
    // for (long unsigned int i=0; i<AP.getNormals().size(); i++){
    //     cout << "normal : " << AP.getNormals()[i].getNormal() << " perteneciente al punto original: " << AP.getNormals()[i].getNodeIndex() << "\n";
    // }

    cout << "VUI SIZE: " << VUI.size() << "\n";
    cout << "VUI2 SIZE: " << VUI2.size() << "\n";
    cout << "NewElems Size: " << AP.getFaces().size() << "\n";
    cout << "cellTypes Size: " << cellTypes.size() << "\n";
    
    unsigned int index_pivot = VUI.size();
    for (unsigned int k=0; k < index_pivot; k++){cout<<k<< " ";} cout << "\n";
    for (unsigned int k=VUI.size(); k<index_pivot+AP.getFaces().size(); k++){cout << k << " ";} cout << "\n";
    
    cout << "oldpoints\t\tnormals\n";
    for (int i=0; i<AP.getNormals().size(); i++){
        cout << AP.getNormals()[i].getNodeIndex() << "-> " << AP.getPoints()[AP.getNormals()[i].getNodeIndex()] << "\t\t" << AP.getNormals()[i].getNormal() << "\n";
    }
    
    //agregar nuevos puntos y elementos a los arreglos
    // cout << "newpoints ["<< AP.getNewPoints().size() <<"]\n";
    for (long unsigned int i=0; i<AP.getNewPoints().size(); i++){
        Puntos.push_back(AP.getNewPoints()[i]);
        cout << AP.getPoints().size()+i << "-> " << AP.getNewPoints()[i] << "\n";
    }

    vector < vector<unsigned int>> T1 = AP.getFaces();
    for (long unsigned int i=0; i<AP.getFaces().size(); i++){ //evaluar cambio de nombre de getFaces a getElems
        VUI.push_back(AP.getFaces()[i]);
    }

    if (show_previous_mesh){
        //add previous elements to mesh
        cout << "Agregando caras y elementos no expandidos\n";
        for (int i=Whitelist_faces.size()-1; i >= 0;i--){
            if (Whitelist_faces[i] == 0){
                T1.insert(T1.begin(), VUI[i]);
            }
        }
    }
    
    cout << "T1 size: " << T1.size() << "\n";
    // for (unsigned int k=0; k <T1.size(); k++){
    //     cout << "[";
    //     for (unsigned int l=0; l<T1[k].size(); l++){
    //         cout << T1[k][l] << " ";
    //     }
    //     cout << "]\n";
    // }

    for (unsigned int PP=0; PP < cellTypes.size(); PP++){ cout << "cellTypes[" << PP << "] = " <<cellTypes[PP] << "\n";}

    //pruebas de output generado
    output.setElements(T1);
    output.setPoints(Puntos);

    /************************************************************************************/

    
    if (vtkformat || !oneout) {
        std::cout << "Esto imprime un output en vtk\n";
        // Services::WriteVTK(out_name, output);
        Services::WriteVTK(out_name, Puntos, T1, index_pivot, cellTypes);
        // Services::WriteVTK(out_name, Puntos, VUI);
        Services::WriteMeshGetfem(out_name, output);
    }

    // if (m3dfor) {
    //     // Services::WriteOutputMesh(out_name,output);
    //     std::cout << "Esto imprime un output en m2d\n"; 
    // }

    // if (mvmfor) {
    //     // Services::WriteMixedVolumeMesh(out_name,output);
    //     std::cout << "Esto imprime un output en mvm\n"; 
    // }

    // auto end_time = chrono::high_resolution_clock::now();
    // cout << "  All done in " << chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
    // cout << " ms"<< endl;
	
    //FIN CODIGO PROFE

    // MI CODIGO

    return 0;
}

