#include <fstream>
#include <iostream>

#include "image_ppm.hpp"
#include "seam_carver.hpp"

void PrintUsage(const std::string& command) {
  std::cout << "Usage: " << command << " INPUT_FILE [options] -o OUTPUT_FILE\n"
            << "Options:\n"
            << "\t-h times\tcarve times horizontal seams\n"
            << "\t-v times\tcarve times vertical seams\n";
}

int main(int argc, char* argv[]) {
  std::string out_option = "-o";
  if (argc < 4 || out_option != argv[argc - 2]) {
    PrintUsage(argv[0]);
    return 1;
  }

  std::ifstream infile(argv[1]);
  if (!infile.good()) {
    std::cout << "file '" << argv[1] << "' not found" << std::endl;
    return 1;
  }
  int height_ = 5, width_ = 5;
  int values[5][5] =
  {
    {0, 10, 30, 10, 16},
    {10, 28, 2, 28, 4},
    {30, 2, 4, 6, 0}, 
    {10, 28, 6, 12, 0}, 
    {6, 4, 0, 0, 0}
  };
  for (int row = 0; row < height_; row++) { //populate LAST col ONLY
    values[row][width_ - 1] = SeamCarver::GetEnergy(row, width_ - 1);
  }
  //bring it back 
   for (int col = width_ - 2; col >= 0; col--) {
    for (int row = 0; row < height_; row++) {
      //should be included min function
      int best = 0;
      if (row == 0) { //left edge
        best = std::min(values[row][col + 1], values[row + 1][col + 1]);
      } else if (row == height_ - 1) {
        best = std::min(values[row][col + 1], values[row - 1][col]);
      } else {
        best = std::min(std::min(values[row + 1][col + 1], values[row][col + 1]), values[row - 1][col + 1]);
      }
      values[row][col] = best + GetEnergy(row, col);
    }
  }
  int row_min = height_ - 1;
  int minimum = values[row_min][0];
  for (int row = height_ - 2; row >= 0; row--) {
    if (values[row][0] <= minimum) minimum = values[row][0], row_min = row;
  }
  return row_min;
  // values[0] = {0, 10, 30, 10, 16};
  // values[1] = {10, 28, 2, 28, 4};
  // values[2] = {30, 2, 4, 6, 0};
  // values[3] = {10, 28, 6, 12, 0};
  // values[4] = {6, 4, 0, 0, 0};
  


  /* UNCOMMENT COMMENTED CODE BELOW AS YOU IMPLEMENT */

  ImagePPM image;
  infile >> image;
  infile.close();

  SeamCarver carver( image );
  for ( int i = 2; i < argc - 2; i += 2 ) {
    std::string times_string = argv[i + 1];
    int times = 0;
    try {
      times = std::stoi( times_string );
    } catch ( const std::exception& e ) {
      std::cout << "Malformed option" << std::endl;
      PrintUsage( argv[0] );
      return 1;
    }

    std::string option = argv[i];
    if ( option == "-h" ) {
      for ( int t = 0; t < times; t++ ) {
        carver.RemoveHorizontalSeam();
      }
    } else if ( option == "-v" ) {
      for ( int t = 0; t < times; t++ ) {
        carver.RemoveVerticalSeam();
      }
    } else {
      std::cout << argv[i] << " not an option" << std::endl;
      PrintUsage( argv[0] );
      return 1;
    }
  }

  std::ofstream outfile( argv[argc - 1] );
  outfile << carver.GetImage();
  outfile.close();
}
