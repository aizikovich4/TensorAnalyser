#include "includeheader.hpp"
#include <fstream>


Header::Header(path p): path(p){
   left = next = parent = nullptr;
   last = exist = global = false;
};

