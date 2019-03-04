
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

struct config {
    int threads = 1;
    string path = "";
    string pholder = "{}";
    string command = "{}";
    bool verbose = false;
};


bool Usage(int argc, char* argv[], config &conf) 
{
    using namespace clipp;
    bool success = true;

    auto cli = (
        option("-f") & value("file", conf.path).doc("Input file path, default"),
        option("-p") & value("threads", conf.threads).doc("Thread pool count"),
        option("-i") & value("placeholder", conf.pholder).doc("Placeholder string (default : {})"),
        option("-v").set(conf.verbose).doc("Verbose"),
        value("Command", conf.command).doc("Command expression (ex: \"sleep {}\")")
    );

    if(!parse(argc, argv, cli)) {
       cout <<  make_man_page(cli, argv[0]) << endl;
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

void Worker(const string& expr, const string& param, const config &conf)
{
    const int MAXLINE = 1024;
    char buff[MAXLINE];
    // memset(buff, 0, MAXLINE);

    if (conf.verbose) cout << expr << endl;

    FILE* fp = popen(expr.c_str(), "r");
    while(fgets(buff, MAXLINE, fp)) {
        cout << "[" << param << "] " << buff;
    }
    pclose(fp);
}

void Run(const config &conf)
{
    auto run_ = [&conf](istream& fin) {
        ThreadPool pool(conf.threads);
        vector<future<void>> vs;
        
        string expr, param;
        while (!fin.eof()) {
            getline(fin, param);

            if (param.empty()) continue;
            expr = replaceAll(conf.command, conf.pholder, param);

            vs.emplace_back(
                pool.enqueue(Worker, expr, param, conf)
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