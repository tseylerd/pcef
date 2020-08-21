//
// Created by Dmitriy Tseyler  on 20.08.2020.
//

#include <iostream>
#include "util.h"

namespace util {
  void concat(char* result, const char* first, const char* second) {
    size_t f_len = strlen(first);
    strcpy(result, first);
    strcpy(result + f_len, second);
    puts(result);
  }
}
