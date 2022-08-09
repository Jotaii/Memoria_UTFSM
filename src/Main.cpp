#include "TriMesh.h"
#include "Services.h"
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
using Clobscode::Point3D;
using Clobscode::Services;

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void endMsg(){
	cout << "use: ./node_exp [-V|-o] inputFile [-u] outputName [options]";
	cout << "where:\n";
	cout << "  one of the parameters must be an input surface mesh in\n";
    cout << "  vtk or off format.\nOptions:\n";
	cout << "    -l <int> Set quantity of layers for expansion.\n";
    cout << "    -e <float> Change distance between layers of expansion.\n";
	cout << "    -x <float> Change the distance ratio between layers of expansion.\n";
    cout << "    -f <filePath> Select faces to be consider for expansion. The file\n";
    cout << "                  must contain faces index, one by line.";
    cout << "    -k Include original input mesh in output mesh.\n";
}


int main(int argc,char** argv){
    
    if (argc<5) {
        endMsg();
        return 0;
    }
    
	string in_name = "", out_name = "";
	bool out_name_given = false, in_name_given = false;
    

    //inputs
    vector <Point3D> Puntos;
    vector <vector <unsigned int>> VUI, OriginalMesh;
    vector <unsigned int> cellTypes;

    vector<Clobscode::TriMesh> inputs;
    inputs.reserve(4);
    //Clobscode::Services io;
    
    bool getfem=false, vtkformat=false, octant_start=false;
    bool m3dfor=false, mvmfor=false, oneout=false, plyfor=false;
    
    //Default values for options
    int distance_between_layers, layers_qty;
    unsigned int index_pivot = 0;
    string faces_whitelist;
    bool distance_given=false, layers_given=false, faces_whitelist_given=false, distance_multiplier_given=false, show_previous_mesh=false;
    float distance_multiplier;

    
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
            case 'k': //si -k es entregado, se muestra el mesh no expandido
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
            
            // Not implemented for mdl files
            // case 'd': //para archivos .mdl
            //     in_name = argv[i+1];
            //     if (!Services::ReadMdlMesh(in_name,Puntos, VUI)) {
            //         std::cerr << "couldn't read file " << argv[i+1] << std::endl;
            //         return 1;
            //     }
            //     in_name_given = true;
            //     i++;
            //     break;

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
                if (!Services::readVtk(in_name, Puntos, VUI, OriginalMesh, cellTypes)) {
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

    
    if (faces_whitelist_given){
        std::ifstream indata;
        unsigned int num; 
        indata.open(faces_whitelist); 
        if(!indata) { 
            cerr << "Error: El archivo " << faces_whitelist << " no puede ser abierto o no existe" << endl;
            exit(1);
        }
        indata >> num;
        while ( !indata.eof() ) { // keep reading until end-of-file
            if (num < VUI.size()){
                Whitelist_faces.at(num) = 1;
                index_pivot++;
            }
            indata >> num; // sets EOF flag if no value found
        }
        indata.close();        
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
    	
	
    auto start_time = chrono::high_resolution_clock::now();

    Clobscode::FEMesh output;

    /***************************** Codigo que genera las mallas nuevas **********************************/
    AdvancingPoint AP(Puntos, VUI, distance_between_layers, layers_qty, Whitelist_faces, distance_multiplier, faces_whitelist_given);    
    bool IsVolMesh = false;
    for(unsigned int i=0; i < cellTypes.size();i++){
        if (cellTypes[i] >= 9){
            IsVolMesh = true;
            break;
        }
    }

    //agregar nuevos puntos y elementos a los arreglos
    for (long unsigned int i=0; i<AP.getNewPoints().size(); i++){
        Puntos.push_back(AP.getNewPoints()[i]);
    }

    vector < vector<unsigned int>> T1 = AP.getFaces();
    for (long unsigned int i=0; i<AP.getFaces().size(); i++){
        VUI.push_back(AP.getFaces()[i]);
    }
    
    //Si el parametro -k fue ingresado, se debe agregar la malla original
    if (show_previous_mesh && IsVolMesh == false){
        for (int i=Whitelist_faces.size()-1; i >= 0;i--){
            if (Whitelist_faces[i] == 0){
                T1.insert(T1.begin(), VUI[i]);
            }
        }
    }
    else if (show_previous_mesh && IsVolMesh == true){
        for(unsigned int V=OriginalMesh.size(); V>0; V--){
            T1.insert(T1.begin(), OriginalMesh[V-1]);
        }        
    }
    

    else if(show_previous_mesh==false && IsVolMesh == true){
        cellTypes = {};
    }
    
    //Output generado
    output.setElements(T1);
    output.setPoints(Puntos);

    if (vtkformat || !oneout) {
        Services::WriteVTK(out_name, Puntos, T1, index_pivot, cellTypes, IsVolMesh);
        cout << "Output generado satisfactoriamente.\n";
    }

    auto end_time = chrono::high_resolution_clock::now();
    cout << "  All done in " << chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
    cout << " ms"<< endl;
    return 0;
}

