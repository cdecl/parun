
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

struct config {
    int threads = 1;
    string path = "";
    bool verbose = false;
};


bool Usage(int argc, char* argv[], config &conf) 
{
    using namespace clipp;
    bool success = true;

    auto cli = (
        option("-f") & value("file_path", conf.path),
        option("-p") & value("threads", conf.threads),
        option("-v").set(conf.verbose)
    );

    if(!parse(argc, argv, cli)) {
        std::cout << usage_lines(cli, argv[0]) << endl;
        success = false;
    }

    return success;
}

void Worker(const string& cmd, const config &conf)
{
    int MAXLINE = 1024;
    char buff[MAXLINE];
    // memset(buff, 0, MAXLINE);

    if (conf.verbose) cout << cmd << endl;

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
        
        string cmd;
        while (!fin.eof()) {
            getline(fin, cmd);
            vs.emplace_back(
                pool.enqueue(Worker, cmd, conf)
            );
        }

        for (auto &&v : vs) v.get();
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