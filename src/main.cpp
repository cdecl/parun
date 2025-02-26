
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
using namespace std;

#include <CLI11.hpp>
#include <BS_thread_pool.hpp>

#ifdef _MSC_VER 
  #define popen _popen
  #define pclose _pclose 
#endif


struct config {
    int threads{1};
    std::string path{};
    bool isholder{false};
    std::string pholder{"{}"};
    std::vector<std::string> command{};
    bool verbose{false};
    bool version{false};
};


int ParseArguments(int argc, char* argv[], config &conf) 
{
    CLI::App app;
    app.add_option("-f,--file", conf.path, "Input file path, default");
    app.add_option("-p,--threads", conf.threads, "Thread pool count");
    app.add_flag("-i,--replace", conf.isholder, "Placeholder string (default : {})");
    app.add_flag("-t,--verbose", conf.verbose, "Verbose");
    app.add_flag("-v,--version", conf.version, "Version 250225.0");
    app.add_option("Command", conf.command, "Command expression (ex: echo {} )")->required();

    CLI11_PARSE(app, argc, argv);
    return 0;
}


std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

void Worker(const string& redirect, const config &conf)
{
    const int MAXLINE = 1024;
    char buff[MAXLINE];
    // memset(buff, 0, MAXLINE);

    std::ostringstream cmd;
    for (auto s : conf.command) {
        cmd << (conf.isholder ? replaceAll(s, conf.pholder, redirect) : s) << " ";
    }
    
    if (!conf.isholder) {
        cmd << redirect;
    }

    std::string command = cmd.str();
    if (conf.verbose) {
        cout << command << endl;
    }

    FILE* fp = popen(command.c_str(), "r");
    if (!fp) {
        std::cerr << "Error executing command: " << command << std::endl;
        return;
    }

    while(fgets(buff, MAXLINE, fp)) {
        cout << buff;
    }
    pclose(fp);
}


void Run(const config &conf)
{
    auto run_ = [&conf](std::istream& fin) {
        BS::thread_pool pool(conf.threads);
        std::vector<std::future<void>> tasks;
        
        std::string redirect;
        while (std::getline(fin, redirect)) {
            if (redirect.empty()) continue;
            // tasks.push_back(std::move(pool.submit_task(Worker, redirect, conf)));
            auto f = pool.submit_task(std::bind(Worker, redirect, conf));
            tasks.push_back(std::move(f));
        }

        for (auto &t : tasks) t.get(); // 모든 작업 완료 대기
    };

    if (conf.path.empty()) {
        run_(std::cin);
    } else {
        std::ifstream fin(conf.path);
        if (!fin) {
            std::cerr << "File Open Error\n";
            exit(EXIT_FAILURE);
        }
        run_(fin);
    }
}


int main(int argc, char* argv[]) 
{
    config conf;
    ParseArguments(argc, argv, conf);

    Run(conf);
}