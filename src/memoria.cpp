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
#include <string>
#include <cctype>
#include <time.h>
#include <chrono>

using std::atoi;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
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

//-------------------------------------------------------------------
//-------------------------------------------------------------------

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

    vector<Clobscode::TriMesh> inputs;
    inputs.reserve(4);
    //Clobscode::Services io;
    
    bool getfem=false, vtkformat=false, octant_start=false;
    bool m3dfor=false, mvmfor=false, oneout=false, plyfor=false;
    
    //Default values for options
    int distance_between_layers=1, layers_qty=1;
    string faces_whitelist = "";
    bool distance_given=false, layers_given=false, faces_whitelist_given=false;

    //for reading an octant mesh as starting point.
    vector<MeshPoint> oct_points;
    vector<Octant> oct_octants;
    set<OctreeEdge> oct_edges;
    vector<unsigned int> oct_ele_link;
    GeometricTransform gt;
    
	for (unsigned int i=1; i<argc; i++) {
        
		if (argv[i][0]!='-') { //si la opcion no empieza con guion, entonces es invalida
			cout << "Error: expected option -X and got " << argv[i] << "\n";
			endMsg();
			return 0;
		}
        
        bool inout = false;
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
                if (!Services::ReadMdlMesh(in_name,inputs)) {
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
            case 'V':
                in_name = argv[i+1];
                
                if (!Services::readVtk(in_name, Puntos, VUI)) {
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

            case 'f': //para archivos .oct
                faces_whitelist = argv[i+1];
                faces_whitelist_given = true;

                i++;
                break;
            default:
                cerr << "Warning: unknown option " << argv[i] << " skipping\n";
                break;
        }

    }
    
    if (!in_name_given) {
        cerr << "No input domain surface mesh provided. Aborting\n";
        return 0;
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
    // Clobscode::FEMesh output;

    // POR VERIFICAR ESTO
    vector <Face> FVector;
    for (int i=0;i<VUI.size(); i++){
        Face Ftemp(VUI[i]);
        FVector.push_back(Ftemp);
        std:cout << "Face\t" << i << ":\t";
        for (int j=0; j<FVector[i].getPoints().size(); j++){
            std::cout << FVector[i].getPoints()[j] << "\t";
        };
        std::cout << "\n";
    }

    std::cout << "Numero de Nodos: " << Puntos.size() << "\n";
    std::cout << "Numero de caras: " << FVector.size() << "\n";
    
    // if (getfem) {
    //     // Services::WriteMeshGetfem(out_name,output);
    //     std::cout << "Esto imprime un output en gmf\n"; 
    // }

    // if (vtkformat || !oneout) {
    //     std::cout << "Esto imprime un output en vtk\n";
        
    //     for (int trimesh_index=0; trimesh_index< inputs.size(); trimesh_index++){
    //         // vector <Point3D> Puntos = inputs[trimesh_index].getPoints();
    //         // vector <vector <unsigned int>> VUI = inputs[trimesh_index].getFaces().getPoints(); //ver como capturar el vector de caras
            
    //         //ESTO NO ESTA DEL TODO PENSADO------------------------------------
    //         vector <Face> FVector;

    //         cout << "Nodes:\n";
    //         for (int i=0; i<Puntos.size(); i++){
    //             cout << i << " (" << Puntos[i] << ")\n";
    //         }

    //         cout << "\nFaces:\n";
    //         for (int i=0; i<VUI.size(); i++){
    //             Face TempFace(VUI[i]);
    //             FVector.push_back(TempFace);
    //             cout << "{ ";
    //             for (int j=0; j < TempFace.getPoints().size(); j++){
    //                 cout << TempFace.getPoints()[j] << "\t";
    //             }
    //             cout << "}\n";
                
    //         }
    //         cout << "\n\nResultado Calculo de normales:\n";
    //         vector <NodeProjection> NodeProjectionV;
    //         //Funcion que inicializa un objeto del tipo NodeProjection
    //         for (int i=0; i<Puntos.size(); i++){
    //             NodeProjection NP(i, Puntos[i], FVector);
    //             //funcion que calcula la normal acumulada de las caras que involucran al nodo NP
    //             NP.CalcPreNormal(Puntos);
    //             //Funcion que normaliza el valor en la normal acumulada de las caras que involucran al nodo NP
    //             NP.Normalize();
    //             //Funcion de utilidad que imprime toda la informacion en el objeto NP
    //             NP.print();
    //             NodeProjectionV.push_back(NP);
    //         }
    //         //---------------------------------------------------------------
    //         //Aun no esta funcionando esto, es una idea

    //         output.setElements(VUI);
    //         output.setPoints(Puntos);
    //         // Services::WriteVTK(out_name,output);
    //     }
        
    // }

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
    // Point3D a(-1,0,-1);
    // Point3D b(-1,0,1);
    // Point3D c(1,0,1);
    // Point3D d(1,0,-1);
    // Point3D e(2,1,0);
    // vector <Point3D> Puntos = {a,b,c,d,e};

    // vector <vector <unsigned int>> VUI = {{0,1,2,3}, {3,2,4}};

    // vector <Face> FVector;

    // cout << "Nodes:\n";
    // for (int i=0; i<Puntos.size(); i++){
    //     cout << i << " (" << Puntos[i] << ")\n";
    // }

    // cout << "\nFaces:\n";
    // for (int i=0; i<VUI.size(); i++){
    //     Face TempFace(VUI[i]);
    //     FVector.push_back(TempFace);
    //     cout << "{ ";
    //     for (int j=0; j < TempFace.getPoints().size(); j++){
    //         cout << TempFace.getPoints()[j] << "\t";
    //     }
    //     cout << "}\n";
        
    // }
    // cout << "\n\nResultado Calculo de normales:\n";
    // vector <NodeProjection> NodeProjectionV;
    // //Funcion que inicializa un objeto del tipo NodeProjection
    // for (int i=0; i<Puntos.size(); i++){
    //     NodeProjection NP(i, Puntos[i], FVector);
    //     //funcion que calcula la normal acumulada de las caras que involucran al nodo NP
    //     NP.CalcPreNormal(Puntos);
    //     //Funcion que normaliza el valor en la normal acumulada de las caras que involucran al nodo NP
    //     NP.Normalize();
    //     //Funcion de utilidad que imprime toda la informacion en el objeto NP
    //     NP.print();
    //     NodeProjectionV.push_back(NP);
    // }
    

   
    
    return 0;
}