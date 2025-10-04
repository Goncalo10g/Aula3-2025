[README.md](README.md)# Scheduler Simulator

The objective of this project is to simulate various CPU scheduling algorithms and
analyze their performance based on different metrics. The simulator allows users to
input a set of processes with their respective arrival times, burst times, and priorities,
and then applies different scheduling algorithms to determine the order of execution.

## Message Format
Each message sends the application PID, the message request type and a time parameter.
Since we are using Unix Domain Sockets (sender and receiver on the same machine), we can
safely pass structs between the application and the simulator.

### Messages from the application to the simulator:
The messages from the application to the simulator (RUN/BLOCK) send the time in ms
that the process requests the CPU or the I/O device.
Although this is not completely realistic, it simplifies the implementation of the simulator
and allows us to focus on the scheduling algorithms.

### Messages from the simulator to the application:
The messages from the simulator to the application (ACK/EXIT) send the current time in ms
in the simulation ("wall clock"). This allows the application to keep track of the time even if
we take some time debugging the code.

## Time Diagram
The time diagram below illustrates the interaction between the application and the simulator:

```
Simulator                           Applications
   |                                    |
   | <---- App1 RUN (run time) -------- |
   |                                    |
   | ---- App1 ACK (current time) ----> |
   |                                    |
   | <---- App2 RUN (run time) -------- |
   |                                    |
   | ---- App2 ACK (current time) ----> |
   |                                    |
   | ---- App1 DONE (current time) ---> |   --> App 1 prints its statistics and terminates
   |                                    |
   | ---- App2 DONE (current time) ---> |   --> App 2 prints its statistics and terminates
   |
 (keeps running)
   ```

## Scheduling Algorithms

### FIFO (First In First Out)
The FIFO scheduling algorithm processes tasks in the order they arrive. The first task to arrive is the
first to be executed.
This is already implemented in the simulator.

### SJF (Shortest Job First)
The SJF scheduling algorithm selects the task with the shortest burst time to execute next.

### Round Robin
The Round Robin scheduling algorithm assigns a fixed time slice to each task in the queue. Each task
is executed for a maximum of the time slice before being moved to the back of the queue.
In the simulator, create a first version of Round Robin with a time slice of 1s.

### MLFQ (Multi-Level Feedback Queue)
The MLFQ scheduling algorithm uses multiple queues with different priority levels. The app to be used
here is app-pre, which not only sends burst times, but also block times. The app-pre has a filename as
command line argument, which contains on each line the burst time and the block time (in ms) of each cycle.

Hint: The diagram used here is slightly different from the one used in class, as it includes not only RUN
messages, but also BLOCK messages. The BLOCK messages are used to simulate I/O operations.

```
Simulator                           Applications
   |                                    |
   | <---- App1 RUN (run time) -------- |
   |                                    |
   | ---- App1 ACK (current time) ----> |
   |                                    |
   | <---- App2 RUN (run time) -------- |
   |                                    |
   | ---- App2 ACK (current time) ----> |
   |                                    |
   | ---- App1 DONE (current time) ---> | 
   |                                    |
   | <---- App1 BLOCK (run time) ------ |
   |                                    |
   | ---- App1 ACK (current time) ----> |
   |                                    |
   | ---- App2 DONE (current time) ---> | 
   |                                    |
   | <---- App2 BLOCK (run time) ------ |
   |                                    |
   | ---- App2 ACK (current time) ----> |
   |                                    |
   | ---- App1 DONE (current time) ---> | 
   |                                    |
   | ---- App2 DONE (current time) ---> | 
```


# Getting started

To compile the simulator and the applications you can use CLion, 
which is set up to compile these on our Linux based virtual machine.

Running the simulator requires you to run the ossim (scheduler) app and the app (applications app).
Since we want to run multiple applications at the same time, it is easiest to set up a shell script,
such as run_apps.sh.

Open a remote terminal to the Linux VM, navigate to the scheduler_examples directory and enter the
cmake-build-debug folder, where CLion stores the binaries of the application. 

Copy or move the run_apps.sh file to this directory, make it executable and edit it to match your needs.

```bash
chmod +x run_apps.sh
cp run_apps.sh cmake-build-debug/.
```

Now open a second remote terminal en enter the cmake-build-debug directory. In this
terminal you will run the simulator (ossim), like so:

```bash
./ossim FIFO
```
This will start the simulator with the FIFO scheduling algorithm.

After starting the simulator, switch to the first terminal and run the run_apps.sh script:

```bash
./run_apps.sh
```
This will start the applications, which will connect to the simulator and start sending
messages.

You can edit the run_apps.sh script to change the scheduling algorithm used by the simulator
and the applications to be run. You can also add more applications or change the parameters
of the existing ones.

## Note for MLFQ
To test the MLFQ scheduling algorithm, you need to use the app-io application, which requires
a filename as command line argument. This file should contain on each line the burst time and
the block time (in ms) of each cycle. You can create multiple files with different parameters
to test the MLFQ algorithm.

We have provided A-5.csv, B-5.csv and C-5.csv in this directory, which you can use to test the MLFQ algorithm.
You can edit the run_apps.sh script to use these files as arguments for the app-io application.
Make sure to provide the correct path to these files, for instance:

```
./app-io ../A-5.csv &
```
