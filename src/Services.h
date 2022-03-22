/*
 <Mix-mesher: region type. This program generates a mixed-elements mesh>
 
 Copyright (C) <2013,2017>  <Claudio Lobos>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/gpl.txt>
 */

#ifndef Services_h
#define Services_h 1

#include <string.h>
#include "TriMesh.h"
#include "FEMesh.h"
#include "RefinementCubeRegion.h"
#include "RefinementSurfaceRegion.h"
#include "RefinementInputSurfaceRegion.h"
#include "RefinementAllRegion.h"
#include "MeshPoint.h"
#include "Octant.h"
#include "OctreeEdge.h"
#include "FaceContainer.h"
#include "Element.h"
#include "Hexahedron.h"
#include "Pyramid.h"
#include "PyramidBase5.h"
#include "Tetrahedron.h"
#include "Prism2.h"
#include "Visitors/EdgeVisitor.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <set>


using Clobscode::Point3D;
using Clobscode::TriMesh;
using Clobscode::MeshPoint;
using Clobscode::Octant;
using Clobscode::OctreeEdge;
using std::vector;
using std::string;
using std::set;

namespace Clobscode
{
	
	class Services {
        
        public:
        
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        
        static unsigned short readRefinementRegions(string name,
                                                    list<RefinementRegion *> &regions){
            
            char word [256];
            int cant;
            double x,y,z;
            unsigned short max_refinement = 0;
            
            vector<Point3D> cube_pts;
            
            FILE *file = fopen(name.c_str(),"r");
            
            if (file==NULL) {
                std::cout << "File " << name << " doesn't exist\n";
                return max_refinement;
            }
            
            //read number of nodes
            while(true){
                if(std::fscanf(file,"%s",word)==EOF){
                    fclose(file);
                    return max_refinement;
                }
                if(!strcmp(word,"n_regions\0"))
                    break;
            }
            std::fscanf(file,"%i",&cant);
            
            //read each node
            cube_pts.reserve(cant*2);
            
            for( int i=0;i<cant;i++){
                
                //Read region's first point
                std::fscanf(file,"%s",word);
                x = atof(word);
                std::fscanf(file,"%s",word);
                y = atof(word);
                std::fscanf(file,"%s",word);
                z = atof(word);
                Point3D p1 (x,y,z);
                
                //Read region's second point
                std::fscanf(file,"%s",word);
                x = atof(word);
                std::fscanf(file,"%s",word);
                y = atof(word);
                std::fscanf(file,"%s",word);
                z = atof(word);
                Point3D p2 (x,y,z);
                
                //Read Refinement Level
                unsigned short rrl = 0;
                std::fscanf(file,"%s",word);
                rrl = atoi(word);
                
                if (rrl>max_refinement) {
                    max_refinement = rrl;
                }
                
                RefinementRegion *rr = new RefinementCubeRegion(p1,p2,rrl);
                
                regions.push_back(rr);
            }
            
            fclose(file);
            
            return max_refinement;
        }
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        // static bool readSurfaceRefinementRegion(string name,
        //                                         list<RefinementRegion *> &regions,
        //                                         const unsigned short &rrl){
            
        //     vector<TriMesh> tmp;
        //     tmp.reserve(1);
        //     if (!ReadMdlMesh(name,tmp, )) {
        //         return false;
        //     }
        //     RefinementRegion *rr = new RefinementSurfaceRegion(tmp[0],rrl);
        //     regions.push_back(rr);
            
