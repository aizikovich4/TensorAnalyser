#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

int check_exist_dirs(vector<std::string> &sources)
{
    for(auto i = 0; i< sources.size(); ++i)
    {
        if ( !boost::filesystem::exists( sources[i]) )
        {
            std::cout << "Wrong path to the directory: " << sources[i]<< std::endl;
            sources.erase(sources.begin() + i);
            --i;
        }
    }
    if (!sources.size())
        return -1;
    return 0;
}

void recursive_add_dirs(vector<std::string> &sources)
{

    for(auto &it: sources){
        recursive_directory_iterator dir( it), end;
        while (dir != end)
        {
            //dir->path().exte

            ++dir;
        }
    }

}

vector<path> collect_source(string &path_to_source)
{
    vector<path> sources_cpp;
        recursive_directory_iterator dir( path_to_source), end;
        while (dir != end)
        {
            if (dir->path().extension() == ".cpp")
            {
                sources_cpp.push_back(dir->path());
            }
            ++dir;
        }

        for(auto &dir: sources_cpp)
        {
                //cout<<dir<<endl;
            // Open the File
            ifstream in(dir.string());
            // Check if object is valid
            if(!in)
            {
                cout << "Cannot open the File : "<<dir<<endl;
                continue;
            }
            string str;
             // Read the next line from File untill it reaches the end.
             while (getline(in, str))
             {

                   size_t pos = str.find("#include");
                   if (pos != std::string::npos)
                       cout << str << endl;
             }
             //Close The File
             in.close();

        }

        return sources_cpp;
}


int main(int argc, char* argv[])
{
    vector<std::string> dirs_headers{};
    string source_dir = "";
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
            return -1;
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
    }


    recursive_add_dirs(dirs_headers);
    collect_source(source_dir);

    return 0;

}
