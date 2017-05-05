//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Richard F L Evans 2017. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

// C++ standard library headers
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

// program header
#include "vdc.hpp"

namespace vdc{

// forward function declarations

//------------------------------------------------------------------------------
// Function to output crystal.xyz file compatible with rasmol
//------------------------------------------------------------------------------
void output_inc_file(unsigned int spin_file_id){

   // Open Povray Include File
	std::stringstream incpov_file_sstr;
	incpov_file_sstr << "spins-";
	incpov_file_sstr << std::setfill('0') << std::setw(8) << spin_file_id;
	incpov_file_sstr << ".inc";
	std::string incpov_file = incpov_file_sstr.str();

   // output informative message to user
   std::cout << "   Writing povray file " << incpov_file << "..." << std::flush;

   // temporary variables defining spin colours
   double red=0.0, green=0.0, blue=1.0;

   // open incfile
   std::ofstream incfile;
   incfile.open(incpov_file.c_str());

   for(unsigned int atom = 0; atom < vdc::num_atoms; atom++){

      // calculate rgb components
      //vdc::rgb(red, green, blue);

      std::stringstream line;
      line << "spinm"<< type[atom] << "(" <<
              coordinates[3*atom+0] << "," << coordinates[3*atom+1] << "," << coordinates[3*atom+2] << "," <<
              spins[3*atom+0] << "," << spins[3*atom+1] << "," << spins[3*atom+2] << "," <<
              red << "," << green << "," << blue << ")\n";

      incfile << line.str();

   }

   incfile << std::flush;
   incfile.close();

   // output informative message to user
   if(vdc::verbose) std::cout << "done!" << std::endl;

   return;

}

//------------------------------------------------------------------------------
// Function to output crystal.xyz file compatible with rasmol
//------------------------------------------------------------------------------
void output_povray_file(){

	std::ofstream pfile;
	pfile.open("spins.pov");

   // Calculate location of camera
   double dim[3] = {vdc::system_size[0], vdc::system_size[1], vdc::system_size[2]};
   double vec[3];

	double size = sqrt(dim[0]*dim[0] + dim[1]*dim[1] + dim[2]*dim[2]);
	vec[0] = (1.0/dim[0]);
	vec[1] = (1.0/dim[1]);
	vec[2] = (1.0/dim[2]);
	double mag_vec = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	vec[0]/=mag_vec;
	vec[1]/=mag_vec;
	vec[2]/=mag_vec;

   pfile << "//-------------------------------------------------------------------------" << std::endl;
   pfile << "// Povray file generated using vampire" << std::endl;
   pfile << "//-------------------------------------------------------------------------" << std::endl;
	pfile << "#include \"colors.inc\"" << std::endl;
	pfile << "#include \"metals.inc\""	<< std::endl;
	pfile << "#include \"screen.inc\""	<< std::endl;
	pfile << "#declare LX=0.0;" << std::endl;
	pfile << "#declare LY=0.0;" << std::endl;
	pfile << "#declare LZ=0.0;" << std::endl;
	pfile << "#declare CX=" << size*vec[0]*6.0 << ";" << std::endl;
	pfile << "#declare CY=" << size*vec[1]*6.0 << ";" << std::endl;
	pfile << "#declare CZ=" << size*vec[2]*6.0 << ";" << std::endl;
	pfile << "#declare ref=0.5;" << std::endl;
	pfile << "global_settings { assumed_gamma 2.0 }" << std::endl;
	pfile << "background { color Gray30 }" << std::endl;

	pfile << "Set_Camera(<CX,CY,CZ>, <LX,LY,LZ>, 15)" << std::endl;
	pfile << "Set_Camera_Aspect(4,3)" << std::endl;
	pfile << "Set_Camera_Sky(<0,0,1>)" << std::endl;
	pfile << "light_source { <2*CX, 2*CY, 2*CZ> color White}" << std::endl;

   pfile << "#declare Initial_Frame = " << vdc::start_file_id << ";" << std::endl;
   pfile << "#declare Final_Frame = " << vdc::final_file_id << ";" << std::endl;

   // Output material specific macros
	for(int imat=0; imat < vdc::materials.size(); imat++){
		pfile << "#declare sscale"<< imat << "=2.0;" << std::endl;
		pfile << "#declare rscale"<< imat << "=1.2;" << std::endl;
		pfile << "#declare cscale"<< imat << "=3.54;" << std::endl;
		pfile << "#declare cones"<< imat << "=0;" << std::endl;
		pfile << "#declare arrows"<< imat << "=1;" << std::endl;
		pfile << "#declare spheres"<< imat << "=1;" << std::endl;
		pfile << "#declare cubes" << imat << "=0;" << std::endl;
		pfile << "#declare spincolors"<< imat << "=1;" << std::endl;
		pfile << "#declare spincolor"<< imat << "=pigment {color rgb < 0.1 0.1 0.1 >};" << std::endl;
		pfile << "#macro spinm"<< imat << "(cx,cy,cz,sx,sy,sz, cr,cg,cb)" << std::endl;
		pfile << "union{" << std::endl;
		pfile << "#if(spheres" << imat << ") sphere {<cx,cy,cz>,0.5*rscale"<< imat << "} #end" << std::endl;
		pfile << "#if(cubes" << imat << ") box {<cx-cscale"<< imat << "*0.5,cy-cscale" << imat << "*0.5,cz-cscale"<< imat << "*0.5>,<cx+cscale"<< imat << "*0.5,cy+cscale" << imat << "*0.5,cz+cscale"<< imat << "*0.5>} #end" << std::endl;
		pfile << "#if(cones"<< imat << ") cone {<cx+0.5*sx*sscale0,cy+0.5*sy*sscale"<< imat << ",cz+0.5*sz*sscale"<< imat << ">,0.0 <cx-0.5*sx*sscale"<< imat << ",cy-0.5*sy*sscale"<< imat << ",cz-0.5*sz*sscale"<< imat << ">,sscale0*0.5} #end" << std::endl;
		pfile << "#if(arrows" << imat << ") cylinder {<cx+sx*0.5*sscale"<< imat <<",cy+sy*0.5*sscale"<< imat <<",cz+sz*0.5*sscale"<< imat <<
					">,<cx-sx*0.5*sscale"<< imat <<",cy-sy*0.5*sscale"<< imat <<",cz-sz*0.5*sscale"<< imat <<">,sscale"<< imat <<"*0.12}";
		pfile << "cone {<cx+sx*0.5*1.6*sscale"<< imat <<",cy+sy*0.5*1.6*sscale"<< imat <<",cz+sz*0.5*1.6*sscale"<< imat <<">,sscale"<< imat <<"*0.0 <cx+sx*0.5*sscale"<< imat <<
					",cy+sy*0.5*sscale"<< imat <<",cz+sz*0.5*sscale"<< imat <<">,sscale"<< imat <<"*0.2} #end" << std::endl;
		pfile << "#if(spincolors"<< imat << ") texture { pigment {color rgb <cr cg cb>}finish {reflection {ref} diffuse 1 ambient 0}}" << std::endl;
		pfile << "#else texture { spincolor"<< imat << " finish {reflection {ref} diffuse 1 ambient 0}} #end" << std::endl;
		pfile << "}" << std::endl;
		pfile << "#end" << std::endl;

	}
   // frame specific povray output
	pfile << "#include concat(\"spins-\", str(frame_number, -8, 0) \".inc\")" << std::endl;

   // close output file
	pfile.close();

   return;

}

}