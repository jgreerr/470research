Set Up

**Installation with GUI**
The original version of the model can be used if utilizing GUI to configure the model. It can be
downloaded from the University of Hamburg’s website. If using GUI on the Cluster, it must be
connected to with this command (assuming there is an alias for the cluster setup):
ssh -Y cluster
Create a new folder and download the tarball into it. After navigating into the folder with the tarbell,
the GUI can be ran with:
tar -zxvf plasim0318.tgz
./configure.sh
./most.x

**Running with GUI**
This sequence of commands will run a GUI to enable configuring the model. This includes fields such
as core count, simulation length, and atmospheric levels. After configuring the desired fields, the user
must click Save and Run to configure and begin execution of the model.

**Installation with no GUI**
If running the no GUI version of the program, clone the repository for this project. The project
repository contains a version of the model with the required files to run the application. A specific
version of MPI is required to run the program on the slurm job scheduling service. Use the following
command to load it on the Cluster:
module load mpi/openmpi-x86_64

**Running with no GUI**
Before running nogui.x, it must be compiled using the following command:
gcc nogui.c -o nogui.x
The program has several options, they are listed below:
• -n is used to specify the number of cores used in the execution of the program. This is directly
passed to the srun command, so if it exceeds 16 multiple nodes will be created.
• -l determines the levels of the atmosphere, this is one axis of the three dimensional data plane
that controls how much data each process needs to generate.
• -y is the number of years that the simulation will run for. An executable is generated for each
year, and they get run in a sequential order.
• -e is the name of the experiment, which is the name of the file that is generated at the end of
execution. By default it is ”MOST”.
• -r tells the executable to immediately run the program after generating the executable. When
running a batch of tests, it is important to turn this off so that the execution of the program can
be waited on.
