# Exploring_Multiple_Processes_and-_Inter_Process_Communication

Project Purpose:

The purpose of the project is to make the students aware about the different processes that occur in a processor and how those processes communicate among themselves.
The process of communication between the processes is called IPC (Inter Process Communication) and this project focuses on its implementation as to how to write codes to carry out the same.
The project teaches about following important concepts:

1-	Forking: It is the process of spawning a new process from a given process. The new process is called the child process and the old one is called the parent process.

2-	 Pipes: Pipes are used for IPC. One end of the pipe is used for sending the input and the other end is used to receive the output.

3-	Some of the other concepts include topics like:

a)	Interaction between CPU and memory

b)	How processor handles instructions

c)	Role of registers like PC, SP, AC, X, Y, etc

d)	Stack processing, call procedures and system calls

e)	Interrupt handling

f)	Memory protection 

Project Implementation:

I wrote my project in C language. The implementation was carried as follows:

•	Use of two pipes for implementing duplex communication between the two processes

•	Parent process: Memory process is the parent process

It reads the output (which is input for child process) from pipec2m[0] and writes the output (which becomes input for the child process) at pipem2c[1]

•	Child process: CPU process is the child process

It writes the output (which is input for parent process) at pipec2m[1] and reads the input (which is output for parent process) from pipem2c[0]

•	The two inputs: .txt file and timer value are taken as command line arguments

An integer ‘instnum’ keeps track of the number of instructions which have been executed only during ‘user mode’ operation

•	An integer array with size 2000 is used to represent memory

•	There are 2 modes of operation:

1-	User mode: When program is running and only the memory from 0 to 999 can be accessed

2-	System/Kernel mode: When ISR (Interrupt Service Routine) is running and the memory from 1000 to 1999 can be accessed

How To Execute:

1- Compile using:
	gcc IPC.c -o IPC
2- Run:
	./IPC sample5.txt 20

Note:

- The value of the timer can be chosen arbitrarily. 
