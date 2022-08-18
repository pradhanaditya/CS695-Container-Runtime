#include <sys/wait.h>
#include <sched.h>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <sys/mount.h>
#include <string>

using namespace std;

// Arguments Position
typedef enum
{
    PROGRAM_NAME,
    HOST_NAME,
    ROOT_FILESYSTEM_PATH,
    PARENT_VETH_NAME,
    PARENT_VETH_IP_ADDR,
    CHILD_VETH_NAME,
    CHILD_VETH_IP_ADDR,
    CGROUP_NAME,
    CGROUP_MEMORY_LIMIT
} Argument_Positions;

// Structure for all the arguments
typedef struct args
{
    string hostName;
    string root_filesystem_path;
    string parent_veth_name;
    string parent_veth_ip_addr;
    string child_veth_name;
    string child_veth_ip_addr;
    string cgroup_name;
    int cgroup_memory_limit_in_megabytes;
} Arguments;

// Stack size to be allocated for clone system call
#define STACK_SIZE 65536

// Total arguments required (including the name of program)
#define TOTAL_ARGUMENTS_REQUIRED 9

// This function will use system function to execute command
void execute_command(string command)
{
    system(command.c_str());
}

// This function will print the usage message
void display_usage_message()
{
    cout << "Please give 8 arguments in the following order:" << endl;
    cout << "1. Hostname\n2. Root filesystem path\n3. Veth name for the parent namespace" << endl;
    cout << "4. IP addresses assigned to the parent namespace veth" << endl;
    cout << "5. Veth name for the child namespace" << endl;
    cout << "6. IP addresses assigned to the child namespace veth" << endl;
    cout << "7. CGroup name" << endl;
    cout << "8. Memory limit for the child process (in MB)" << endl;
}

// Configure veth pair between parent network namespace and child network namespace
void add_veth_interfaces(string parent_veth_name, string child_veth_name, int childPID)
{
    execute_command("ip link add " + parent_veth_name + " type veth peer name " + child_veth_name + " netns " + to_string(childPID));
}

// Configure network namespace
void configure_network_namespace(string veth_name, string veth_ip_addr_range, string route_entry)
{
    execute_command("ip addr add " + veth_ip_addr_range + " dev " + veth_name);
    execute_command("ip link set " + veth_name + " up");
    execute_command("ip link set lo up");
    execute_command("ip route add " + route_entry + " dev " + veth_name);
}

// Creates the cgroup and assigns memory limit as specified in the arguments
void set_up_cgroup_and_memory_limit(string cGroupName, int memory_limit_in_bytes, int childPID)
{
    execute_command("mkdir /sys/fs/cgroup/memory/" + cGroupName);
    execute_command("echo \"" + to_string(memory_limit_in_bytes) + "\"" + " > /sys/fs/cgroup/memory/" + cGroupName + "/memory.limit_in_bytes");
    execute_command("echo \"0\" > /sys/fs/cgroup/memory/" + cGroupName + "/memory.swappiness");
    execute_command("echo \"" + to_string(childPID) + "\"" + " > /sys/fs/cgroup/memory/" + cGroupName + "/tasks");
}

// Child process will execute this function
int childCode(void *arg)
{
    // Waiting for parent process to set up network namespace parameters
    sleep(2);

    // Fetch the arguments into Arguments structure
    Arguments *args = (Arguments *) arg;
    
    // Setting up the hostname using sethostname system call
    sethostname(args->hostName.c_str(), args->hostName.length());

    // Setting up the root filesystem
    chroot(args->root_filesystem_path.c_str());

    // Changing the current working directory to root directory
    chdir("/");

    // Proc file system path
    const char *PROC_MOUNT_POINT_PATH = "/proc";

    // Mounting the proc file system
    mount("proc", PROC_MOUNT_POINT_PATH, "proc", 0, NULL);

    // Configure network namespace settings (veth and routing table)
    configure_network_namespace(args->child_veth_name, args->child_veth_ip_addr, args->parent_veth_ip_addr);

    // Executing the bash (using exec system call)
    char *shellArgs[] = {NULL};
    int status = execvp("/bin/bash", shellArgs);

    if (status == -1)
        cout << "Error occurred during exec!" << endl;
    
    return 0;
}

int main(int argc, char *argv[])
{
    // Display usage message if the -help option is passed
    if (argc == 2 && strcmp(argv[1], "-help") == 0)
    {
        display_usage_message();
        exit(0);
    }

    // Checking if appropriate number of arguments are passed or not
    if (argc != TOTAL_ARGUMENTS_REQUIRED)
    {
        cout << "Error: ";
        display_usage_message();
        exit(1);
    }

    // Allocating memory for the stack
    char *stack = (char *) malloc(STACK_SIZE);

    // Allocating memory for the Arguments object
    Arguments *args = new Arguments();

    // Assigning arguments passed to the args structure
    args->hostName = argv[HOST_NAME];
    args->root_filesystem_path = argv[ROOT_FILESYSTEM_PATH];
    args->parent_veth_name = argv[PARENT_VETH_NAME];
    args->parent_veth_ip_addr = argv[PARENT_VETH_IP_ADDR];
    args->child_veth_name = argv[CHILD_VETH_NAME];
    args->child_veth_ip_addr = argv[CHILD_VETH_IP_ADDR];
    args->cgroup_name = argv[CGROUP_NAME];
    args->cgroup_memory_limit_in_megabytes = atoi(argv[CGROUP_MEMORY_LIMIT]);

    // Creating the child process by passing suitable flags
    int childPID = clone(childCode, stack + STACK_SIZE, CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, args);

    // If there is no error clone will return the child process id, otherwise it will return -1
    if (childPID == -1)
    {
        cout << "Error during clone!" << endl;        
    }

    // Configure veth pair between parent network namespace and child network namespace
    add_veth_interfaces(args->parent_veth_name, args->child_veth_name, childPID);

    // Configure network namespace settings (veth and routing table)
    configure_network_namespace(args->parent_veth_name, args->parent_veth_ip_addr, args->child_veth_ip_addr);

    // Create cgroup and assign the memory limit specified in the arguments
    set_up_cgroup_and_memory_limit(args->cgroup_name, args->cgroup_memory_limit_in_megabytes*1024*1024, childPID);

    // Wait for the child to exit
    waitpid(childPID, NULL, 0);

    return 0;
}