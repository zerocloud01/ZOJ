#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main(int argc, char* argv[])// 传入命令行参数
{
    if (argc != 6)
    {
        cerr << "用法: " << argv[0] << " source.cpp data.in data.out time_limit(s) memory_limit(MB)" << endl;
        // 顺序严格！！！
        // 编译后使用 ./judge user_code.cpp data.in data.out 2 256
        return 1;
    }
    
    const char* source = argv[1];        // 用户提交的 C++ 源代码文件
    const char* input_file = argv[2];    // 测试输入文件
    const char* expected_file = argv[3]; // 标准答案文件
    int time_limit = atoi(argv[4]);      // 时间限制（秒）
    int mem_limit_kb = atoi(argv[5])*1024;    // 内存限制（KB）
    
    // 编译用户程序
    string compile_cmd = "g++ -O2 -std=c++11 ";
    compile_cmd += source;
    compile_cmd += " -o solution 2> compile_error.txt";
    if (system(compile_cmd.c_str()) != 0)
    {
        cout << "CE" << endl;
        return 0;
    }

    // 创建子进程运行用户程序
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        return 1;
    }

    if (pid == 0)
    { // 子进程执行用户代码
        struct rlimit rl;

        // 限制 CPU 时间（秒）
        rl.rlim_cur = rl.rlim_max = time_limit;
        if (setrlimit(RLIMIT_CPU, &rl) < 0)
        {
            perror("setrlimit RLIMIT_CPU error");
            exit(1);
        }

        // 限制内存（单位转换为字节）
        rl.rlim_cur = rl.rlim_max = mem_limit_kb * 1024;
        if (setrlimit(RLIMIT_AS, &rl) < 0)
        {
            perror("setrlimit RLIMIT_AS error");
            exit(1);
        }

        // 重定向 std
        freopen(input_file, "r", stdin);
        freopen("user_output.txt", "w", stdout);

        // 设置TLE（使用 alarm 发送 SIGALRM）
        alarm(time_limit);

        // 执行用户程序
        execl("./solution", "solution", (char*)NULL);
        exit(1);
    }
    else
    { // 父进程监控子进程
        int status;
        int waited_seconds = 0;
        int ret = 0;

        while ((ret = waitpid(pid, &status, WNOHANG)) == 0 && waited_seconds < time_limit)
        {
            sleep(1);
            waited_seconds++;
        }

        // TLE
        if (ret == 0)
        {
            kill(pid, SIGKILL);  // 强制杀死子进程
            cout << "TLE" << endl;
            return 0;
        }

        // 检查子进程是否因信号终止
        if (WIFSIGNALED(status))
        {
            int sig = WTERMSIG(status);
            // 如果因 SIGKILL、SIGXCPU 或 SIGALRM 被杀，返回 TLE
            if (sig == SIGKILL || sig == SIGXCPU || sig == SIGALRM)
            {
                cout << "TLE" << endl;
                return 0;
            }
            else if (sig == SIGSEGV || sig == SIGABRT)
            {
                cout << "MLE" << endl;
                return 0;
            }
            else
            {
                cout << "RE" << endl;
                return 0;
            }
        }

        // 如果子进程正常结束但返回非零，则为运行时错误
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            cout << "RE" << endl;
            return 0;
        }

        // 若程序在规定时间内结束，则进行输出比对
        ifstream user_out("user_output.txt");
        ifstream expected_out(expected_file);
        string user_line, expected_line;
        bool correct = true;

        while (getline(user_out, user_line) && getline(expected_out, expected_line))
        {
            if (user_line != expected_line)
            {
                correct = false;
                break;
            }
        }
        if (getline(user_out, user_line) || getline(expected_out, expected_line))
        {
            correct = false;
        }

        cout << (correct ? "AC" : "WA") << endl;
    }

    return 0;
}