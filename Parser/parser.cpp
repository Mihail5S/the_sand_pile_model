#include "parser.h"


void InvalidArg() {
  std::cerr << "error: Wrong argument";
  exit(EXIT_FAILURE);
};

void InvalidArgNum() {
  std::cerr << "error: No arguments";
  exit(EXIT_FAILURE);
};

void FileNotOpened() {
  std::cerr << "error: Unable to open file";
  exit(EXIT_FAILURE);
}

size_t CompareStrings(const char* s1, const char* s2, size_t count) {
  while (count > 0 && *s1 != '\0' && (*s1 == *s2)) {
    ++s1;
    ++s2;
    --count;
  }
  if (count == 0) {
    return 0;
  }

  return (*s1 > *s2) - (*s2 > *s1);
};
size_t StringLen(const char* str) {
  const char* ptr = str;
  for (; *ptr != '\0'; ++ptr);
  return ptr - str;
};
char* StringArgumentValue(char* s) {
  return std::find(s, s + StringLen(s), '=') + 1;
}
int64_t Int64ArgumentValue(char* s) {
  return std::stoll(StringArgumentValue(s), nullptr, 10);
}

SandpileOptions ParseArguments(int argc, char* argv[]) {
  SandpileOptions options;
  ArgInfo arguments[] = {
      "-i", 2, 0,
      "--input=", 8, 0,
      "-o", 2, 1,
      "--output=", 9, 1,
      "-m", 2, 2,
      "--max-iter=", 11, 2,
      "-f", 2, 3,
      "--freq=", 7, 3
  };
  int i = 1;
  bool is_argument_correct;
  while (i < argc) {
    is_argument_correct = false;
    for (int j = 0; j < options.kNumberOfArguments; ++j) {
      if (!CompareStrings(argv[i], arguments[j].kArgumentKey, arguments[j].kArgumentLen)) {
        is_argument_correct = true;
        if (arguments[j].kArgumentLen == 2) {
          if (i < argc - 1) {
            switch (arguments[j].kArgumentType) {
              case 0:
                options.open_path = argv[++i];
                break;
              case 1:
                options.save_path = argv[++i];
                break;
              case 2:
                options.max_num_iters = std::stoll(argv[++i], nullptr, 10);
                break;
              case 3:
                options.freq_saving = std::stoll(argv[++i], nullptr, 10);
                break;
              default:
                break;
            }
          } else {
            InvalidArgNum();
          }
        } else {
          switch (arguments[j].kArgumentType) {
            case 0:
              options.open_path = StringArgumentValue(argv[i]);
              break;
            case 1:
              options.save_path = StringArgumentValue(argv[i]);
              break;
            case 2:
              options.max_num_iters = Int64ArgumentValue(argv[i]);
              break;
            case 3:
              options.freq_saving = Int64ArgumentValue(argv[i]);
              break;
            default:
              break;
          }
        }
      }
    }
    if (!is_argument_correct) {
      InvalidArg();
    }
    ++i;
  }
  return options;
};


bool IsDigit(const char chr) {
  return '0' <= chr && chr <= '9';
}
int16_t GetDigitFromChar(const char chr) {
  return chr - '0';
}

SandPileParameters GetMatrixFromFile(SandpileOptions options) {
  std::ifstream file(options.open_path, std::ios::in);
  if (!file.is_open()) {
    FileNotOpened();
  }
  char tmp;
  file.seekg(0, std::ios::beg);
  int cnt = 0;
  int16_t x = 0, y = 0;
  uint64_t num = 0;
  int16_t max_x = 0, max_y = 0;
  int16_t min_x = -1, min_y = -1;
  CoordsList list;
  SandPileParameters sp;
  while (file.get(tmp)) {
    if (tmp == '\t') {
      ++cnt;
    } else if (tmp == '\n') {
      cnt = 0;
      if (x > max_x) {
        max_x = x;
      }
      if (x < min_x || min_x == -1) {
        min_x = x;
      }
      if (y > max_y) {
        max_y = y;
      }
      if (y < min_y || min_y == -1) {
        min_y = y;
      }
      list.PushBack(x, y, num);
      x = 0;
      y = 0;
      num = 0;
    } else if (IsDigit(tmp)) {
      switch (cnt) {
        case 0:
          x = x * 10 + GetDigitFromChar(tmp);
          break;
        case 1:
          y = y * 10 + GetDigitFromChar(tmp);
          break;
        case 2:
          num = num * 10 + GetDigitFromChar(tmp);
          break;
        default:
          break;
      }
    }
  }
  file.close();

  sp.size_x = max_x + 1;
  sp.size_y = max_y + 1;

  sp.sapi_matrix = new uint64_t* [sp.size_y];
  for (int16_t i = 0; i < sp.size_y; ++i) {
    sp.sapi_matrix[i] = new uint64_t[sp.size_x];
    for (int16_t j = 0; j < sp.size_x; ++j) {
      sp.sapi_matrix[i][j] = 0;
    }
  }

  while (!list.IsEmpty()) {
    sp.sapi_matrix[list.head->y][list.head->x] += list.head->num;
    list.Shear();
  }
  sp.from_x = min_x;
  sp.from_y = min_y;
  sp.to_x = max_x + 1;
  sp.to_y = max_y + 1;
  return sp;
}
