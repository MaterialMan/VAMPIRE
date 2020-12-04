//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Sarah Jenkins 2018. All rights reserved.
//
//   Email: sarah.jenkins@york.ac.uk
//
//------------------------------------------------------------------------------
//

#ifndef HIERARCHICAL_H_
#define HIERARCHICAL_H_

// C++ standard library headers
#include <string>

// Vampire headers
#include "hierarchical.hpp"

//--------------------------------------------------------------------------------
// Namespace for variables and functions for hierarchical module
//--------------------------------------------------------------------------------
namespace hierarchical{

   //-----------------------------------------------------------------------------
   // Function to initialise hierarchical module
   //-----------------------------------------------------------------------------
   void initialize(double system_dimensions_x, double system_dimensions_y, double system_dimensions_z);

   //---------------------------------------------------------------------------
   // Function to process input file parameters for hierarchical module
   //---------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line);

   //---------------------------------------------------------------------------
   // Function to process material parameters
   //---------------------------------------------------------------------------
   bool match_material_parameter(std::string const word, std::string const value, std::string const unit, int const line, int const super_index, const int sub_index);

} // end of hierarchical namespace

#endif //HIERARCHICAL_H_
