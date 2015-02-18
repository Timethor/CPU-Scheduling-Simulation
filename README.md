CPU-Scheduler-Simulator
=======================


Author
------
Nicholas \`Timethor\` Rich

Compilation & Running
---------------------

If the files you recieve are not already, make sure everything has permission 755. This can be done with `chmod +x *` typically.

There are two files named `make.sh` and `run.sh`. Run `make.sh` first to compile the simulator, Then run `run.sh` without arguments to see a usage detail.

- Typical Usage: `./run.sh Input_File` - 
An output file named Input_File.result will be created as well as 

- Typical Usage with Specified Output File: `./run.sh Input_File Output_File`

- Verbose Usage: `./run.sh Input_File Output_File -v [4-8]`

- File-Only Ouput: `./run.sh -f Input_File Output_File`


Summary of known problems
-------------------------
- Occasionally the system takes a little bit to catch up and will print this twice: `I/O Device X now in use by Process Y`
At the end of the time window
- 8 BurstNode's are not freed at the end of execution despite quite a decent bit of time trying to figure out where I neglect to free them. I know they are malloc'ed in the SS_processLineForProcessSchedule function of the LineInterpreter.c file. Look there for a fun and relevant movie reference.
