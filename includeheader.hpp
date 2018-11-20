#pragma once

#include<string>
#include<boost/filesystem.hpp>
#include<boost/filesystem/fstream.hpp>
//it is an element of a tree
using namespace boost::filesystem;

class Header:public path
{
public:
    explicit Header(path);

    void set_LastHeader(bool l) {last=l;}
    bool get_LastHeader() const {return last;}

    bool is_HeaderExist() const {return exist;}
    void set_HeaderExist(bool y) {exist = y;}

    void set_LeftHeader(Header *h) {left=h;}
    Header* get_LeftHeader() const {return left;}

    bool is_globalHeader() const {return global;}
    void set_GlobalHeader(bool a) {global = a;}

    void set_NextHeader(Header* n) {next = n;}
    Header* get_NextHeader() const {return next;}

    void set_ParentHeader(Header *p){parent = p;}
    Header* get_ParentHeader() const {return parent;}


private:
    Header* left; //LLink
    Header* next;
    Header* parent;
    bool last; // tag
    bool exist;
    bool global;
};
