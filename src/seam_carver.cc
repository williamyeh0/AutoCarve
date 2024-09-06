#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here

const ImagePPM& SeamCarver::GetImage() const {
  return image_;
}

int SeamCarver::GetHeight() const {
  return height_;
}

int SeamCarver::GetWidth() const {
  return width_;
}

int SeamCarver::GetEnergy(int row, int col) const {

  //check above if not at row 0, check below if not at row 1, buncha if else ifs

  int rabove = (row > 0) ? image_.GetPixel(row - 1, col).GetRed() : image_.GetPixel(image_.GetHeight() - 1, col).GetRed();
  int rbelow = (row < image_.GetHeight() - 1) ? image_.GetPixel(row + 1, col).GetRed() : image_.GetPixel(0, col).GetRed();
  int gabove = (row > 0) ? image_.GetPixel(row - 1, col).GetGreen() : image_.GetPixel(image_.GetHeight() - 1, col).GetGreen();
  int gbelow = (row < image_.GetHeight() - 1) ? image_.GetPixel(row + 1, col).GetGreen() : image_.GetPixel(0, col).GetGreen();
  int babove = (row > 0) ? image_.GetPixel(row - 1, col).GetBlue() : image_.GetPixel(image_.GetHeight() - 1, col).GetBlue();
  int bbelow = (row < image_.GetHeight() - 1) ? image_.GetPixel(row + 1, col).GetBlue() : image_.GetPixel(0, col).GetBlue();

  int rleft = (col > 0) ? image_.GetPixel(row, col - 1).GetRed() : image_.GetPixel(row, image_.GetWidth() - 1).GetRed();
  int rright = (col < image_.GetWidth() - 1) ? image_.GetPixel(row, col + 1).GetRed() : image_.GetPixel(row, 0).GetRed();
  int gleft = (col > 0) ? image_.GetPixel(row, col - 1).GetGreen() : image_.GetPixel(row, image_.GetWidth() - 1).GetGreen();
  int gright = (col < image_.GetWidth() - 1) ? image_.GetPixel(row, col + 1).GetGreen() : image_.GetPixel(row, 0).GetGreen();
  int bleft = (col > 0) ? image_.GetPixel(row, col - 1).GetBlue() : image_.GetPixel(row, image_.GetWidth() - 1).GetBlue();
  int bright = (col < image_.GetWidth() - 1) ? image_.GetPixel(row, col + 1).GetBlue() : image_.GetPixel(row, 0).GetBlue();

  int rvertdiff = std::abs(rabove - rbelow), rhordiff = std::abs(rleft - rright), gvertdiff = std::abs(gabove - gbelow), ghordiff = std::abs(gleft - gright), bvertdiff = std::abs(babove - bbelow), bhordiff = std::abs(bleft - bright);
  int vertical = rvertdiff*rvertdiff + gvertdiff*gvertdiff + bvertdiff*bvertdiff;
  int horizontal = rhordiff*rhordiff + ghordiff*ghordiff + bhordiff*bhordiff;
  return vertical + horizontal; 
}

int SeamCarver::VerticalSeamEnergy(int** values) const { //vertical btw 
  for (int col = 0; col < width_; col++) { //populate LAST ROW ONLY
    values[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }
  //bring it back 
   for (int row = height_ - 2; row >= 0; row--) {
    for (int col = 0; col < width_; col++) {
      //should be included min function
      int best = 0;
      if (col == 0) { //left edge
        best = std::min(values[row + 1][col], values[row + 1][col + 1]);
      } else if (col == width_ - 1) {
        best = std::min(values[row + 1][col - 1], values[row + 1][col]);
      } else {
        best = std::min(std::min(values[row + 1][col - 1], values[row + 1][col]), values[row + 1][col + 1]);
      }
      values[row][col] = best + GetEnergy(row, col);
    }
  }
  int column = width_ - 1;
  int minimum = values[0][column];
  for (int col = width_ - 2; col >= 0; col--) {
    if (values[0][col] <= minimum) minimum = values[0][col], column = col;
  }
  return column;
}

int* SeamCarver::GetVerticalSeam() const { //vertical btw 
  int** values = new int*[height_];
  for (int i = 0; i < height_; i++) { //give arrays 
    values[i] = new int[width_];
  }
  int* seam = new int[height_]; //one for each row buddy so height
  seam[0] = VerticalSeamEnergy(values);
  //high key the same procedure, check min of values array ig of the 3 below 
  //make sure to add it to the seam 
  for (int row = 0; row < height_ - 1; row++) { 
    int col = seam[row]; //the previous term hopefully 
    int best_column_index = col;
    //look at the pixels right below 
    //middle > left > right
    if (col == 0) {
      //check row + 1 and row + 1, col + 1, track col index 
      if (values[row + 1][col] > values[row + 1][col + 1]) {
        best_column_index = col + 1; //default to middle which is col = 0
      }
    } else if (col == width_ - 1) { //no error checking if size is like 2 or smth
      if (values[row + 1][col - 1] < values[row + 1][col]) best_column_index = col - 1;
    } else {
      if (values[row + 1][col - 1] < values[row + 1][best_column_index]) { //left
        best_column_index = col - 1;
      } 
      if (values[row + 1][col + 1] < values[row + 1][best_column_index]) { //right
        best_column_index = col + 1;
      }
    }
    seam[row + 1] = best_column_index;
    }
    for (int i = 0; i < height_; i++) {
      delete[] values[i];
    }
    delete[] values;
    values = nullptr;
    return seam;
}
//middle > top > bottom
//start at first col, same direction as in mountain paths 
//which means start from the last col with HSE
int SeamCarver::HorizontalSeamEnergy(int** values) const { //vertical btw 
  for (int row = 0; row < height_; row++) { //populate LAST col ONLY
    values[row][width_ - 1] = GetEnergy(row, width_ - 1);
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
}

int* SeamCarver::GetHorizontalSeam() const {
  int** values = new int*[height_];
  for (int i = 0; i < height_; i++) { //give arrays 
    values[i] = new int[width_];
  }
  int* seam = new int[width_]; //one for each col buddy so width
  seam[0] = HorizontalSeamEnergy(values);
  //high key the same procedure, check min of values array ig of the 3 below 
  //make sure to add it to the seam 
  for (int col = 0; col < width_ - 1; col++) { 
    int row = seam[col]; //the previous term hopefully 
    int best_row_index = row; //default to middle
    //look at the pixels right below 
    //middle > top > bottom
    if (row == 0) {
      //check row + 1 and row + 1, col + 1, track col index 
      if (values[row][col + 1] > values[row + 1][col + 1]) {
        best_row_index = row + 1; //default to middle which is col = 0
      }
    } else if (row == height_ - 1) { //no error checking if size is like 2 or smth
      if (values[row - 1][col + 1] < values[row][col + 1]) best_row_index = row - 1;
    } else {
      if (values[row - 1][col + 1] < values[best_row_index][col + 1]) { //top
        best_row_index = row - 1;
      } 
      if (values[row + 1][col + 1] < values[best_row_index][col + 1]) { //bottom
        best_row_index = row + 1;
      }
    }
    seam[col + 1] = best_row_index;
    }
    for (int i = 0; i < height_; i++) {
      delete[] values[i];
    }
    delete[] values;
    values = nullptr;
    return seam;
}

void SeamCarver::RemoveHorizontalSeam() {
  image_.RemoveHorizontalSeam(GetHorizontalSeam());
  height_--;
} 
//delete seam upon finding it 
void SeamCarver::RemoveVerticalSeam() {
  image_.RemoveVerticalSeam(GetVerticalSeam());
  width_--;
}

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}