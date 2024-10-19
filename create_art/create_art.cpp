#include "create_art.h"

void SaveImage(SandPileParameters& sp, SandpileOptions& options, int img_count) {
  char file_path[StringLen(std::to_string(img_count).c_str()) + 4 + StringLen(options.save_path)];
  strcpy(file_path, options.save_path);
  strcat(file_path, std::to_string(img_count).c_str());
  strcat(file_path, ".bmp");
  std::ofstream file(file_path, std::ios::out | std::ios::binary);
  uint16_t width = sp.to_x - sp.from_x;
  uint16_t height = sp.to_y - sp.from_y;
  const uint64_t padding = (8 - (width % 8)) % 8;
  const uint64_t full_width = width + padding;

  const uint64_t fileSize = kFileHeaderSize + kInformationHeaderSize + kColorPalleteSize + (full_width * height) / 2;
  uint8_t fileHeader[kFileHeaderSize] = {};
  uint8_t informationHeader[kInformationHeaderSize] = {};

  //file type
  fileHeader[0] = 'B';
  fileHeader[1] = 'M';
  //file size
  fileHeader[2] = fileSize;
  fileHeader[3] = fileSize >> 8;
  fileHeader[4] = fileSize >> 16;
  fileHeader[5] = fileSize >> 24;
  //reserved
  fileHeader[6] = 0;
  fileHeader[7] = 0;
  fileHeader[8] = 0;
  fileHeader[9] = 0;
  //number of bytes between start of file and the first byte of pixel data
  fileHeader[10] = kFileHeaderSize + kInformationHeaderSize + kColorPalleteSize;
  fileHeader[11] = 0;
  fileHeader[12] = 0;
  fileHeader[13] = 0;
  //information_header size
  informationHeader[0] = kInformationHeaderSize;
  informationHeader[1] = 0;
  informationHeader[2] = 0;
  informationHeader[3] = 0;
  //width of image
  informationHeader[4] = width;
  informationHeader[5] = width >> 8;
  informationHeader[6] = width >> 16;
  informationHeader[7] = width >> 24;
  //height of image
  informationHeader[8] = height;
  informationHeader[9] = height >> 8;
  informationHeader[10] = height >> 16;
  informationHeader[11] = height >> 24;
  //planes
  informationHeader[12] = 1;
  informationHeader[13] = 0;
  //bits per pixel
  informationHeader[14] = kBitsPerPixel;
  informationHeader[15] = 0;
  //total colors: black, white, green, purple, yellow
  informationHeader[32] = kTotalColors;
  informationHeader[33] = 0;
  informationHeader[34] = 0;
  informationHeader[35] = 0;

  ColorsPreset colors;
  uint8_t colorPallete[kColorPalleteSize] = {};
  //0 - white
  colorPallete[0] = colors.kColorWhite.b;
  colorPallete[1] = colors.kColorWhite.g;
  colorPallete[2] = colors.kColorWhite.r;
  colorPallete[3] = 0;
  //1 - green
  colorPallete[4] = colors.kColorGreen.b;
  colorPallete[5] = colors.kColorGreen.g;
  colorPallete[6] = colors.kColorGreen.r;
  colorPallete[7] = 0;
  //2 - purple
  colorPallete[8] = colors.kColorYellow.b;
  colorPallete[9] = colors.kColorYellow.g;
  colorPallete[10] = colors.kColorYellow.r;
  colorPallete[11] = 0;
  //3 - yellow
  colorPallete[12] = colors.kColorPurple.b;
  colorPallete[13] = colors.kColorPurple.g;
  colorPallete[14] = colors.kColorPurple.r;
  colorPallete[15] = 0;
  //4 - black
  colorPallete[16] = colors.kColorBlack.b;
  colorPallete[17] = colors.kColorBlack.g;
  colorPallete[18] = colors.kColorBlack.r;
  colorPallete[19] = 0;

  file.write(reinterpret_cast<char*>(fileHeader), kFileHeaderSize);
  file.write(reinterpret_cast<char*>(informationHeader), kInformationHeaderSize);
  file.write(reinterpret_cast<char*>(colorPallete), kColorPalleteSize);

  uint64_t first_color, second_color;
  uint8_t total_color;
  for (int64_t y = height - 1; y >= 0; y--) {
    for (int64_t x = 0; x < full_width; x += 2) {
      if (x >= width) {
        first_color = 0;
        second_color = 0;
      } else if (x + 1 >= width) {
        first_color = sp.sapi_matrix[y + sp.from_y][x + sp.from_x];
        second_color = 0;
      } else {
        first_color = sp.sapi_matrix[y + sp.from_y][x + sp.from_x];
        second_color = sp.sapi_matrix[y + sp.from_y][x + sp.from_x + 1];
      }
      if (first_color >= 4) {
        first_color = 4;
      }
      if (second_color >= 4) {
        second_color = 4;
      }
      total_color = (first_color << kBitsPerPixel);
      total_color += second_color;
      file << total_color;
    }
  }
  file.close();
}


