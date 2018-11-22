#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <regex>
#include "headlist.h"
#include "includeheader.hpp"

/* TODO
 * 1. Refactoring for use classes, not C-style function
 * 2. Need incapsulate collecting and create tree in the class
 * 3. For tree - need normal recursive output
 * 4. Move global params to class (no incapsulate there)
 * 5. Regular expression for search file - may be need do it made more flexible in future
 * 6. Create big special class, which have been perform all work by collect headers, find dependency, pretty output.
 * */



using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

vector<path> path_headers{};
string source_dir = "";

bool try_find_file(string header)
{
    for(auto &dir_s: path_headers)
    {
        recursive_directory_iterator dir( dir_s), end;
        while (dir != end)
        {
            if (dir->path().filename() == header )
            {
                return true;
            }
            ++dir;
        }
    }
    return false;

}
int check_exist_dirs(vector<path> &sources)
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

vector<Header> collect_include(path cpp_file)
{
    regex reg("^#include([<\"])(.*[.hpp|.h])([>\"])$");
    string str;
    string::size_type pos1,pos2;
    std::smatch match;
    vector<Header> temp{};
    path working_path = cpp_file.parent_path();


        //cout<<"Source: "<< dir << endl;
        std::ifstream in(cpp_file.string());
        if(!in)
        {
            cout << "Can't open file : "<<cpp_file<<endl;
            return temp;
        }

         // Reads line by line until the end
         while (getline(in, str))
         {
             str.erase(remove(str.begin(), str.end(), ' '),str.end()); //delete all whitespaces

             if(((pos1=str.find("/*")) != string::npos) &&
                ((pos2=str.find("*/")) != string::npos) &&
                  pos2-pos1 > 2
                )
             str.erase(pos1,pos2-pos1+2);
             if((pos1=str.find("//")) != string::npos)
                str.erase(pos1,str.length()-pos1);


            if(str.find("/*") != string::npos)
             {
               pos1=in.tellg(); // new reading position in the file

               while(getline(in, str))
                 if((pos2=str.find("*/")) != string::npos)
                   continue;
               in.seekg(pos1);
             }

           if(regex_match(str, match, reg))
             if(match[1].str() == string("<") && match[3].str() == string(">"))
             {
               Header *header=new Header(match[2].str());
               if(try_find_file(match[2].str()))
                   header->set_HeaderExist(true);
               header->set_GlobalHeader(true);
               temp.push_back(*header);
               delete header;
             }
           if(match[1].str() == string("\"") && match[3].str() == string("\""))
             {
               path local = source_dir;
               local/= match[2].str();
               Header *header=new Header(match[2].str());
               if( exists(local.string()))
                    header->set_HeaderExist(true);
               else
                    header->set_HeaderExist(false);
                temp.push_back(*header);
               delete header;
             }
         }
         in.close();

    return temp;
}


int main(int argc, char* argv[])
{
    vector<path> cpp_files{};
    vector<HeadList> headerLists;
    vector<Header> cpp_includes{};

    try {
        if(argc < 2)
        {
            cerr<< "need more params"<<endl;
            return -1;
        }
        options_description desc("Allowed options");
        desc.add_options()
        ("help,h", "print usage message")
        ("input,I",  value(&path_headers), "Input directory for headers file")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        if (vm.count("input"))
        {
            path_headers = vm["input"].as< vector<path> >();
        }

        //check existing source directory (it's second parameters in argv). If it does't exist - next actions is mindless.
        source_dir = argv[1];
        if ( !boost::filesystem::exists( source_dir) )
        {
            return -1;
        }

        //check existing headers directories - if they doesn't exist - there is no place for search out headers.
        if(check_exist_dirs(path_headers))
        {
            cerr<<"Wrong path to include files"<<endl;
            return -1;
        }


        //********begin real work*******************************************************//

        recursive_directory_iterator dir( source_dir), end;
        //collect all .cpp files
        while (dir != end)
        {
            if (dir->path().extension() == ".cpp" )
            {
                HeadList headList(dir->path());
                headList.set_Includes(collect_include(dir->path()));
                headerLists.push_back(headList);
            }
            ++dir;
        }

        //simple output, it's not tree
        for(auto &list: headerLists)
        {
            cout<<list.get_Workdir()<<endl;
            for(auto &header: list.get_Includes())
            {
                cout<<"\t"<<header.string()<< (header.is_HeaderExist()? " ":  "(!)")<<endl;
            }
        }

    }
    catch(exception& e) {
        cerr << e.what() <<endl;
    }
    catch(...){
        cerr<<"something bad"<<endl;
    }

    return 0;

}



