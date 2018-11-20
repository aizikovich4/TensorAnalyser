#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include<regex>
#include "includeheader.hpp"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

vector<Header> headers;

int check_exist_dirs(vector<string> &sources)
{
    for(auto i = 0; i< sources.size(); ++i)
    {
        if ( !boost::filesystem::exists( sources[i]) )
        {
            cout << "Wrong path to the directory: " << sources[i]<< endl;
            sources.erase(sources.begin() + i);
            --i;
        }
    }
    if (!sources.size())
        return -1;
    return 0;
}

//collect all .cpp files
vector<path> collect_source(string &path_to_source)
{
    vector<path> sources_cpp;
    recursive_directory_iterator dir( path_to_source), end;
    //collect all .cpp files
    while (dir != end)
    {
        if (dir->path().extension() == ".cpp" )
        {
            sources_cpp.push_back(dir->path());
        }
        ++dir;
    }
    return sources_cpp;
}

void build_tree(vector<path> cpp_files)
{
    std::regex reg("^#include([<\"])(.*[.hpp|.h])([>\"])$");
    string str;
    string::size_type pos1,pos2;
    std::smatch match;

    for(auto &dir: cpp_files)
    {
        cout<<"Source: "<< dir << endl;
        std::ifstream in(dir.string());
        if(!in)
        {
            cout << "Cannot open the File : "<<dir<<endl;
            continue;
        }

         // Reads line by line until the end
         while (getline(in, str))
         {
             str.erase(std::remove(str.begin(), str.end(), ' '),str.end()); //delete all whitespaces

             if(((pos1=str.find("/*")) != std::string::npos) &&
                ((pos2=str.find("*/")) != std::string::npos) &&
                  pos2-pos1 > 2
                )
                str.erase(pos1,pos2-pos1+2);
             if((pos1=str.find("//")) != std::string::npos)
                str.erase(pos1,str.length()-pos1);


                   if(str.find("/*") != std::string::npos)
                     {
                       pos1=in.tellg(); // new reading position in the file

                       while(getline(in, str))
                         if((pos2=str.find("*/")) != string::npos)
                           continue;
                       in.seekg(pos1);
                     }

                   if(std::regex_match(str, match, reg))
                     if(match[1].str() == std::string("<") && match[3].str() == std::string(">"))
                       {
                         cout<<"Global " <<str << "   "<<match[2].str()<< endl;

                         Header *h=new Header(match[2].str());
                         h->set_GlobalHeader(true);
                        // sibs.push_back(*h);
                         delete h;
                       }
                   if(match[1].str() == std::string("\"") && match[3].str() == std::string("\""))
                     {
                       cout<<"Local " <<str << "   "<<dir / match[2].str()<< endl;
                       Header *h=new Header(dir / match[2].str());
                       if( exists(h->string()))
                            h->set_ExistHeader(true);
                       else
                            h->set_ExistHeader(false);
                      // sibs.push_back(*h);
                       delete h;
                     }
         }
         in.close();
    }
}

int main(int argc, char* argv[])
{
    string source_dir = "";
    vector<string> dirs_headers{};
    vector<path> cpp_files{};

    try {
        if(argc < 2)
        {
            cout<< "need more params"<<endl;
            return -1;
        }
        options_description desc("Allowed options");
        desc.add_options()
        ("help,h", "print usage message")
        ("input,I",  value(&dirs_headers), "Input directory for headers file")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        if (vm.count("input"))
        {
            dirs_headers = vm["input"].as< vector<string> >();
        }

        //check existing source directory (it's second parameters in argv). If it does't exist - next actions is mindless.
        source_dir = argv[1];
        if ( !boost::filesystem::exists( source_dir) )
        {
            return -1;
        }

        //check existing headers directories - if they doesn't exist - there is no place for search out headers.
        if(check_exist_dirs(dirs_headers))
        {
            cout<<"Wrong path to include files"<<endl;
            return -1;
        }

        cpp_files = collect_source(source_dir);
        build_tree(cpp_files);

    }
    catch(exception& e) {
        cerr << e.what() <<endl;
    }
    catch(...){
        cerr<<"something bad"<<endl;
    }


    return 0;

}