        //     return true;
        // }

        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static bool ReadOffMesh(string name,
                                vector<Clobscode::Point3D> &Points,
                                vector< vector < unsigned int>> &Faces){
            
            char word [256];
            int np, nf;
            double x,y,z;
            bool skel = false;
            // vector<vector<unsigned int> > allfaces;
            // vector<Point3D> tri_pts;
            
            FILE *file = fopen(name.c_str(),"r");
            
            if (file==NULL) {
                std::cout << "File " << name << " doesn't exist\n";
                fclose(file);
                return false;
            }
            
            while(true){
                if( fscanf(file,"%s",word) == EOF) {
                    fclose(file);
                    return false;
                }
                if (!strcmp(word,"")) {
                    continue;
                }
                if(!strcmp(word,"OFF\0"))
                    break;
                if(!strcmp(word,"SKEL\0")){
                    break;
                    skel = true;
                }
                fclose(file);
                return false;
            }
            
            //read number of points
            fscanf(file,"%i",&np);
            //read number of faces
            fscanf(file,"%i",&nf);
            
            if (!skel) {
                //read number of edges [info not needed].
                fscanf(file,"%s",word);
            }
            
            //read each node
            // tri_pts.reserve(np);
            Points.reserve(np);
            
            for( int i=0;i<np;i++){
                std::fscanf(file,"%s",word);
                x=atof(word);
                std::fscanf(file,"%s",word);
                y=atof(word);
                std::fscanf(file,"%s",word);
                z=atof(word);
                Point3D p (x,y,z);
                Points.push_back(p);
                fgets(word,256,file);
            }
            
            // allfaces.reserve(nf);
            Faces.reserve(np);
            //number of face points
            int nfp;
            for( int i=0;i<nf;i++){
                std::fscanf(file,"%i",&nfp);
                
                std::vector<unsigned int> fpts(nfp,0);
                for(unsigned int j=0;j<nfp;j++){
                    std::fscanf(file,"%i",&fpts[j]);
                }
                Faces.push_back(fpts);
                //read any other data in the line
                fgets(word,256,file);
            }
            fclose(file);
            
            // TriMesh tm (tri_pts, allfaces);
            // clobs_inputs.push_back(tm);
            
            return true;
        }
        
        //--------------------------------------------------------------------
        static bool readVtk(string name, vector<Point3D>&points, vector<vector< unsigned int>> &faces){
            
            char word [256];
            unsigned int cant,type;
            double x,y,z;
            vector < Element *> elements;
            FILE *file = fopen(name.c_str(),"r");
            
            while(true){
                if(!fscanf(file,"%s",word)){
                    fclose(file);
                    return false;
                }
                if(!strcmp(word,"UNSTRUCTURED_GRID\0"))
                    break;
            }
            
            //read word Points
            fscanf(file,"%s",word);
            
            fscanf(file,"%u",&cant);
            points.reserve(cant);
            
            //read word float
            fscanf(file,"%s",word);
            
            if(cant<=0){
                cerr << "warning: no nodes were found\n";
                fclose(file);
                return true;
            }
            
            //read the first point (to initialize the bounds)
            fscanf(file,"%s",word);
            x=atof(word);
            fscanf(file,"%s",word);
            y=atof(word);
            fscanf(file,"%s",word);
            z=atof(word);
            Point3D p (x,y,z);
            
            points.push_back(p);
            
            for(int i=1;i<cant;i++){
                fscanf(file,"%s",word);
                x=atof(word);
                fscanf(file,"%s",word);
                y=atof(word);
                fscanf(file,"%s",word);
                z=atof(word);
                Point3D p (x,y,z);
            
                points.push_back(p);
                
            }
            
            while(true){
                if(!fscanf(file,"%s",word)){
                    fclose(file);
                    return false;
                }
                if(!strcmp(word,"CELLS\0"))
                    break;
            }
            
            fscanf(file,"%u",&cant);
            elements.reserve(cant);
            fscanf(file,"%s",word);
            
            unsigned int idx;
            
            for(int i=0;i<cant;i++){
                
                Element *element;
                fscanf(file,"%s",word);
                if (word[0]=='8') {
                    vector<unsigned int> epts;
                    epts.reserve(8);
                    for(int j=0;j<8;j++){
                        fscanf(file,"%u",&idx);
                        epts.push_back(idx);
                    }
                    element = new Hexahedron(epts);
                }
                else if(word[0]=='4'){
                    vector<unsigned int> epts;
                    epts.reserve(4);
                    for(int j=0;j<4;j++){
                        fscanf(file,"%u",&idx);
                        epts.push_back(idx);
                    }
                    element = new Tetrahedron(epts);
                }
                else if(word[0]=='5'){
                    vector<unsigned int> epts;
                    epts.reserve(5);
                    for(int j=0;j<5;j++){
                        fscanf(file,"%u",&idx);
                        epts.push_back(idx);
                    }
                    element = new Pyramid(epts);
                }
                else if(word[0]=='6'){
                    vector<unsigned int> epts;
                    epts.reserve(6);
                    for(int j=0;j<6;j++){
                        fscanf(file,"%u",&idx);
                        epts.push_back(idx);
                    }
                    element = new Prism(epts);
                }
                else{
                    continue;
                }
                
                elements.push_back(element);
            }
            
            fclose(file);

            FaceContainer FC;
            //save faces with neigbhoring information
            for(unsigned int i=0; i<elements.size(); i++){
                //An EnhancedElement is an Element with optimizations for
                //removing and visualizing.
                elements[i]->computeBbox(points);
                for(int j=0;j<elements[i]->numberOfFaces();j++){
                    
                    vector<unsigned int> fpts = elements[i]->getFacePoints(j);
                    Face face(fpts);
                    int fid = FC.addFace(face);
                    FC.getFace(fid).addElement(i);
                    //add the face to the EnhancedElement
                    elements[i]->addFace(fid);
                }
            }
            //REVISAR!!!!
            int cantidad_caras = 0, cantidad_superficiales = 0;
            for (int fidx=0; fidx < FC.getFacesVec().size(); fidx++){
                
                if(FC.getFace(fidx).numberOfElements()==1){
                    // std::cout << fidx << ":" << FC.getFace(fidx).numberOfElements() << "\n";
                    cantidad_superficiales++;
                    faces.push_back(FC.getFace(fidx).getPoints());
                }
                cantidad_caras++;
            }
            std::cout << "Cantidad de caras de la malla: " << cantidad_caras << "\n";
            std::cout << "Cantidad de caras superficiales: " << cantidad_superficiales << "\n";
            return true;
        }


