# [ARP-assignment V2.0](https://github.com/andreabradpitto/ARP-assignment)

## 📑 Contents list

1. [Introduction](https://github.com/andreabradpitto/ARP-assignment#-introduction)
2. [Repository structure](https://github.com/andreabradpitto/ARP-assignment#-repository-structure)
3. [Software requirements](https://github.com/andreabradpitto/ARP-assignment#-software-requirements)
4. [Installation](https://github.com/andreabradpitto/ARP-assignment#-installation)
5. [Execution](https://github.com/andreabradpitto/ARP-assignment#%EF%B8%8F-execution)
6. [Additional information](https://github.com/andreabradpitto/ARP-assignment#%E2%84%B9%EF%B8%8F-additional-information)
    1. [Process list and brief description](https://github.com/andreabradpitto/ARP-assignment#process-list-and-brief-description)
    2. [Configuration file](https://github.com/andreabradpitto/ARP-assignment#configuration-file)
    3. [Run modes](https://github.com/andreabradpitto/ARP-assignment#run-modes)
    4. [Log file](https://github.com/andreabradpitto/ARP-assignment#log-file)
    5. [Input Terminal & Output Terminal](https://github.com/andreabradpitto/ARP-assignment#input-terminal--output-terminal)
7. [Results](https://github.com/andreabradpitto/ARP-assignment#-results)
8. [Author](https://github.com/andreabradpitto/ARP-assignment#-author)

## 📛 Introduction

This is my repository for the assignment of [Advanced and Robot Programming](https://unige.it/en/off.f/2019/ins/37514) (ARP) [Robotics Engineering](https://courses.unige.it/10635) Master degree course (a.y. 2019/2020), attended at the [University of Genoa](https://unige.it/en). There are 2 versions of the assignment specifications: [V1.0](doc/Assignment%20Specifications%20V1.0.pdf) and [V2.0](doc/Assignment%20Specifications%20V2.0.pdf). This is because of the COVID-19 outbreak, which made the first version unfeasible for health and safety reasons. My implementation is relative to the second and most recent version, but I decided to still include the original specifications file for the sake of completeness.

## 📂 Repository structure

- [doc](doc): folder containing the 2 versions of the assignment specifications

- [src](src): folder containing the source code of the assignment

- [config](config): file storing all the cofiguration settings for the assignment. The header [def.h](src/def.h) contains, among other comments, also the default values for all the elements

- [makefile](makefile): file used to automatically build or delete executables and log files. See [Make](https://en.wikipedia.org/wiki/Make_(software)) for further information

- [.gitignore](.gitignore): hidden file that specifies which files and folder are not relevant for [Git](https://git-scm.com/)

- [LICENSE](LICENSE): a plain text file containing the license terms

- [README.md](README.md): this file

## ❗ Software requirements

- A [**POSIX**](https://en.wikipedia.org/wiki/POSIX)-compliant machine
- [GCC](https://gcc.gnu.org/) compiler collection
- At least 1 MB of free disk space

The space required is intended for the repository contents + the compiled code. Please beware that running the code for an extended time span may produce much greater log file sizes (~ 7 MB per minute with the default [parameters](https://github.com/andreabradpitto/ARP-assignment#configuration-file)).

## ✅ Installation

In order to create the executables, open a terminal, move to this folder, and then run:

```bash
make
```

The make file will take care of compiling all the code needed.  
If you want to remove the executables (and the log file), instead type:

```bash
make clean
```

## ▶️ Execution

To run the code, open a terminal, move to the folder where this repository is located, and then run:

```bash
./main
```

The main process will take care of launching all the other processes, as well as an additional terminal (more on this [later](https://github.com/andreabradpitto/ARP-assignment#input-terminal--output-terminal)). You can stop everything by pressing Ctrl+C in the terminal where *main* has been launched. Notice that Ctrl+C is actually just the `SIGINT` **POSIX** signal.

## ℹ️ Additional information

This assignment has been coded in [Ubuntu 20.04.2 LTS](https://releases.ubuntu.com/20.04/) (64-bit), and tested with an [Intel® Core™ i7-8565U CPU @ 1.80GHz × 8](https://ark.intel.com/content/www/us/en/ark/products/149091/intel-core-i7-8565u-processor-8m-cache-up-to-4-60-ghz.html). The code has been compiled via GCC, version 9.3.0 [(Ubuntu 9.3.0-17ubuntu1~20.04)](https://packages.ubuntu.com/focal/gcc-9).

### Process list and brief description

- [*main*](src/main.c): This is the main process, which launches all the other software. The duty of this piece of code is to load the 3 unnamed pipes and to launch all the involved processes (*S*, *P*, *G* and *L*)

- [*S*](src/S.c): This is the process that is used to communicate with the others. It is the only one that interfaces with the terminal. It can receive and handle 3 different commands: *start*, *pause*, *log*. In order to use it, you have to type e.g. `kill -18 22961`, where the first number is the signal chosen, whilst the second is the PID of the node *S* itself. This process then communicates with *P*, spreading user commands through all the children of *main*

- [*P*](src/P.c): This process is the computational core. It is also the nevralgic waypoint of communications: all other nodes involved are, in a way or another, bond to *P*. This process uses different constants based on `config.run_mode`. An arbitrary delay is added to the computation when `config.run_mode = 0`. `config.run_mode = 1` scenario has *P* expecting data from the G process of the previous PC in the chain

- [*G*](src/G.c): This process can be run in 2 modes: *Debug* mode (i.e. *Single machine* - `config.run_mode = 0`) or *Multi-machine* mode (i.e.communicating with other PCs - `config.run_mode = 1`). In the first case it receives tokens from *P*, and then sends them back to it. In the other scenario, it sends data to the P of the next PC in the chain

- [*L*](src/L.c): This process is the one responsible of logging. It registers every command issued by the user and every token processed by *P* (received/sent). When prompted, it opens the current log file via the user's preferred application

### Configuration file

The [config file](config) contains the list of all the parameters needed to tweak the code on the fly. All these elements are then saved in the `configuration` struct, which is declared in [def.h](src/def.h):

```c
struct configuration
{
    int run_mode;               // set to 0 to go in Debug mode (= Single-machine mode), to 1 for Multi-machine mode [default: 0]
    double rf;                  // sine wave frequency [default: 1.0]
    int waiting_time_microsecs; // waiting time, in microseconds, applied by process P before sending the updated token [default: 1000]
    char *next_ip;              // IP address of the next machine in the chain ("hostname -I" to get your current IP) [default: 192.168.1.106]
    int next_port;              // chosen remote port for the communication [default: 5000]
    char *fifo1;                // name of the first fifo (i.e. named pipe) [default: npipe1]
    char *fifo2;                // name of the second fifo (i.e. named pipe) [default: npipe2]
};
```

**Warning**: failing to open the config file may require to manually `kill` processes *S* and *L*.

### Run modes

The code can be run in 2 different modes, as suggested by `configuration.run_mode`. The first one, labelled *Single machine* mode or *Debug* mode, is the the one in which the *G* process is kept on the same machine as the others: in this case, it is possible to execute this code right out of the box. This is the default mode since the release of [Assignment Specifications V2.0](doc/Assignment%20Specifications%20V2.0.pdf). The *Single machine* mode features a richer integration between the processes *P* and *G*.

The other run mode, called *Multi-machine* mode, requires more than 1 PC to be tested. In this case, one has to send a copy of *G* and *def.h* (or simply the compiled version of that process) to the next PC in the chain. In addition, be sure to align *config* entries, or to send that file too.  
In return they should be sent a copy of the previous G process in the chain, along with required headers/config files.  
This version features a simpler token (i.e. just a `float` value inside a `char` array) in order to ease the interface with other chain members' codes.  
There are 5 mandatory elements that need to be adjusted in this scenario: the `config.run_mode` parameter itself, IP and port number of the next PC, and the 2 [named pipes](https://en.wikipedia.org/wiki/Named_pipe)' names, which are used to find a common mean of communication between foreign P-G processes without relying on [sockets](https://en.wikipedia.org/wiki/Network_socket).

### Log file

The log file gets saved in the main folder of the repository. It gets overwritten at each new run, if present, otherwise it gets created. A single entry of the log file looks like this:

```text
1626960952632212 | Thu Jul 22 15:35:52 2021 | from G | value | -0.319868
```

Where the first number is the the number of microseconds since the [Epoch](https://man7.org/linux/man-pages/man2/time.2.html), the second entry is the same data, but presented in a human-readable way (up to seconds precision), the third is the name of the process from which the data is coming from, the fourth is the type of information (*start*, *pause*, *value*), and the last one is the token value itself, if relevant (otherwise it is labeled as *undefined*).

### Input Terminal & Output Terminal

Upon execution, the code launches a second terminal, that will serve as the **Input Terminal**. The one in which *main* has been launched
is referred to as the **Output Terminal**.

The **Output Terminal** shows all the run-time prints, and welcomes the user with this message:

```console
[This is the Output Terminal, which shows the processes' outputs. Use the Input Terminal to send signals to the processes. Press Ctrl+C to end]
```

It also shows the [PID](https://en.wikipedia.org/wiki/Process_identifier) of *main*, *S*, *P*, *G* and *L*. For example:

```console
main: PID is 22960
G: my PID is 22962
S: my PID is 22961
L: my PID is 22964
P: my PID is 22963
```

All the PIDs are different for each new run.

**The Input Terminal** welcomes the user with these lines instead:

```console
[This is the Input Terminal, through which you can send signals to the running processes. Please also check the Output Terminal to inspect outputs]
```

It then also prints the list of available commands, while also conveniently reporting the correct PID to use. For instance, the Input Terminal goes on reporting something like this:

```console
Available commands:

kill -10 22961
kill -12 22961
kill -18 22961

10 is to open the log file, 12 to pause, 18 to resume. To end, press Ctrl + C in the Output Terminal
```

The dynamic part is the "22961", which gets updated every time the code is run.

**Warning**: be careful not to input a different [signal value](https://dsa.cs.tsinghua.edu.cn/oj/static/unix_signal.html) (other than `10`, `12`, and `18`), as it may break the correct program execution. If that happens, it may be needed to manually `kill` processes *P*, *G*, and *L*.

Please notice that asking for the log file to open at run-time automatically pauses token computation, so that the user can check its entries without being bothered by "document has changed" warnings and alike.

## 📊 Results

After several tests, I came to the conclusion that the token function reported by the professor does not work (this is true for both [Assignment Specifications V1.0](doc/Assignment%20Specifications%20V1.0.pdf) and [Assignment Specifications V2.0](doc/Assignment%20Specifications%20V2.0.pdf)).
It should produce a [sine wave](https://en.wikipedia.org/wiki/Sine) but, even with the default parameters, it does not even come close to that.
After several tweaks to the function itself, I ended up using one I defined myself. This formula, albeit less fancy than the original, works as intended, while still including all the parameters that were present in the provided one.

For the above mentioned reasons, I could not test the maximum `config.rf` value as expected by the goals of the assignment specifications. Of course, also the optional part (i.e. check whether token values constituted a sinusoidal or not) lost any sense, so I skipped that too.  
The only thing I could test was the waiting time, and I can confirm that my minumum waiting time is 0 (i.e. no waiting time), as even with that value the code behaves as expected. I proved this by adding a recursion element to my function (i.e. I added `10 * token.value` to the new token computation), so that I could then easily check in the [log file](https://github.com/andreabradpitto/ARP-assignment#log-file) that no message had been skipped.

Given these premises, results were satisfying nonetheless: my code works in both run modes, and does not seem to feature any relevant bug. The only known issues are highlighted by the **warnings** in this file, but they are mostly problems generated by users' errors, which are not handled seamlessly.

## 📫 Author

[Andrea Pitto](https://github.com/andreabradpitto) - s3942710@studenti.unige.it
