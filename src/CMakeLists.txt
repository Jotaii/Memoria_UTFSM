cmake_minimum_required(VERSION 2.8.12)
project(node_exp)

#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
# Update if necessary
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES Main 
Point3D 
Hexahedron 
Prism
Prism2
Pyramid 
Tetrahedron 
Element 
Quality
Metric
MetricJens
MetricJR
math/Matrix 
math/Polynom 
math/PolyMatrix 
math/Vector
)


set(SOURCE_FILES MeshPoint Point3D TriMesh Octant OctreeEdge HexRotation
        SurfTemplate1 SurfTemplate2 SurfTemplate3 SurfTemplate4
        SurfTemplate5 SurfTemplate6 SurfTemplate7
        Mesher GridMesher TransitionPoint PatternFactory TransitionTemplate
        TransitionCube SurfPair SurfEdge SurfEdgeContainer SurfTriangle
        RefinementRegion RefinementCubeRegion RefinementSurfaceRegion
        RefinementInputSurfaceRegion RefinementAllRegion
        FaceDiagonal SurfPrism SurfPyramid SurfHexahedron SurfTetrahedron 
	SurfaceEdge GeometricTransform Face FaceContainer 
        Element Hexahedron Pyramid PyramidBase5 Tetrahedron Prism2
        NodeProjection NormalRepair AdvancingPoint NewPointRef memoria 
        Visitors/Visitor Visitors/SplitVisitor Visitors/EdgeVisitor 
	Visitors/IntersectionsVisitor
        Visitors/OneIrregularVisitor Visitors/PointMovedVisitor
        Visitors/TransitionPatternVisitor Visitors/SurfaceTemplatesVisitor
        Visitors/RemoveSubElementsVisitor)


add_executable(node_exp ${SOURCE_FILES}) 