		static bool ReadMdlMesh(std::string name,
								  vector<Point3D> &pts,
                                  vector<vector< unsigned int>> &faces){

			char word [256];
			int cant;
			double x,y,z;
			vector<vector<unsigned int> > allfaces;
			// vector<Point3D> tri_pts;
			
			FILE *file = fopen(name.c_str(),"r");
			
			if (file==NULL) {
				std::cout << "File " << name << " doesn't exist\n";
				return false;
			}
			
			//read number of nodes
			while(true){
				if(std::fscanf(file,"%s",word)==EOF){
					fclose(file);
					return false;
				}
				if(!strcmp(word,"ARRAY1<POINT3D>]\0"))
					break;
			}
			std::fscanf(file,"%i",&cant);
			
			if(cant<=0)
				return false;
			//read each node
			pts.reserve(cant);
			
			for( int i=0;i<cant;i++){
				std::fscanf(file,"%s",word);
				x=atof(word);
				std::fscanf(file,"%s",word);
				y=atof(word);
				std::fscanf(file,"%s",word);
				z=atof(word);
				Point3D p (x,y,z);
				pts.push_back(p);
			}
			
			//read number of "triangle faces"
			cant = 0;
			while(1){
				if(std::fscanf(file,"%s",word) == EOF){
					std::cout << "didn't find faces\n";
					fclose(file);
					return false;
				}
				
				if(!strcmp(word,"ARRAY1<STRING>]\0")){
					//std::fscanf(file,"%s",word);
					std::fscanf(file,"%i",&cant);
					break;
				}
			}
			
			faces.reserve(cant);
			//read each face (assuming they are triangles
			int dust;
			for( int i=0;i<cant;i++){
				std::vector<unsigned int> fpts(3,0);
				for(unsigned int j=0;j<3;j++){
					std::fscanf(file,"%i",&fpts[j]);
				}
				//read some unnecessary integers
				for(unsigned int j=0;j<3;j++)
					std::fscanf(file,"%i",&dust);
				
				faces.push_back(fpts);
			}
			fclose(file);
						
			// TriMesh tm (pts, faces);
			// clobs_inputs.push_back(tm);
			
			return true;
		}
        
        // //-------------------------------------------------------------------
        // //-------------------------------------------------------------------
        // static bool ReadOctantList(std::string name, list<unsigned int> &olist,
        //                            vector<unsigned int> &ele_oct_ref) {
        //     FILE *file = fopen(name.c_str(),"r");
        //     int idx = 0;
        //     char word [256];
        //     while (std::fscanf(file,"%s",word) != EOF) {
        //         bool num = true;
        //         for (unsigned int i=0; word[i]!= '\0'; i++) {
        //             if (!isdigit(word[i])) {
        //                 num = false;
        //                 break;
        //             }
        //         }
        //         if(num) {
        //             idx = atoi(word);
        //             if (ele_oct_ref.size()<=idx) {
        //                 cerr << "Invalid element index while reading list of elements to refine\n";
        //                 cerr << "Octree mesh must be readed before the list is provided\n";
        //                 cerr << "Abortig!!!\n";
        //                 std:abort();
        //             }
                
        //             olist.push_back(ele_oct_ref[idx]);
        //         }
        //     }
        //     olist.sort();
        //     olist.unique();
        //     fclose(file);
        //     return true;
        // }
        
        
        
