/*
 * =====================================================================================
 *
 *       Filename:  meshParameterize.h  Version:  1.0  Created:  01/11/2015 11:10:44 AM
 *
 *    Description:  meshParameterize 
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2015, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/parameterize.h>
#include <CGAL/Square_border_parameterizer_3.h>
#include <CGAL/LSCM_parameterizer_3.h>
#include <CGAL/Parameterization_polyhedron_adaptor_3.h>
#include <CGAL/Parameterization_mesh_patch_3.h>
#include <list>
#include <cstring>

#include "Polyhedron_ex.h"
#include "Mesh_cutter.h"
#include "Parameterization_polyhedron_adaptor_ex.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Parameterization_polyhedron_adaptor_3<Polyhedron> Parameterization_polyhedron_adaptor;
typedef std::list<Parameterization_polyhedron_adaptor::Vertex_handle> Seam;
typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;


struct MeshParamterization
{
	MeshParamterization(std::string mesh_name, int s=0, int len=10);
	~MeshParamterization();
	TriMesh mesh;
	Polyhedron cgalMesh;
	Seam seam;
	//offName is for cgal to read
	std::string meshName, offName;
	int CUT_LENGTH, start;
	// cut mesh for parameterize
	void cutMesh();
	void parameterize();
};
