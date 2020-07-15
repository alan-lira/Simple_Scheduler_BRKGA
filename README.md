# Simple_Scheduler_BRKGA

    Universidade Federal Fluminense — UFF
    Instituto de Computação
    Pós-Graduação
    Inteligência Computacional
    Prof. Luiz Satoru Ochi
    2020.1

-----------------------------------------------
         TUTORIAL FOR LINUX USERS
-----------------------------------------------

**1) DEPENDENCIES**

   - Run the following instruction to get latest updates for packages and programs: **sudo apt update**


   - Run the following instruction to install 'make': **sudo apt install make**
-----------------------------------------------
**2) TUTORIAL**

  - **Clone (or Download ZIP) this repository**;


  - **Unzip it (necessary step if downloaded)**;


  - **Open this directory '.Simple_Scheduler_BRKGA/' into Terminal**;


  - Run the following instructions **to COMPILE**:


      - **cp makefile.txt Makefile**


      - **make clean**
 
 
      - **make**


  - Run the following instruction **to EXECUTE**:


      - **./SimpleSchedulerBRKGA**


-----------------------------------------------
**3) CUSTOMIZATIONS**

- To modify any of the BRKGA's input parameters or scheduling environment's parameters, **edit file BRKGA_and_Scheduler_Parameters.txt and save it before running!**

   - **N_TASKS = N**, where N refers to number of tasks to be scheduled.

   - **M_PROCESSORS = M**, where M refers to number of candidate processors that can execute those N tasks.

   - **PROCESSING_TIME_VECTOR_N_x_M = { {...} | {...} | ... | {...} }** refers to processing times spent by m ∈ {1, ..., M} processors to execute n ∈ {1, ..., N} tasks.

   - **COST_PER_UNIT_OF_TIME_VECTOR_M = { {...} | {...} | ... | {...} }** refers to monetary cost per unit of time related to the use of m ∈ {1, ..., M} processors.

   - **DECODING_STRATEGY = d**, where d refers to chosen decoding strategy to evaluate each chromosome belonging to the population.

   - **MAX_THREADS_DECODING = t**, where t refers to maximum number of threads decoding in parallel.

   - **P = p1**, where p1 refers to size of population (BRKGA's input parameter).

   - **Pe = p2**, where p2 refers to portion of elite population (BRKGA's input parameter).

   - **Pm = p3**, where p3 refers to portion of mutant population (BRKGA's input parameter).

   - **RHOa = p4**, where p4 refers to probability of inheriting the elite gene (BRKGA's input parameter).

   - **PI = p5**, where p5 refers to number of parallel population (BRKGA's input parameter).

   - **Kp = p6**, where p6 refers to frequency of population exchange (BRKGA's input parameter).

   - **Km = p7**, where p7 refers to permutation number of solutions (BRKGA's input parameter).

   - **STOPPING_CRITERION_MAX_GENERATIONS = p8**, where p8 refers to the implemented termination criteria of maximum number of generations (BRKGA's input parameter).

   - **EOF**, never remove this line :)

- **Randoms values for PROCESSING_TIME_VECTOR_N_x_M and COST_PER_UNIT_OF_TIME_VECTOR_M can be generated automatically**, just setting these parameters as follow:

   - PROCESSING_TIME_VECTOR_N_x_M = **RANDOM**
   - COST_PER_UNIT_OF_TIME_VECTOR_M = **RANDOM**

         Note: PROCESSING_TIME_VECTOR_N_x_M random values' range: [1, 30]. This can't be set without recompile.
               COST_PER_UNIT_OF_TIME_VECTOR_M random values' range: [1, 15]. This can't be set without recompile.

- **Customizable PROCESSING_TIME_VECTOR_N_x_M can be loaded, respecting the following 2D matrix pattern**:

   - PROCESSING_TIME_VECTOR_N_x_M = **{ {n1m1, n1m2, ..., n1mM} | {n2m1, n2m2, ..., n2mM} | ... | {nNm1, nNm2, ..., nNmM} }**

         Where M = M_PROCESSORS and N = N_TASKS.

- **Customizable COST_PER_UNIT_OF_TIME_VECTOR_M can be loaded, respecting the following 1D matrix pattern**:

   - COST_PER_UNIT_OF_TIME_VECTOR_M = **{ {m1} | {m2} | ... | {mM} }**

         Where M = M_PROCESSORS.

- **Decoding strategies can be chosen as follow**:

   - DECODING_STRATEGY = **d**

         if d = 0 then both implemented strategies will be used during execution.

         if d = 1 then only First Decoding Strategy will be used during execution.

         if d = 2 then only Second Decoding Strategy will be used during execution.

-----------------------------------------------
**4) EXAMPLES**

- Example of **balanced BRKGA's input parameters with small scheduling environment's parameters**:

      N_TASKS = 6
      M_PROCESSORS = 2
      PROCESSING_TIME_VECTOR_N_x_M = { {7, 8} | {3, 2} | {4, 6} | {2, 1} | {6, 3} | {12, 5} }
      COST_PER_UNIT_OF_TIME_VECTOR_M = { {8} | {12} }
      DECODING_STRATEGY = 0
      MAX_THREADS_DECODING = 1
      P = 100
      Pe = 0.10
      Pm = 0.10
      RHOa = 0.70
      PI = 1
      Kp = 100
      Km = 2
      STOPPING_CRITERION_MAX_GENERATIONS = 1000
      EOF
