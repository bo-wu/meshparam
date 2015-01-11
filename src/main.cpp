/*
 * =====================================================================================
 *
 *       Filename:  main.cpp  Version:  1.0  Created:  01/11/2015 11:07:47 AM
 *
 *    Description:  main function for parameterizing a mesh
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2015, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */
#include "meshParameterize.h"

int main(int argc, char** argv)
{
	std::string mesh_name = argv[1];
	MeshParamterization mp(mesh_name);
	mp.parameterize();
	return 0;
}
