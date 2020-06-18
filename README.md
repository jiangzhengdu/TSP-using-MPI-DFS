# TSP-using-MPI-DFS
Soving TSP using MPI&amp;&amp;DFS

Ubuntu 18.04

How to compile the source code with mpi: mpicc  -g  -Wall  -o  mpi_hello  mpi_hello.c

How to excute the program: mpiexec -n <number> ./mpi_hello 
  
number means how many processes u want to use

By the way ,if u want to compile the c source code without mpi just : gcc -g -Wall -o xxx xxx.c



ATTENTION!   FOR each parallel file ,the number of process depends on the the node_num , the boradcast file ,the n should be 

node_num-1  and for the reduction file ,the n should be node_num


Anyway if u have any question , please contact me through wechat:17321066897  or mail Du-William@outlook.com or just open an issue