        // //-------------------------------------------------------------------
        // //-------------------------------------------------------------------
        // static bool ReadOctreeMesh(std::string name, vector<MeshPoint> &points,
        //                            vector<Octant> &octants,
        //                            set<OctreeEdge> &edges,
        //                            vector<unsigned int> &ele_oct_ref,
        //                            GeometricTransform &gt,
        //                            unsigned short &minrl,
        //                            unsigned short &maxrl) {
            
        //     char word [256];
        //     double x,y,z;
        //     int e1,e2,e3,elem;
        //     int np=0, ne=0, no=0, nl=0;
            
        //     FILE *file = fopen(name.c_str(),"r");
            
        //     if (file==NULL) {
        //         std::cout << "File " << name << " doesn't exist\n";
        //         return false;
        //     }
            
        //     //read header
        //     std::fscanf(file,"%i",&np);
        //     std::fscanf(file,"%i",&ne);
        //     std::fscanf(file,"%i",&no);
        //     std::fscanf(file,"%i",&nl);
            
        //     //read each node
        //     points.reserve(np);

        //     for(unsigned int i=0;i<np;i++){
        //         std::fscanf(file,"%s",word);
        //         x=atof(word);
        //         std::fscanf(file,"%s",word);
        //         y=atof(word);
        //         std::fscanf(file,"%s",word);
        //         z=atof(word);
        //         Point3D p (x,y,z);
        //         points.push_back(p);
        //     }
            
        //     //read edges
        //     for(unsigned int i=0;i<ne;i++){
        //         std::fscanf(file,"%i",&e1);
        //         std::fscanf(file,"%i",&e2);
        //         std::fscanf(file,"%i",&e3);
        //         OctreeEdge oe(e1,e2);
        //         unsigned int mid = (unsigned int)e3;
        //         oe.setMidPoint(mid);
        //         edges.insert(oe);
        //     }
            
        //     //read the element octant link
        //     ele_oct_ref.reserve(nl);
        //     unsigned int checksum = 0;
        //     for (unsigned int i=0; i<no; i++) {
        //         std::fscanf(file,"%i",&elem);
        //         for (unsigned int j=0; j<elem; j++) {
        //             ele_oct_ref.push_back(i);
        //         }
        //     }
            
        //     //read the octants, its refinement level and
        //     //the input faces intersected by it.
        //     octants.reserve(no);
        //     int nop = 0, nof = 0, orl = 0, ni=0;
            
        //     minrl = 100;
        //     maxrl = 0;
            
        //     unsigned int noregular=0;
        //     for (unsigned int i=0; i<no; i++) {
        //         vector<unsigned int> opts;
        //         list<unsigned int> ofcs;
        //         std::fscanf(file,"%s",word);
        //         nop = atoi(word);
        //         opts.reserve(nop);
                
        //         if (nop!=8) {
        //             cerr << "warning at Services::ReadOctreeMesh\n";
        //             cerr << "         Octant hasn't 8 nodes: " << nop << "\n";
        //             cout << "octant index " << i << "\n";
        //             continue;
        //         }
                
        //         for (unsigned int j=0; j<nop; j++) {
        //             std::fscanf(file,"%i",&ni);
        //             opts.push_back(ni);
        //         }
        //         std::fscanf(file,"%i",&orl);
        //         std::fscanf(file,"%i",&nof);
        //         for (unsigned int j=0; j<nof; j++) {
        //             std::fscanf(file,"%i",&ni);
        //             ofcs.push_back(ni);
        //         }
        //         Octant octant (opts,orl);
        //         octant.setIntersectedFaces(ofcs);
        //         octants.push_back(octant);
                
        //         if (orl<minrl) {
        //             minrl = short(orl);
        //         }
        //         if (orl>maxrl) {
        //             maxrl = short(orl);
        //         }
        //     }
            
        //     std::fscanf(file,"%s %s",word,word);
        //     std::fscanf(file,"%s",word);
        //     x=atof(word);
        //     std::fscanf(file,"%s",word);
        //     y=atof(word);
        //     std::fscanf(file,"%s",word);
        //     z=atof(word);
        //     Point3D c(x,y,z);
        //     std::fscanf(file,"%s",word);
        //     x=atof(word);
        //     std::fscanf(file,"%s",word);
        //     y=atof(word);
        //     std::fscanf(file,"%s",word);
        //     z=atof(word);
        //     gt.setCentroid(c);
        //     gt.setXAxis(x);
        //     gt.setYAxis(y);
        //     gt.setZAxis(z);
            
		// 	fclose(file);
        //     return true;
        // }
    
