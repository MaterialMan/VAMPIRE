// File with all functions required for reservoir computing
// C++ standard library headers
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

// Vampire headers
#include "cells.hpp"
#include "micromagnetic.hpp"
#include "sim.hpp"
#include "errors.hpp"
#include "vio.hpp"
#include "units.hpp"
#include "errors.hpp"

// micromagnetic module headers
#include "internal.hpp"

namespace micromagnetic::reservoir{

  // vectors to store applied field and output magnetisation, over time steps and individual cells
  std::vector <std::vector <double>> source_field_cell;
  std::vector<double> input_field;
  std::vector<double> output_mag;
  bool reservoircheck; // is set to false if no reservoir input file is found, allowing vampire to continue normally

  bool cells_source_output;

  // vars needed for updating the field input and storing H for each cell at each timestep
  int fieldstate=-1;
  
  int timestepcounter=0;
  std::vector <std::vector <double>> Mag_cell_and_time;

  // reads data from sources input file
  void read_field(){

    // variables to store source field data
    std::vector <double> x;
    std::vector <double> y;
    //std::vector <double> ymin;
    //std::vector <double> ymax;
    std::vector <std::vector <double>> sourcefield;
    std::string line;

    std::cout << "Total sim time: " << sim::total_time << std::endl;
    std::cout << "Sim time-step: " << sim::partial_time << std::endl;

    std::ifstream file; 
    file.open("sourcefield.txt");

    if (file.is_open()){

      // find system input size
      getline(file, line); //read the first line of your file to string
      std::stringstream s;
      s << line; //send the line to the stringstream object...
      int how_many_columns = 0;
      double value;

      while (s >> value)
        how_many_columns++; //while there's something in the line, increase the number of columns

      std::cout << "Num of txt file columns: " << how_many_columns << std::endl;

      // resize the vector
      sourcefield.resize(how_many_columns);
      
      // variables to read file
      std::string line;
      std::vector <double> columns;

      while (std::getline(file, line)) // Read each line until end.
      {
        std::istringstream iss(line);

        double number;
        while (iss >> number) // separate string into double vector
          columns.push_back(number);

        //std::cout << columns[0] << std::endl

        if (columns[how_many_columns-1] != 0)
          {
            x.push_back(columns[0]);
            y.push_back(columns[1]);
            //xmax.push_back(columns[2]);
            //ymax.push_back(columns[3]); 
            
          }
        else{
          for (int i = 0; i < how_many_columns - 2; i++){
              sourcefield[i].push_back(columns[i+1]);
              //std::cout << i+1 << std::endl;
            }
        }
            
            // clear line details
        columns.clear();
      }
          
      file.close();

      // get the lengths of each side of the rectangular system
      /* double sidelength_x = 0;
      double sidelength_y = 0;
      for (int i=0; i < cells::pos_and_mom_array.size()/4; i++){
        if (cells::pos_and_mom_array[4*i+0]>sidelength_x){
          sidelength_x=cells::pos_and_mom_array[4*i+0];
        }
        if (cells::pos_and_mom_array[4*i+1]>sidelength_y){
          sidelength_y=cells::pos_and_mom_array[4*i+1];
        }
      }  */
      //std::cout << "xy: " << sidelength_x <<  sidelength_y << std::endl;

      // assign the appropiate external field to each cell, fields are additive over sources
     source_field_cell.resize(cells::num_cells, std::vector <double> (sourcefield[0].size(), 0));
      /* for (int i=0; i<cells::num_cells; ++i){
        for (int j=0; j<xmin.size(); ++j){
          if (xmin[j]*sidelength_x<= cells::pos_and_mom_array[4*i+0] && cells::pos_and_mom_array[4*i+0]<=xmax[j]*sidelength_x){
            if (ymin[j]*sidelength_y<= cells::pos_and_mom_array[4*i+1] && cells::pos_and_mom_array[4*i+1]<=ymax[j]*sidelength_y){
              std::transform (sourcefield[j].begin(), sourcefield[j].end(), source_field_cell[i].begin(), source_field_cell[i].begin(), std::plus<float>());
            
            
            }
          }
        }
      } */


      // assign the appropiate external field to each cell, fields are additive over sources
      for (int i=0; i<cells::num_cells; i++){
          int v = sqrt(cells::num_cells)*(x[i]-1) + y[i] -1;
          //std::cout << "cell: " << v  << i << std::endl;
          std::transform (sourcefield[i].begin(), sourcefield[i].end(), source_field_cell[v].begin(), source_field_cell[v].begin(), std::plus<float>());
          //std::cout << "cell: " << v << std::endl;
      }

  /*     std::cout << "input:";
        for (int i = 0; i < sourcefield.size(); i++){
          for (int j = 0; j < sourcefield[0].size(); j++){
            std::cout << ' ' << sourcefield[i][j];
            }
          std::cout << '\n';
        }  */

      std::cout << "Number of cells: " << cells::num_cells << std::endl;

      input_field.resize(cells::num_cells, 0);
      Mag_cell_and_time.resize(cells::num_cells, std::vector <double> (sim::total_time/sim::partial_time));
      output_mag.resize(cells::num_cells, 0);

      reservoircheck = true;
    } // end of file.is_open case

    else{ // input file not opened
      terminaltextcolor(RED);
      std::cout << "Field source file for reservoir computing was not found!" << std::endl;
      reservoircheck = false;
      input_field.resize(cells::num_cells, 0);
      terminaltextcolor(WHITE);
    }

    return;
  } // end of readfile function


  // function called whenever LLG is called (at each sim:timestep)
  void update_field(){
  
  if (reservoircheck==true){
    ++timestepcounter;

    if (timestepcounter==sim::partial_time){ // after (timestep) number of LLG calculations, update field and save magnetisation state
      
      ++fieldstate;
      timestepcounter=0;
      input_field.clear();

      for (int i=0; i<cells::num_cells; ++i){
        input_field.push_back(source_field_cell[i][fieldstate]);
        Mag_cell_and_time[i][fieldstate] = output_mag[i];
      }

      /* std::cout << "input:";
      for (int n = 0; n < input_field.size(); n++)
        std::cout << ' ' << input_field[n] ;
      std::cout << '\n'; */

      if (sim::cells_source_output == true) { // if plotting activated, make file with the external field data for each cell
        std::ofstream o;
        char buffer [100];
        sprintf(buffer, "cells_sources_output-%08i.txt", fieldstate);
        o.open(buffer);
        for (int i=0; i<cells::num_cells; ++i){
          o << source_field_cell[i][fieldstate] << std::endl;
        }
        o.close();
      }
    }
  }

    return;
  } // end of update_field function

  // outputs magnetisation data for each cell to .txt at each interval
  void output_field(){

    if (reservoircheck==true){ // write reservoir_output file 
      std::ofstream w;
      w.open("reservoir_output.txt");
      for (int i=0; i<Mag_cell_and_time[0].size() - 1; ++i){
        for (int j=0; j<cells::num_cells; ++j){
          w << Mag_cell_and_time[j][i+1] << " ";
        }
        w << std::endl;
      }
      w.close();
    }

    //legacy_cells_coords()

    return;
  } // end of output_field function

} // end of micromagnetic::reservoir namespace
