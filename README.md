# ARP-assignment V2.0

## 📑 0 - Contents list

1. [Introduction](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-1---introduction)
2. [Repository structure](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-2---repository-structure)
3. [Software requirements](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-3---software-requirements)
4. [Installation](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-4---installation)
5. [Execution](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#%EF%B8%8F-5---execution)
6. [Additional information](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#%E2%84%B9%EF%B8%8F-6---additional-information)
7. [Results](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-7---results)
8. [Author](https://github.com/andreabradpitto/ARP-assignment/blob/main/README.md#-8---author)

## 📛 1 - Introduction

This is my repository for the assignment of Advanced and Robot Programming (ARP) Robotics Engineering course.  
There are 2 versions of the assignment specifications: [V1.0](doc/Assignment%20Specifications%20V1.0.pdf) and
[V2.0](doc/Assignment%20Specifications%20V2.0.pdf). This is because of the COVID-19 outbreak, which made the
first version unfeasible for health and safety reasons. My implementation is relative to the second and most
recent version, but I decided to still include the original specifications file for the sake of completeness.

## 📂 2 - Repository structure

- [doc](doc) folder: contains the 2 versions of the assignment specifications

- [src](src) folder: contains the source code of the assignment

- [makefile](makefile): file used to automatically build or delete executables and log files.
  See [Make](https://en.wikipedia.org/wiki/Make_(software)) for further information

- [.gitignore](.gitignore): hidden file that specifies which files and folder are not relevant for [Git](https://git-scm.com/)

- [LICENSE](LICENSE): a plain text file containing the license terms

- [README.md](README.md): this file

## ❗ 3 - Software requirements

- A [**POSIX**](https://en.wikipedia.org/wiki/POSIX)-compliant machine
- [gcc](https://gcc.gnu.org/)
- At least 1 MB of free disk space

The space required is intended for the repository contents + the compiled code. Please beware that running the code for an extended time span may produce much greater log file sizes.

## ✅ 4 - Installation

In order to create the executables, open a terminal, move to this folder, and then run:

```bash
make
```

The make file will take care of compiling all the code needed.  
If you want to remove the executables (and the log file), instead type:

```bash
make clean
```

## ▶️ 5 - Execution

To run the code, open a terminal, move to the folder where this repository is located, and then run:

```bash
./main
```

The main process will take care of launching all the other processes. You can stop everything by pressing Ctrl+C in the terminal where *main* has been launched. Notice that Ctrl+C is actually just the `SIGINT` **POSIX** signal.

## ℹ️ 6 - Additional information

This assignment has been coded in [Ubuntu 20.04.2 LTS](https://releases.ubuntu.com/20.04/) (64-bit), and tested with an [Intel® Core™ i7-8565U CPU @ 1.80GHz × 8](https://ark.intel.com/content/www/us/en/ark/products/149091/intel-core-i7-8565u-processor-8m-cache-up-to-4-60-ghz.html). The code has been compiled via gcc, version 9.3.0 [(Ubuntu 9.3.0-17ubuntu1~20.04)](https://packages.ubuntu.com/focal/gcc-9).  

Process list and brief description:

- [main](src/main.c): This is the main, which launches all the other processes. The duty of this piece of code
is to load configuration data and to launch all the involved processes (S, P, G and L)

- [S](src/S.c): This is the process that is used to communicate with the others. It is the only one that interfaces with the terminal.
It can receive and handle 3 different commands: start, pause, log. In order to use it, you have to type e.g. "kill -18 1234",
where the first number is the signal chosen, whilst the second is the PID of the node S itself.
This process then communicates with P, spreading user commands through all the children of main

- [P](src/P.c): This process is the computational core. It is also the nevralgic waypoint of communications:
all other nodes involved are, in a way or another, bond to P. This process uses different constants
based on RUN_MODE. A fake delay is added to the computation when RUN_MODE = 0. RUN_MODE = 1 scenario
has P expecting data from the G process of the previous PC in the chain

- [G](src/G.c): This process can be run in 2 modes: debug mode (i.e. single machine/covid/V2.0 - RUN_MODE = 0) or
normal mode (i.e.communicating with other PCs - RUN_MODE = 1). In the first case it receives tokens
from P, and then sends them back to it. In the other scenario, it sends data to the P of the next PC in the chain

- [L](src/L.c): This process is the one responsible of logging. It registers every command issued by the user
and every token processed by P (received/sent). When prompted, it opens the current log file via the user's preferred application

Upon execution, the code launches a second terminal, that will serve as the **Input Terminal**. The one in which *main* has been launched
is referred to as the **Output Terminal**.  
The **Output Terminal** shows all the run-time prints, and welcomes the user with this message:

```console
[This is the Output Terminal, which shows the processes' outputs. Use the Input Terminal to send signals to the processes. Press Ctrl+C to end]
```

It also shows the [PID](https://en.wikipedia.org/wiki/Process_identifier) of *main*, *S*, *P*, *G* and *L*.
  
**The Input Terminal** welcomes the user with these lines instead:

```console
[This is the Input Terminal, through which you can send signals to the running processes. Please also che the Output Terminal to inspect outputs]
```

It then also prints the list of available commands, while also conveniently reporting the correct PID to use. For instance, the Input Terminal goes on reporting something like this:

```console
Available commands:
  
kill -10 22961
kill -12 22961
kill -18 22961
  
10 is to output a log, 12 to pause, 18 to resume. To end, press Ctrl + C in the Output Terminal
```  

The dynamic part is the "22961", which gets updated every time the code is run.

## 📊 7 - Results

After several tests, I came to the conclusion that the token function reported by the professor ([Assignment Specifications V1.0](doc/Assignment%20Specifications%20V1.0.pdf) and [Assignment Specifications V2.0](doc/Assignment%20Specifications%20V2.0.pdf)) does not work.
It should produce a [sine wave](https://en.wikipedia.org/wiki/Sine) but, even with the default parameters, it does not even come close to that.
After several tweaks to the function itself, I ended up using ... TODO
Results TODO

## 📫 8 - Author

[Andrea Pitto](https://github.com/andreabradpitto) - s3942710@studenti.unige.it
