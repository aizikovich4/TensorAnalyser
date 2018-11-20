#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include<regex>
#include "includeheader.hpp"

#define LIMIT_INCLUDES 1000

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

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

vector<Header> collect_include(vector<path> cpp_files)
{
    regex reg("^#include([<\"])(.*[.hpp|.h])([>\"])$");
    string str;
    string::size_type pos1,pos2;
    std::smatch match;
    vector<Header> temp;

    for(auto &dir: cpp_files)
    {
        //cout<<"Source: "<< dir << endl;
        std::ifstream in(dir.string());
        if(!in)
        {
            cout << "Can't open file : "<<dir<<endl;
            continue;
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
                         //cout<<"Global " <<str << "   "<<match[2].str()<< endl;

                         Header *header=new Header(match[2].str());
                         header->set_GlobalHeader(true);
                         temp.push_back(*header);
                         delete header;
                       }
                   if(match[1].str() == string("\"") && match[3].str() == string("\""))
                     {
                       //cout<<"Local " <<str << "   "<<dir / match[2].str()<< endl;
                       Header *header=new Header(dir / match[2].str());
                       if( exists(header->string()))
                            header->set_HeaderExist(true);
                       else
                            header->set_HeaderExist(false);
                      temp.push_back(*header);
                       delete header;
                     }
         }
         in.close();
    }
    return temp;
}

void create_tree(vector<Header> includes, vector<path> path_headers)
{

    std::vector<Header> tmpvector;
     tmpvector.reserve(1024);

     includes.back().set_LastHeader(true);


     size_t i=includes.size();
     for(size_t s=0;s < i; s++)
       {
         if( s == LIMIT_INCLUDES )
           break;
         path fileForParse;

         if(includes.at(s).is_globalHeader())
           for(auto& a : path_headers)
             {
               exists(a / includes.at(s).filename()) ? includes.at(s).set_HeaderExist(true) : includes.at(s).set_HeaderExist(false);
               if(includes.at(s).is_HeaderExist())
                 {
                   fileForParse=a / includes.at(s).filename();
                   break;
                 }
             }

         else
         {
           if(!includes.at(s).is_globalHeader())
           {
             exists(includes.at(s).string()) ? includes.at(s).set_HeaderExist(true) : includes.at(s).set_HeaderExist(false);
             fileForParse=includes.at(s).string();
           }
         }

         tmpvector=collect_include(vector<path>{fileForParse});

         if(tmpvector.size())
           {
             tmpvector.back().set_LastHeader(true);

             /* copying all elements from tmpvector to headers */
             size_t  prev_i = i;
             for(auto& it : tmpvector)
               {
                 it.set_ParentHeader(&includes.at(s));
                 includes.push_back(it);
                 includes.at(s).set_LeftHeader(&tmpvector[0]);

               }

             includes.at(s).set_LeftHeader(&includes.at(prev_i));
             i+=tmpvector.size();
           }
         tmpvector.clear();
       }
     /* pointer to the next element that has element with no last tag */
     for(size_t j=0; j < i-1; ++j)
       {
         if(!includes.at(j).get_LastHeader())
           includes.at(j).set_NextHeader(&includes.at(j+1));
       }

}

int main(int argc, char* argv[])
{
    string source_dir = "";
    vector<path> path_headers{};
    vector<path> cpp_files{};
    vector<Header> cpp_includes{};

    try {
        if(argc < 2)
        {
            cout<< "need more params"<<endl;
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
            cout<<"Wrong path to include files"<<endl;
            return -1;
        }

        cpp_files = collect_source(source_dir);
        cpp_includes = collect_include(cpp_files);
            if (cpp_includes.size() == 0 )
            {
                cout <<"Includes not detected"<<endl;
                return -1;
            }

        create_tree(cpp_includes, path_headers);

    }
    catch(exception& e) {
        cerr << e.what() <<endl;
    }
    catch(...){
        cerr<<"something bad"<<endl;
    }


    return 0;

}



