#include "../Parser/parser.cpp"
#include "../create_art/create_art.cpp"

#include <iostream>

int main(int argc, char* argv[]) {
  SandpileOptions options = ParseArguments(argc, argv);
  SandPileParameters sp = GetMatrixFromFile(options);
  ExecuteSandpile(sp, options);
  return 0;
}