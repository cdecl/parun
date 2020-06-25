
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
using namespace std;

#include <clipp.h>
#include <ThreadPool.h>

#ifdef _MSC_VER 
  #define popen _popen
  #define pclose _pclose 
#endif

struct config
{
    int threads = 1;
    string path = "";
    bool isholder = false;
    string pholder = "{}";
    vector<string> command;
    bool verbose = false;
    bool version = false;
};


bool Usage(int argc, char* argv[], config &conf) 
{
    using namespace clipp;
    bool success = true;

    auto cli = (option("-f") & value("file", conf.path).doc("Input file path, default"),
        option("-p") & value("threads", conf.threads).doc("Thread pool count"),
                option("-i", "--replace").set(conf.isholder).doc("Placeholder string (default : {})"),
                option("-t", "--verbose").set(conf.verbose).doc("Verbose"),
                option("-v", "--version").set(conf.version).doc("Version 200626.0"),
                values("Command", conf.command).doc("Command expression (ex: echo {} )"));

    if (!parse(argc, argv, cli) || conf.version)
    {
        cout << make_man_page(cli, argv[0]) << endl;
        //std::cout << usage_lines(cli, argv[0]) << endl;
        success = false;
    }
    
    return success;
}

string replaceAll(string str, const string& f, const string &r)
{
	string::size_type offset = 0;
	string::size_type pos = string::npos;

	while ((pos = str.find(f, offset)) != string::npos) {
		str.replace(pos, f.length(), r);
		offset = pos + r.length();
	}

	return str;
}

void Worker(const string& redirect, const config &conf)
{
    const int MAXLINE = 1024;
    char buff[MAXLINE];
    // memset(buff, 0, MAXLINE);

    string cmd {};
    for (auto s : conf.command) {
        if (conf.isholder) {
            cmd += replaceAll(s, conf.pholder, redirect); 
        } else {
            cmd += s;
        }
        cmd += " ";
    }

    if (!conf.isholder) {
        cmd += redirect;
    }
    if (conf.verbose) {
        cout << cmd << endl;
    }

    FILE* fp = popen(cmd.c_str(), "r");
    while(fgets(buff, MAXLINE, fp)) {
        cout << buff;
    }
    pclose(fp);
}

void Run(const config &conf)
{
    auto run_ = [&conf](istream& fin) {
        ThreadPool pool(conf.threads);
        vector<future<void>> vs;
        
        string redirect;
        while (!fin.eof()) {
            getline(fin, redirect);
            if (redirect.empty()) continue;

            vs.emplace_back(
                pool.enqueue(Worker, redirect, conf)
            );
        }

        for (auto &&v : vs) v.get();    // wait
    };

    if (conf.path.empty()) {
        run_(cin);
    }
    else {
        ifstream fin(conf.path.c_str());
        if (!fin) {
            cerr << "File Open Error" << endl;
            return exit(-1);
        }
        run_(fin);
    }
}

int main(int argc, char* argv[]) 
{
    config conf;
    if (!Usage(argc, argv, conf)) return 1;

    Run(conf);
}