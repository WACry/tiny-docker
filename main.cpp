#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <sched.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::runtime_error;

const char *child_hostname = "container";

static void run(int argc, char **argv);
static string build_cmd(int argc, char **argv);
static void run_child(int argc, char **argv);
static void check_syscall(int ret, const string& errmsg);

int main(int argc, char **argv) {
    try {
        if (argc < 3) {
            throw runtime_error("Too few arguments");
        }

        if (!strcmp(argv[1], "run")) {
            run(argc - 2, &argv[2]);
        }

    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }
}

static void run(int argc, char **argv) {
    cout << "Parent running " << build_cmd(argc, argv) << " as " << getpid() << endl;    

    check_syscall(unshare(CLONE_NEWPID), "Fail to unshare PID namespace");

    pid_t child_pid = fork();

    check_syscall(child_pid, "Fail to fork child");

    if (child_pid) {
        check_syscall(waitpid(child_pid, NULL, 0), "Fail to wait for child");
    } else {        
        run_child(argc, argv);
    }
}

static string build_cmd(int argc, char **argv) {
    string cmd = "";
    for (auto i = 0; i < argc; ++i) {
        cmd.append(argv[i] + string(" "));
    }
    return cmd;
}

static void run_child(int argc, char **argv) {
    cout << "Child running " << build_cmd(argc, argv) << " as " << getpid() << endl;    

    int flags = CLONE_NEWUTS | CLONE_NEWNS;

    check_syscall(unshare(flags), "Fail to unshare in child");

    check_syscall(mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL), "Fail to mount /");

    check_syscall(chroot("../ubuntu-fs"), "Fail to chroot");

    check_syscall(chdir("/"), "Fail to chdir to /");

    check_syscall(mount("proc", "proc", "proc", 0, NULL), "Fail to mount /proc");

    check_syscall(sethostname(child_hostname, strlen(child_hostname)), "Fail to change hostname");

    if (execvp(argv[0], argv)) {
        cerr << "Fail to exec" << endl;
    }
}

static void check_syscall(int ret, const string& errmsg) {
    if (ret < 0) {
        throw runtime_error(errmsg);
    }
}
