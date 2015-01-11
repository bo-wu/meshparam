/*
 * =====================================================================================
 *
 *       Filename:  meshParameterize.cpp  Version:  1.0  Created:  01/11/2015 11:10:53 AM
 *
 *    Description:  meshParameterize function
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2015, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */
#include <CGAL/IO/Polyhedron_iostream.h>
#include <iostream>
#include <fstream>
#include "meshParameterize.h"

typedef CGAL::Parameterization_mesh_feature_extractor<Parameterization_polyhedron_adaptor> Mesh_feature_extractor;
typedef CGAL::Parameterization_mesh_patch_3<Parameterization_polyhedron_adaptor> Mesh_patch_polyhedron;

MeshParamterization::MeshParamterization(std::string mesh_name, int s, int len)
{
	meshName = mesh_name;
	CUT_LENGTH = len;
	start = s;
//	OpenMesh::IO::Options ropt;
//	ropt += OpenMesh::IO::Options::VertexNormal;
	if(!OpenMesh::IO::read_mesh(mesh, meshName))
	{
		std::cerr<<"canot read mesh "<< meshName<<std::endl;
		exit(-1);
	}

	//write mesh for cgal to read
	if(meshName.substr(meshName.length()-3, meshName.length()) != "obj")
	{
		offName = meshName.substr(0, meshName.length()-3) + "off";
		OpenMesh::IO::write_mesh(mesh, offName);
	}
	else
	{
		offName = meshName;
	}

	std::ifstream stream(offName.c_str());
	stream >> cgalMesh;
	if(!stream || !cgalMesh.is_valid() || cgalMesh.empty())
	{
		std::cerr <<"cannot read mesh "<<offName<<std::endl;
		exit(-1);
	}
}

void MeshParamterization::parameterize()
{
	
}


void MeshParamterization::cutMesh()
{
	Parameterization_polyhedron_adaptor mesh_adaptor(cgalMesh);
	Mesh_feature_extractor feature_extractor(mesh_adaptor);
	int nb_borders = feature_extractor.get_nb_borders();
	int genus = feature_extractor.get_genus();

	if(genus == 0 && nb_borders > 0)
	{
		seam = feature_extractor.get_longest_border();
	}
	else  // virtual cut 
	{
		Polyhedron::Halfedge_handle seam_halfedges[CUT_LENGTH];
		seam_halfedges[0] = cgalMesh.halfedges_begin();
		if (seam_halfedges[0] == NULL)
			return;
		for(int i=1; i<CUT_LENGTH; ++i)
		{
			seam_halfedges[i] = seam_halfedges[i-1]->next()->opposite()->next();
			if(seam_halfedges[i] == NULL)
				return;
		}
		for(int i=0; i<CUT_LENGTH; ++i)
			seam.push_back(seam_halfedges[i]->vertex());
		for(int i=CUT_LENGTH-1; i>=0; --i)
			seam.push_back(seam_halfedges[i]->opposite()->vertex());
	}
}

