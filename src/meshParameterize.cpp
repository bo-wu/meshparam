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
#include <CGAL/Timer.h>
#include <CGAL/Mean_value_coordinates_parameterizer_3.h>
#include <CGAL/Two_vertices_parameterizer_3.h>
#include <CGAL/Barycentric_mapping_parameterizer_3.h>
#include <CGAL/Discrete_conformal_map_parameterizer_3.h>
#include <CGAL/LSCM_parameterizer_3.h>
#include <CGAL/Discrete_authalic_parameterizer_3.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include "meshParameterize.h"

typedef CGAL::Parameterization_mesh_feature_extractor<Parameterization_polyhedron_adaptor_ex> Mesh_feature_extractor;
typedef CGAL::Parameterization_mesh_patch_3<Parameterization_polyhedron_adaptor> Mesh_patch_polyhedron;
typedef Mesh_cutter::Backbone  Backbone;
typedef CGAL::Parameterizer_traits_3<Mesh_patch_polyhedron> Parameterizer;

MeshParamterization::MeshParamterization(std::string mesh_name, int s, int len)
{
	meshName = mesh_name;
	CUT_LENGTH = len;
	start = s;
//	OpenMesh::IO::Options ropt;
//	ropt += OpenMesh::IO::Options::VertexNormal;
	if(!OpenMesh::IO::read_mesh(mesh, meshName))
	{
		std::cerr<<"openmesh canot read mesh "<< meshName<<std::endl;
		exit(-1);
	}

	//write mesh for cgal to read
	if(meshName.substr(meshName.length()-3, meshName.length()) == "obj")
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
		std::cerr <<"cgal cannot read mesh "<<offName<<std::endl;
		exit(-1);
	}
}

void MeshParamterization::parameterize()
{
	CGAL::Timer total_timer, task_timer;
	total_timer.start();
	task_timer.start();
	cutMesh();
	Parameterization_polyhedron_adaptor mesh_adaptor(cgalMesh);
	if(seam.empty())
	{
		std::cerr<<"input mesh not supported: the cutting algorithm is too simple to cut this shape\n";
		return;
	}
	Mesh_patch_polyhedron mesh_patch(mesh_adaptor, seam.begin(), seam.end());
	if(!mesh_patch.is_valid())
	{
		std::cerr<<"input mesh not supported: non manifold shape or invalid cutting\n";
		return;
	}
	std::cerr<<"cutting mesh use "<<task_timer.time()<<" seconds\n";
	task_timer.reset();
	//Parameterizer::Error_code err = CGAL::parameterize(mesh_patch);
	//Parameterizer::Error_code err = CGAL::parameterize(mesh_patch, CGAL::Discrete_authalic_parameterizer_3<Mesh_patch_polyhedron, CGAL::Square_border_arc_length_parameterizer_3<Mesh_patch_polyhedron> >());
	Parameterizer::Error_code err = CGAL::parameterize(mesh_patch, CGAL::Barycentric_mapping_parameterizer_3<Mesh_patch_polyhedron, CGAL::Square_border_arc_length_parameterizer_3<Mesh_patch_polyhedron> >());

	switch(err)
	{
		case Parameterizer::OK:
			break;
		case Parameterizer::ERROR_EMPTY_MESH:
		case Parameterizer::ERROR_NON_TRIANGULAR_MESH:
		case Parameterizer::ERROR_NO_TOPOLOGICAL_DISC:
		case Parameterizer::ERROR_BORDER_TOO_SHORT:
			std::cerr<<"Input mesh not supported: "<<Parameterizer::get_error_message(err)<<std::endl;
		default:
			std::cerr<<"Error: " << Parameterizer::get_error_message(err)<<std::endl;
			return ;
			break;
	}

	Polyhedron::Vertex_const_iterator pVertex;
	TriMesh::VertexIter v_it = mesh.vertices_begin();
	OpenMesh::Vec3f pos;
	for(pVertex = cgalMesh.vertices_begin(); pVertex!=cgalMesh.vertices_end() && v_it!=mesh.vertices_end(); pVertex++, ++v_it)
	{
		pos[0] = mesh_adaptor.get_vertex_uv(pVertex).x();
		pos[1] = mesh_adaptor.get_vertex_uv(pVertex).y();
		pos[2] = 0.0;
		mesh.point(*v_it) = pos;
	}
	
	std::string outName = meshName.substr(0, meshName.length()-4) + "_param.obj";
	OpenMesh::IO::write_mesh(mesh, outName);
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
	else  // if not a topological disk, virtual cut 
	{
		Backbone seamingBackbone;
		Backbone::iterator he;

		cgalMesh.compute_facet_centers();
		Mesh_cutter cutter(cgalMesh);
		if(genus == 0)
		{
			// no border, cut the mesh
			assert(nb_borders == 0);
			cutter.cut(seamingBackbone);
		}
		else  // genus > 0, has holes, cut the mesh
		{
			cutter.cut_genus(seamingBackbone);
		}
		
		if(seamingBackbone.begin() == seamingBackbone.end())
			return;
		cgalMesh.tag_halfedges(0);
		for(he=seamingBackbone.begin(); he!=seamingBackbone.end(); he++)
		{
			//get next halfedge iterator (looping)
			Backbone::iterator next_he = he;
			next_he++;
			if(next_he == seamingBackbone.end())
				next_he = seamingBackbone.begin();
			if((*he)->vertex() != (*next_he)->opposite()->vertex())
				return;
			(*he)->tag( (*he)->tag()+1 );
		}

		for(he=seamingBackbone.begin(); he!=seamingBackbone.end(); he++)
		{
			// counter of halfedge and opposite halfedge must be 1
			if((*he)->tag() != 1 || (*he)->opposite()->tag() != 1)
				return;
		}
		for(he=seamingBackbone.begin(); he!=seamingBackbone.end(); he++)
			seam.push_back((*he)->vertex());
		
		/*  
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
			*/
	}
}

