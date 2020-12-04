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
  // std::vector<double> input_field; //prev version
  std::vector <std::vector <double>> input_field;
  
// for Z-direction only
  //std::vector<double> output_mag;
  // all three dimensions
  std::vector <std::vector <double>> output_mag;

  bool reservoircheck; // is set to false if no reservoir input file is found, allowing vampire to continue normally

  bool cells_source_output;

  // vars needed for updating the field input and storing H for each cell at each timestep
  int fieldstate=-1;
  
  int timestepcounter=0;

  // store H
  //std::vector <std::vector <double>> Mag_cell_and_time;
  std::vector <std::vector <double>> Mag_cell_and_time_x;
  std::vector <std::vector <double>> Mag_cell_and_time_y;
  std::vector <std::vector <double>> Mag_cell_and_time_z;

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

      int how_many_columns = cells::num_cells+1;

      // find system input size
      /*getline(file, line); //read the first line of your file to string
      std::stringstream s;
      s << line; //send the line to the stringstream object...
      int how_many_columns = 0;
      double value;

      while (s >> value)
        how_many_columns++; //while there's something in the line, increase the number of columns
*/
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


        //std::cout << columns[how_many_columns-1] << '\n';
        
        // get data and add to vectors containing coords and data points
        if (columns[how_many_columns-1] < 0)
          {
            if (columns[how_many_columns-1] != -1){
              for (int i = 0; i < how_many_columns - 1; i++){
                x.push_back(columns[i]);
                std::cout << x[i] << ' '; 
              }
              std::cout << columns[how_many_columns-1] <<'\n';
            }
            else{
              for (int i = 0; i < how_many_columns - 1; i++){
                y.push_back(columns[i]);
                std::cout << y[i] << ' '; 
              }
              std::cout << columns[how_many_columns-1] <<'\n';
            }   
          }
        else{
          for (int i = 0; i < how_many_columns - 1; i++){
              sourcefield[i].push_back(columns[i]);
              std::cout << columns[i] << ' '; 
            }
            std::cout << columns[how_many_columns-1] <<'\n';
        }
            
            // clear line details
        columns.clear();
      }
          
      file.close();

      // assign the appropiate external field to each cell, fields are additive over sources
     source_field_cell.resize(cells::num_cells, std::vector <double> (sourcefield[0].size(), 0));

      // assign the appropiate external field to each cell, fields are additive over sources
      for (int i=0; i<cells::num_cells; i++){
          int v = sqrt(cells::num_cells)*(x[i]-1) + y[i] -1;
          std::transform (sourcefield[i].begin(), sourcefield[i].end(), source_field_cell[v].begin(), source_field_cell[v].begin(), std::plus<float>());
      }

      std::cout << "Number of cells: " << cells::num_cells << std::endl;

	int dimensions = 3;
      //input_field.resize(cells::num_cells, 0);
      input_field.resize(cells::num_cells,std::vector <double> (dimensions));

      // store H
      //Mag_cell_and_time.resize(cells::num_cells, std::vector <double> (sim::total_time/sim::partial_time));
      Mag_cell_and_time_x.resize(cells::num_cells, std::vector <double> (sim::total_time/sim::partial_time));
      Mag_cell_and_time_y.resize(cells::num_cells, std::vector <double> (sim::total_time/sim::partial_time));
      Mag_cell_and_time_z.resize(cells::num_cells, std::vector <double> (sim::total_time/sim::partial_time));
      
      //output_mag.resize(cells::num_cells, 0); // Z only
	output_mag.resize(cells::num_cells,  std::vector <double> (dimensions)); // all three directions
      

      reservoircheck = true;
    } // end of file.is_open case

    else{ // input file not opened
      terminaltextcolor(RED);
      std::cout << "Field source file for reservoir computing was not found!" << std::endl;
      reservoircheck = false;
      //input_field.resize(cells::num_cells, 0);
	input_field.resize(cells::num_cells, std::vector <double> (dimensions));
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

	// update input to reservoir
        // input_field.push_back(source_field_cell[i][fieldstate]); // for Z only
	input_field[i][0]=source_field_cell[i][fieldstate]); 
	input_field[i][1]=source_field_cell[i][fieldstate]);
	input_field[i][2]=source_field_cell[i][fieldstate]);

        //update output
        Mag_cell_and_time_x[i][fieldstate] = output_mag[i][0];
        Mag_cell_and_time_y[i][fieldstate] = output_mag[i][1];
        Mag_cell_and_time_z[i][fieldstate] = output_mag[i][2];
      }

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

      /*for (int i=0; i<Mag_cell_and_time[0].size() - 1; ++i){
        for (int j=0; j<cells::num_cells; ++j){
          w << Mag_cell_and_time[j][i+1] << " ";
        }
        w << std::endl;
      }*/


      // store x
      for (int i=0; i<Mag_cell_and_time_x[0].size() - 1; ++i){
        for (int j=0; j<cells::num_cells; ++j){
          w << Mag_cell_and_time_x[j][i+1] << " ";
        }
        w << std::endl;
      }

      // store y
      for (int i=0; i<Mag_cell_and_time_y[0].size() - 1; ++i){
        for (int j=0; j<cells::num_cells; ++j){
          w << Mag_cell_and_time_y[j][i+1] << " ";
        }
        w << std::endl;
      }

      // store z
      for (int i=0; i<Mag_cell_and_time_z[0].size() - 1; ++i){
        for (int j=0; j<cells::num_cells; ++j){
          w << Mag_cell_and_time_z[j][i+1] << " ";
        }
        w << std::endl;
      }

      w.close();
    }

    //legacy_cells_coords()

    return;
  } // end of output_field function

} // end of micromagnetic::reservoir namespace