        // //-------------------------------------------------------------------
        // //-------------------------------------------------------------------
        // static bool WriteOctreeMesh(std::string name, vector<MeshPoint> &points,
        //                             vector<Octant> &octants,
        //                             set<OctreeEdge> &edges,
        //                             const unsigned int &nels,
        //                             GeometricTransform &gt){
            
        //     OctreeEdge oe;
        //     set<OctreeEdge>::iterator my_edge;
            
        //     string vol_name = name+".oct";
            
        //     //write the volume mesh
        //     FILE *f = fopen(vol_name.c_str(),"wt");
        //     unsigned int np = points.size();
        //     unsigned int no = octants.size();
        //     unsigned int ne = edges.size();
            
        //     fprintf(f,"%u %u %u %u\n\n", np, ne, no, nels);

        //     //write points
        //     for(unsigned int i=0;i<np;i++){
        //         Point3D p = points[i].getPoint();
        //         fprintf(f,"%+1.8E  %+1.8E  %+1.8E\n",p[0],p[1],p[2]);
        //     }
        //     fprintf(f,"\n");
            
        //     //write edeges
        //     for(my_edge=edges.begin();my_edge!=edges.end();my_edge++){
        //         OctreeEdge me = *my_edge;
        //         fprintf(f,"%i %i %i\n",me[0],me[1],me[2]);
        //     }
        //     fprintf(f,"\n");
            
        //     //pair sub-elements with octant index.
        //     //this info is printed per octant and the elements are
        //     //printed in order in the mesh file so we can compute
        //     //for each element to which octant it belongs.
        //     for (unsigned int i=0; i<octants.size(); i++) {
        //         unsigned int nse = octants[i].getSubElements().size();
        //         fprintf(f,"%u ",nse);
        //     }
        //     fprintf(f,"\n\n");
            
        //     for (unsigned int i=0; i<octants.size(); i++) {
        //         vector<unsigned int> opts = octants[i].getPoints();
        //         unsigned int nopts = opts.size();
        //         if (nopts<8) {
        //             cerr << "warning at Services::WriteOctreeMesh\n";
        //             cerr << "        Octant has less than 8 nodes\n";
        //             fprintf(f,"%u ",nopts);
        //         }
        //         else {
        //             nopts=8;
        //             fprintf(f,"8 ");
        //         }
        //         for (unsigned int j=0; j<nopts; j++) {
        //             fprintf(f,"%u ",opts[j]);
        //         }
        //         fprintf(f,"%u\n",octants[i].getRefinementLevel());
        //         list<unsigned int> ofcs = octants[i].getIntersectedFaces();
        //         list<unsigned int>::iterator fiter;
        //         nopts = ofcs.size();
        //         fprintf(f,"%u",nopts);
        //         for (fiter=ofcs.begin(); fiter!=ofcs.end(); fiter++) {
        //             fprintf(f," %u",*fiter);
        //         }
        //         fprintf(f,"\n");
        //     }
            
        //     fprintf(f,"\nGeometric Transform\n");
        //     Point3D c = gt.getCentroid();
        //     fprintf(f,"%f %f %f\n",c[0],c[1],c[2]);
        //     fprintf(f,"%f %f %f\n",gt.getXAxis(),gt.getYAxis(),gt.getZAxis());
            
        //     fclose(f);
            