uint64_t** DoubleMatrix(SandPileParameters& sp, int16_t recoup_x, int16_t recoup_y) {
  uint64_t** new_sp = new uint64_t* [sp.size_y];
  for (int16_t i = 0; i < sp.size_y; ++i) {
    new_sp[i] = new uint64_t[sp.size_x];
    for (int16_t j = 0; j < sp.size_x; ++j) {
      if (sp.from_y <= i - recoup_y && i - recoup_y < sp.to_y && sp.from_x <= j - recoup_x && j - recoup_x < sp.to_x) {
        new_sp[i][j] = sp.sapi_matrix[i - recoup_y][j - recoup_x];
      } else {
        new_sp[i][j] = 0;
      }
    }
    if (sp.from_y <= i - recoup_y && i - recoup_y < sp.to_y) {
      delete[] sp.sapi_matrix[i - recoup_y];
    }
  }
  delete[] sp.sapi_matrix;
  return new_sp;
}

void ClearMatrix(SandPileParameters& bmp_sapi) {
  for (int16_t i = 0; i < bmp_sapi.size_y; ++i) {
    delete[] bmp_sapi.sapi_matrix[i];
  }
  delete[] bmp_sapi.sapi_matrix;
}

void ExecuteSandpile(SandPileParameters& sp, SandpileOptions& options) {
  CoordsList unstable_coords;
  bool need_to_add;
  int iters = 0;
  int iter_to_save = 1;
  int num_of_images = 1;
  int16_t offset_x, offset_y;
  while (iters < options.max_num_iters || options.max_num_iters == -1) {
    offset_x = 0;
    offset_y = 0;
    need_to_add = false;
    for (int16_t i = sp.from_y; i < sp.to_y; ++i) {
      for (int16_t j = sp.from_x; j < sp.to_x; ++j) {
        if (sp.sapi_matrix[i][j] >= 4) {
          unstable_coords.PushBack(j, i, 1);
          sp.sapi_matrix[i][j] -= 4;
          if (i == 0 || i == sp.size_y - 1 || j == 0 || j == sp.size_x - 1) {
            need_to_add = true;
          }
        }
      }
    }
    if (unstable_coords.IsEmpty()) {
      break;
    }
    if (need_to_add) {
      offset_x = sp.size_x / 2;
      offset_y = sp.size_y / 2;
      sp.size_y *= 2;
      sp.size_x *= 2;
      sp.sapi_matrix = DoubleMatrix(sp, offset_x, offset_y);
      sp.from_y += offset_y;
      sp.to_y += offset_y;
      sp.from_x += offset_x;
      sp.to_x += offset_x;
    }

    while (!unstable_coords.IsEmpty()) {
      sp.sapi_matrix[unstable_coords.head->y + 1 + offset_y][unstable_coords.head->x + offset_x] += 1;
      sp.sapi_matrix[unstable_coords.head->y - 1 + offset_y][unstable_coords.head->x + offset_x] += 1;
      sp.sapi_matrix[unstable_coords.head->y + offset_y][unstable_coords.head->x + 1 + offset_x] += 1;
      sp.sapi_matrix[unstable_coords.head->y + offset_y][unstable_coords.head->x - 1 + offset_x] += 1;
      if (unstable_coords.head->y - 1 + offset_y < sp.from_y) {
        --sp.from_y;
      }
      if (unstable_coords.head->y + 1 + offset_y >= sp.to_y) {
        ++sp.to_y;
      }
      if (unstable_coords.head->x - 1 + offset_x < sp.from_x) {
        --sp.from_x;
      }
      if (unstable_coords.head->x + 1 + offset_x >= sp.to_x) {
        ++sp.to_x;
      }
      unstable_coords.Shear();
    }
    if (options.freq_saving > 0 && iter_to_save == options.freq_saving) {
      SaveImage(sp, options, num_of_images);
      ++num_of_images;
      iter_to_save = 0;
    }
    ++iter_to_save;
    ++iters;
  }
  if (options.freq_saving == 0) {
    SaveImage(sp, options, num_of_images
    );
  }
  ClearMatrix(sp);
};