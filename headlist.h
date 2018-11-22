#ifndef HEADLIST_H
#define HEADLIST_H
#include <vector>
#include "includeheader.hpp"

class HeadList
{
public:
    explicit HeadList(path p){work_dir = p; cpp_includes={};};
    void set_Includes( std::vector<Header> v){ cpp_includes = v;};
    std::vector<Header>  get_Includes()const{return cpp_includes;};
    path get_Workdir()const{return work_dir;};

private:
    std::vector<Header> cpp_includes;
    path work_dir;
};

#endif // HEADLIST_H