        //     return true;
        // }
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static bool WritePLY(std::string name, FEMesh &output){
            
            vector<Point3D> points = output.getPoints();
            vector<vector<unsigned int> > elements = output.getElements(), elFcs;
            elFcs.reserve(elements.size());
            
            FaceContainer fcs;
            
            for (auto eNds:elements) {
                vector<vector<unsigned int> > elFcsIdx = getFaces(eNds);
                vector<unsigned int> elemFId;
                elemFId.reserve(elFcsIdx.size());
                for (auto &eF:elFcsIdx) {
                    Face f(eF);
                    elemFId.push_back(fcs.addFace(f));
                }
                elFcs.push_back(elemFId);
            }
            
            string vol_name = name+".ply";
            
            //write the volume mesh
            FILE *f = fopen(vol_name.c_str(),"wt");
            unsigned int np = points.size();
            unsigned int nc = elements.size();
            unsigned int nf = fcs.length();
            
            fprintf(f,"# the format to read\nply\nformat ascii 1.0\n\n");
            fprintf(f,"# total number of element vertices\n");
            fprintf(f,"element vertex %i\n",np);
            fprintf(f,"property float x\nproperty float y\nproperty float z\n\n");
            fprintf(f,"# total number of faces\nelement face %i\n",nf);
            fprintf(f,"property list uchar int vertex_indices\n\n");
            fprintf(f,"# total number of cells/polyhedra\nelement cell %i\n",nc);
            fprintf(f,"property list uchar int face_indices\nend_header\n");
            fprintf(f,"# XYZ coordinates\n");
            
            
            //write points
            for(unsigned int i=0;i<np;i++){
                Point3D p = points[i];
                fprintf(f,"%f  %f  %f\n",p[0],p[1],p[2]);
            }
            //write faces
            fprintf(f,"\n# face idenfiers\n");
            for (unsigned int i=0; i<nf; i++) {
                vector<unsigned int> fIdx = fcs.getFace(i).getPoints();
                fprintf(f,"%i",(int)fIdx.size());
                for (auto id:fIdx) {
                    fprintf(f," %i",id);
                }
                fprintf(f,"\n");
            }
            //write elements face indexes
            fprintf(f,"\n# polyhedra idenfication\n");
            for (auto ef:elFcs) {
                fprintf(f,"%i",(int)ef.size());
                for (auto id:ef) {
                    fprintf(f," %i",id);
                }
                fprintf(f,"\n");
            }
            
            return true;
        }
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static bool WriteMixedVolumeMesh(std::string name, FEMesh &output){
            
            vector<Point3D> points = output.getPoints();
            vector<vector<unsigned int> > elements = output.getElements();
            
            if (elements.empty()) {
                std::cout << "no output elements\n";
                return false;
            }
            
            string vol_name = name+".mvm";
            
            //write the volume mesh
            FILE *f = fopen(vol_name.c_str(),"wt");
            
            unsigned int n = points.size();
            
            fprintf(f,"MIXED\n%i %i\n\n",(int)points.size(),(int)elements.size());
            
            //write points
            for(unsigned int i=0;i<points.size();i++){
                fprintf(f,"%+1.8E",points[i][0]);
                fprintf(f," %+1.8E",points[i][1]);
                fprintf(f," %+1.8E\n",points[i][2]);
            }
            
            //get all the elements in a std::vector
            for (unsigned int i=0; i<elements.size(); i++) {
                std::vector<unsigned int> epts = elements[i];
                unsigned int np = epts.size();
                if (np == 4) {
                    fprintf(f,"T");
                }
                else if (np == 5){
                    fprintf(f,"P");
                }
                else if (np == 6){
                    fprintf(f,"R");
                }
                else if (np == 8){
                    fprintf(f,"H");
                }
                
                for (unsigned int j= 0; j<np; j++) {
                    fprintf(f," %i", epts.at(j));
                }
                
                fprintf(f,"\n");
            }
            fclose(f);
            
            return true;
        }
        
