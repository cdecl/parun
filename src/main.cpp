
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
using namespace std;

#include <CLI11.hpp>
// #include <ThreadPool.h>
#include <BS_thread_pool.hpp>

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
    bool success = true;

    CLI::App app;
    app.add_option("-f,--file", conf.path, "Input file path, default");
    app.add_option("-p,--threads", conf.threads, "Thread pool count");
    app.add_flag("-i,--replace", conf.isholder, "Placeholder string (default : {})");
    app.add_flag("-t,--verbose", conf.verbose, "Verbose");
    app.add_flag("-v,--version", conf.version, "Version 200626.0");
    app.add_option("Command", conf.command, "Command expression (ex: echo {} )")->required();

    CLI11_PARSE(app, argc, argv);

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
        BS::thread_pool pool(conf.threads);
        // ThreadPool pool(conf.threads);
        vector<future<void>> vs;
        
        string redirect;
        while (!fin.eof()) {
            getline(fin, redirect);
            if (redirect.empty()) continue;

            auto f = pool.submit_task(std::bind(Worker, redirect, conf));
            vs.push_back(std::move(f));
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