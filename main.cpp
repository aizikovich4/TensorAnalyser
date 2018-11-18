#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::program_options;

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
        return 1;
    return 0;
}

void recursive_add_dirs(vector<std::string> &sources)
{
    using namespace boost::filesystem;
    for(auto &it: sources){
        recursive_directory_iterator dir( it), end;
        while (dir != end)
        {
            dir->path().exte

            //++dir;
        }
    }

}

int main(int argc, char* argv[])
{
    vector<std::string> source_dir;
    try {
        if(argc < 2)
        {
            cout<< "need more params"<<endl;
            return 1;
        }
        options_description desc("Allowed options");
        desc.add_options()
        ("help,h", "print usage message")
        ("input,I",  value(&source_dir), "Input directory for headers file")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        if (vm.count("input"))
        {
            source_dir = vm["input"].as< vector<string> >();
        }
        source_dir.push_back(argv[1]);

        if(check_exist_dirs(source_dir))
            return 1;
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
    }

    recursive_add_dirs(source_dir);

    for(auto &t: source_dir)
        cout<<t<< endl;

    return 0;

}