		//-------------------------------------------------------------------
		//-------------------------------------------------------------------
		static bool WriteOutputMesh(std::string name, FEMesh &output){
						
			vector<Point3D> points = output.getPoints();
			vector<vector<unsigned int> > elements = output.getElements();
			
			if (elements.empty()) {
				std::cout << "no output elements\n";
				return false;
			}
			
			string vol_name = name+".m3d";
			
			//write the volume mesh
			FILE *f = fopen(vol_name.c_str(),"wt");
			
			unsigned int n = points.size();
			
			fprintf(f,"%s\n","[Nodes, ARRAY1<STRING>]");
			fprintf(f,"%i\n\n",n);
			
			//write points
			for(unsigned int i=0;i<n;i++){			
				fprintf(f,"1 %+1.8E",points[i][0]);
				fprintf(f," %+1.8E",points[i][1]);
				fprintf(f," %+1.8E\n",points[i][2]);
			}
			
			n = elements.size();
			
			fprintf(f,"\n%s\n","[Elements, ARRAY1<STRING>]");
			fprintf(f,"%i\n\n",n);
			
			//get all the elements in a std::vector
			for (unsigned int i=0; i<n; i++) {
				std::vector<unsigned int> epts = elements[i];
				unsigned int np = epts.size();
				if (np == 4) {
					fprintf(f,"T");
				}
				else if (np == 5){
					fprintf(f,"P");
				}
				else if (np == 6){
					fprintf(f,"R");
				}
				else if (np == 8){
					fprintf(f,"H");
				}
				
				for (unsigned int j= 0; j<np; j++) {
					fprintf(f," %i", epts.at(j));
				}
				
				fprintf(f," 1000.0 0.45 1.0\n");
			}
			fclose(f);
			
			return true;
		}
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static bool WriteVTK(std::string name, vector<Point3D> &points, vector<vector<unsigned int> > &elements){
            
            // vector<Point3D> points = output.getPoints();
            // vector<vector<unsigned int> > elements = output.getElements();
            
            if (elements.empty()) {
                std::cout << "no output elements\n";
                return false;
            }
            
            string vol_name = name+".vtk";
            
            //write the volume mesh
            FILE *f = fopen(vol_name.c_str(),"wt");
            
            fprintf(f,"# vtk DataFile Version 2.0\nUnstructured Grid %s\nASCII",name.c_str());
            fprintf(f,"\n\nDATASET UNSTRUCTURED_GRID\nPoints %i float",(int)points.size());
            
            //write points
            for(unsigned int i=0;i<points.size();i++){
                if (i%2==0) {
                    fprintf(f,"\n");
                }
                fprintf(f," %+1.8E",points[i][0]);
                fprintf(f," %+1.8E",points[i][1]);
                fprintf(f," %+1.8E",points[i][2]);
            }
            
            //count conectivity index.
            unsigned int conectivity = 0;
            for (unsigned int i=0; i<elements.size(); i++) {
                conectivity+=elements[i].size()+1;
            }
            
            fprintf(f,"\n\nCELLS %i %i\n",(int)elements.size(),conectivity);
            
            //get all the elements in a std::vector
            for (unsigned int i=0; i<elements.size(); i++) {
                std::vector<unsigned int> epts = elements[i];
                unsigned int np = epts.size();
                fprintf(f,"%i", np);
                
                if (np==6) {
                    unsigned int aux = epts[1];
                    epts[1] = epts[2];
                    epts[2] = aux;
                    aux = epts[4];
                    epts[4] = epts[5];
                    epts[5] = aux;
                }
                
                for (unsigned int j= 0; j<np; j++) {
                    fprintf(f," %i", epts.at(j));
                }
                
                fprintf(f,"\n");
            }
            
            fprintf(f,"\nCELL_TYPES %i\n",(int)elements.size());
            for (unsigned int i=0; i<elements.size(); i++) {
                unsigned int np = elements[i].size();
                if (np == 4) {
                    fprintf(f,"10\n");
                }
                else if (np == 5){
                    fprintf(f,"14\n");
                }
                else if (np == 6){
                    fprintf(f,"13\n");
                }
                else if (np == 8){
                    fprintf(f,"12\n");
                }
            }
            
            fclose(f);
            
            return true;
        }
        
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static bool WriteMeshGetfem(std::string name, FEMesh &output){
            
            vector<Point3D> points = output.getPoints();
            vector<vector<unsigned int> > elements = output.getElements();
            
            if (elements.empty()) {
                std::cout << "no output elements\n";
                return false;
            }
            
            string vol_name = name+".gmf";
            
            //write the volume mesh
            FILE *f = fopen(vol_name.c_str(),"wt");
            
            unsigned int n = points.size();
            
            fprintf(f,"%s\n%s\n\n\n\n","% GETFEM MESH FILE","% GETFEM VERSION 5.1");
            fprintf(f,"%s\n\n","BEGIN POINTS LIST");
            
            //write points
            for(unsigned int i=0;i<n;i++){
                fprintf(f,"  POINT  %i %+1.8E",i,points[i][0]);
                fprintf(f," %+1.8E",points[i][1]);
                fprintf(f," %+1.8E\n",points[i][2]);
            }
            
            n = elements.size();
            
            fprintf(f,"\n%s\n\n\n\n","END POINTS LIST");
            fprintf(f,"%s\n\n","BEGIN MESH STRUCTURE DESCRIPTION");
            
            unsigned int aux;
            
            //get all the elements in a std::vector
            for (unsigned int i=0; i<n; i++) {
                std::vector<unsigned int> epts = elements[i];
                unsigned int np = epts.size();
                
                //if (np==8 || np==4) {
                    fprintf(f,"CONVEX %i    ",i);
                //}

                
                if (np == 4) {
                    fprintf(f,"'GT_PK(3,1)'      ");
                    std::vector<unsigned int> vaux (4,0);
                    aux = epts[1];
                    epts[1] = epts[2];
                    epts[2] = aux;
                }
                else if (np == 5){
                    fprintf(f,"'GT_PYRAMID(1)'      ");
                    std::vector<unsigned int> vaux (5,0);
                    aux = epts[2];
                    epts[2] = epts[3];
                    epts[3] = aux;
                }
                else if (np == 6){
                    fprintf(f,"'GT_PRISM(3,1)'      ");
                    aux = epts[1];
                }
                else if (np == 8){
                    fprintf(f,"'GT_QK(3,1)'      ");
                    std::vector<unsigned int> vaux (8,0);
                    vaux[0] = epts[0];
                    vaux[1] = epts[3];
                    vaux[2] = epts[4];
                    vaux[3] = epts[7];
                    vaux[4] = epts[1];
                    vaux[5] = epts[2];
                    vaux[6] = epts[5];
                    vaux[7] = epts[6];
                    epts = vaux;
                }
                
                //if (np==8 || np==4) {
                    for (unsigned int j= 0; j<np; j++) {
                        fprintf(f," %i", epts.at(j));
                    }
                
                    fprintf(f,"\n");
                //}
            }
            
            fprintf(f,"\n%s\n","END MESH STRUCTURE DESCRIPTION");
            fclose(f);
            
            return true;
        }
        
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------
        static vector<vector<unsigned int> > getFaces(const vector<unsigned int> &idxs){
            
            vector<vector<unsigned int> > faces;
            vector<unsigned int> f(4,0),ft(3,0);
            
            switch (idxs.size()) {
                case 8:
                    faces.reserve(6);
                    f[0]=idxs[0];
                    f[1]=idxs[3];
                    f[2]=idxs[2];
                    f[3]=idxs[1];
                    faces.push_back(f);
                    f[0]=idxs[0];
                    f[1]=idxs[4];
                    f[2]=idxs[5];
                    f[3]=idxs[1];
                    faces.push_back(f);
                    f[0]=idxs[1];
                    f[1]=idxs[2];
                    f[2]=idxs[6];
                    f[3]=idxs[5];
                    faces.push_back(f);
                    f[0]=idxs[2];
                    f[1]=idxs[3];
                    f[2]=idxs[7];
                    f[3]=idxs[6];
                    faces.push_back(f);
                    f[0]=idxs[0];
                    f[1]=idxs[4];
                    f[2]=idxs[7];
                    f[3]=idxs[3];
                    faces.push_back(f);
                    f[0]=idxs[4];
                    f[1]=idxs[5];
                    f[2]=idxs[6];
                    f[3]=idxs[7];
                    faces.push_back(f);
                    return faces;
                case 6:
                    faces.reserve(5);
                    f[0]=idxs[0];
                    f[1]=idxs[1];
                    f[2]=idxs[4];
                    f[3]=idxs[3];
                    faces.push_back(f);
                    f[0]=idxs[1];
                    f[1]=idxs[2];
                    f[2]=idxs[5];
                    f[3]=idxs[4];
                    faces.push_back(f);
                    f[0]=idxs[0];
                    f[1]=idxs[3];
                    f[2]=idxs[5];
                    f[3]=idxs[2];
                    faces.push_back(f);
                    ft[0]=idxs[0];
                    ft[1]=idxs[2];
                    ft[2]=idxs[1];
                    faces.push_back(ft);
                    ft[0]=idxs[3];
                    ft[1]=idxs[4];
                    ft[2]=idxs[5];
                    faces.push_back(ft);
                    return faces;
                case 5:
                    faces.reserve(5);
                    f[0]=idxs[0];
                    f[1]=idxs[3];
                    f[2]=idxs[2];
                    f[3]=idxs[1];
                    faces.push_back(f);
                    ft[0]=idxs[0];
                    ft[1]=idxs[4];
                    ft[2]=idxs[1];
                    faces.push_back(ft);
                    ft[0]=idxs[1];
                    ft[1]=idxs[2];
                    ft[2]=idxs[4];
                    faces.push_back(ft);
                    ft[0]=idxs[2];
                    ft[1]=idxs[3];
                    ft[2]=idxs[4];
                    faces.push_back(ft);
                    ft[0]=idxs[3];
                    ft[1]=idxs[0];
                    ft[2]=idxs[4];
                    faces.push_back(ft);
                    return faces;
                case 4:
                    faces.reserve(5);
                    ft[0]=idxs[0];
                    ft[1]=idxs[2];
                    ft[2]=idxs[1];
                    faces.push_back(ft);
                    ft[0]=idxs[0];
                    ft[1]=idxs[1];
                    ft[2]=idxs[3];
                    faces.push_back(ft);
                    ft[0]=idxs[1];
                    ft[1]=idxs[2];
                    ft[2]=idxs[3];
                    faces.push_back(ft);
                    ft[0]=idxs[2];
                    ft[1]=idxs[0];
                    ft[2]=idxs[3];
                    faces.push_back(ft);
                    return faces;
                default:
                    break;
            }
            
            return faces;
        }
	};
}
#endif
