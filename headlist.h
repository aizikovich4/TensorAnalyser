#ifndef HEADLIST_H
#define HEADLIST_H
#include <vector>
#include "includeheader.hpp"

class HeadList
{
public:
    explicit HeadList(path p){work_dir = p;};
    void set_Includes( std::vector<Header> v){ cpp_includes = v;};
    std::vector<Header>  get_Includes(){return cpp_includes;};
    std::vector<Header> cpp_includes= {};
    path work_dir;
private:

};

#endif // HEADLIST_H
