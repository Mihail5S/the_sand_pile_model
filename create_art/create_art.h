#pragma once
#include "../Parser/parser.h"
#include <cstring>
#include <string>
#include <fstream>

struct Color {
  uint8_t r = 0, g = 0, b = 0;
  Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {};
};

struct ColorsPreset {
  const Color kColorWhite = Color(255, 255, 255);
  const Color kColorGreen = Color(85, 247, 63);
  const Color kColorYellow = Color(220, 233, 28);
  const Color kColorPurple = Color(168, 80, 251);
  const Color kColorBlack = Color(0, 0, 0);
};

const uint64_t kFileHeaderSize = 14;
const uint64_t kInformationHeaderSize = 40;
const uint64_t kTotalColors = 5;
const uint64_t kBitsPerPixel = 4;
const uint64_t kColorPalleteSize = 20;

void SaveImage(SandPileParameters& sp, SandpileOptions& options, int img_count);

uint64_t** DoubleMatrix(SandPileParameters& sp, int16_t recoup_x, int16_t recoup_y);

void ClearMatrix(SandPileParameters& bmp_sapi);

void ExecuteSandpile(SandPileParameters& sp, SandpileOptions& options);
