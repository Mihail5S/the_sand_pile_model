#pragma once
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iostream>

struct SandpileOptions {
  char* open_path;
  char* save_path;
  int64_t max_num_iters = -1;
  int64_t freq_saving = 0;
  const int kNumberOfArguments = 8;
};

struct ArgInfo {
  const char* kArgumentKey;
  const int kArgumentLen;
  const int kArgumentType;
};

void InvalidArg();

void InvalidArgNum();

void FileNotOpened();

size_t CompareStrings(const char* s1, const char* s2, size_t count);

size_t StringLen(const char* str);

char* StringArgumentValue(char* s);

int64_t Int64ArgumentValue(char* s);

SandpileOptions ParseArguments(int argc, char* argv[]);

struct Node {
  int16_t x = 1;
  int16_t y = 1;
  uint64_t num = 0;
  Node* next;
  Node(int16_t a, int16_t b, uint64_t c){
    x = a;
    y = b;
    num = c;
    next = nullptr;
  }
  ~Node() {
    next = nullptr;
  }
};

struct CoordsList {
  Node* head;
  Node* tail;
  CoordsList() {
    head = nullptr;
    tail = nullptr;
  }
  ~CoordsList() {
    while (!IsEmpty()) {
      Shear();
    }
  }

  void PushBack(int16_t a, int16_t b, uint64_t c) {
    Node* new_node = new Node(a, b, c);
    if (IsEmpty()) {
      head = new_node;
      tail = head;
      return;
    }
    tail->next = new_node;
    tail = new_node;
  }

  void Shear() {
    if (IsEmpty()) {
      return;
    }
    Node* tmp = head;
    if (head == tail) {
      tail = nullptr;
      head = nullptr;
    } else {
      head = head->next;
    }
    delete tmp;
  }
  bool IsEmpty() {
    return head == nullptr;
  }
};

struct SandPileParameters {
  int16_t size_x = 1;
  int16_t size_y = 1;
  int16_t from_x = 0;
  int16_t to_x = 2;
  int16_t from_y = 0;
  int16_t to_y = 2;
  uint64_t** sapi_matrix;
};

bool IsDigit(char chr);

int16_t GetDigitFromChar(char chr);

SandPileParameters GetMatrixFromFile(SandpileOptions options);

