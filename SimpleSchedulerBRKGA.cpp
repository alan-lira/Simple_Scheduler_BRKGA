#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <vector>
#include <sstream>
#include <random>
#include <chrono>

#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "SimpleSchedulerDecoder.h"

int getRandomInteger(int min, int max) {
   std::random_device rd; // Only used once to initialise (seed) engine.
   std::mt19937 rng(rd()); // Random-number engine used (Mersenne-Twister in this case).
   std::uniform_int_distribution<int> uni(min, max); // Guaranteed unbiased.
   return uni(rng);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter)) {
      tokens.push_back(token);
   }
   return tokens;
}

void replaceAll(std::string &str, const std::string &from, const std::string &to) {
   if (from.empty()) {
      return;
   }
   size_t start_pos = 0;
   while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length();
   }
}

int main() {

   printf("Welcome to the Simple Scheduler BRKGA.\n\n");

   // Tasks amount.
   int N_TASKS = 0;

   // Processors amount.
   int M_PROCESSORS = 0;

   // Creation of PROCESSING_TIME_VECTOR_N_x_M.
   std::vector< std::vector<int> > PROCESSING_TIME_VECTOR_N_x_M;

   // Creation of COST_PER_UNIT_OF_TIME_VECTOR_M.
   std::vector<int> COST_PER_UNIT_OF_TIME_VECTOR_M;

   // Decoding strategy to be used by SimpleSchedulerDecoder.
   int DECODING_STRATEGY = 0;

   // Number of threads for parallel decoding.
   unsigned MAX_THREADS_DECODING = 0;

   // Size of population.
   unsigned P = 0;

   // Fraction of population to be the elite-set.
   double Pe = 0;

   // Fraction of population to be replaced by mutants.
   double Pm = 0;

   // Probability that offspring inherit an allele from elite parent.
   double RHOa = 0;

   // Number of independent populations.
   unsigned PI = 0;

   // Exchange best individuals at every Kp generations.
   unsigned Kp = 0;

   // Exchange top Km best.
   unsigned Km = 0;

   // Stopping Criterion for Maximum Generations.
   unsigned STOPPING_CRITERION_MAX_GENERATIONS = 0;

   // Loading BRKGA and Scheduler parameters:

   FILE *parametersStream = fopen("BRKGA_and_Scheduler_Parameters.txt", "r");

   if (parametersStream == NULL) {
      printf("Error openning 'BRKGA_and_Scheduler_Parameters.txt' file.\n");
      exit(EXIT_FAILURE);
   }

   regex_t regexInt;

   int regexIntAux = regcomp(&regexInt, "([a-zA-Z_-]*)\\s=\\s([0-9]*)\n", REG_EXTENDED);

   if (regexIntAux) {
      printf("Could not compile regexInt!\n");
      exit(EXIT_FAILURE);
   }

   regex_t regexDouble;

   int regexDoubleAux = regcomp(&regexDouble, "([a-zA-Z_-]*)\\s=\\s([0-9]\\.[0-9]*)\n", REG_EXTENDED);

   if (regexDoubleAux) {
      printf("Could not compile regexDoubleAux!\n");
      exit(EXIT_FAILURE);
   }

   regex_t regexCharArray;

   int regexCharArrayAux = regcomp(&regexCharArray, "([a-zA-Z_-]*)\\s=\\s((.*))\n", REG_EXTENDED);

   if (regexCharArrayAux) {
      printf("Could not compile regexCharArrayAux!\n");
      exit(EXIT_FAILURE);
   }

   regmatch_t matchLine[3];

   char parameterName[100];
   char parameterValue[500];

   char *lineContent = NULL;

   size_t len = 0;

   while ((getline(&lineContent, &len, parametersStream)) != -1) {

      regexIntAux = regexec(&regexInt, lineContent, 13, matchLine, 0);

      if (!regexIntAux) {
         memset(parameterValue, '\0', sizeof(parameterValue));
         memcpy(parameterValue, lineContent + matchLine[2].rm_so, (matchLine[2].rm_eo - matchLine[2].rm_so));
      }

      regexDoubleAux = regexec(&regexDouble, lineContent, 13, matchLine, 0);

      if (!regexDoubleAux) {
         memset(parameterValue, '\0', sizeof(parameterValue));
         memcpy(parameterValue, lineContent + matchLine[2].rm_so, (matchLine[2].rm_eo - matchLine[2].rm_so));
      }

      regexCharArrayAux = regexec(&regexCharArray, lineContent, 13, matchLine, 0);

      if (!regexCharArrayAux) {
         memset(parameterValue, '\0', sizeof(parameterValue));
         memcpy(parameterValue, lineContent + matchLine[2].rm_so, (matchLine[2].rm_eo - matchLine[2].rm_so));
      }

      memset(parameterName, '\0', sizeof(parameterName));
      memcpy(parameterName, lineContent + matchLine[1].rm_so, (matchLine[1].rm_eo - matchLine[1].rm_so));

      if (strcmp(parameterName, "N_TASKS") == 0) {
         // Setting N_TASKS' value.
         N_TASKS = atoi(parameterValue);
      } else if (strcmp(parameterName, "M_PROCESSORS") == 0) {
         // Setting M_PROCESSORS' value.
         M_PROCESSORS = atoi(parameterValue);
      } else if (strcmp(parameterName, "PROCESSING_TIME_VECTOR_N_x_M") == 0) {
         // Initialization value for PROCESSING_TIME_VECTOR_N_x_M.
         int initializationValue = 0;
         // Setting size of PROCESSING_TIME_VECTOR_N_x_M.
         PROCESSING_TIME_VECTOR_N_x_M.resize(N_TASKS, std::vector<int>(M_PROCESSORS, initializationValue));
         // Converting char array to string.
         std::string text(parameterValue);
         // Veryfing if Filling is RANDOM.
         if (strcmp(parameterValue, "RANDOM") == 0) {
            // N Loop...
            for (int indexN = 0; indexN < N_TASKS; indexN++) {
               // M Loop...
               for (int indexM = 0; indexM < M_PROCESSORS; indexM++) {
                  int minTNM = 1; // Minimum interval range's value.
                  int maxTNM = 30; // Maximum interval range's value.
                  // Filling PROCESSING_TIME_VECTOR_N_x_M (line N, column M) with random integers.
                  PROCESSING_TIME_VECTOR_N_x_M[indexN][indexM] = getRandomInteger(minTNM, maxTNM);
               }
            }
         } else {
            // Splitting by vector's line.
            std::vector<std::string> vectorLines = split(text, '|');
            // N Loop...
            for (unsigned int indexN = 0; indexN < vectorLines.size(); indexN++) {
               // Removing matrix garbage.
               replaceAll(vectorLines[indexN], "{", "");
               replaceAll(vectorLines[indexN], " ", "");
               replaceAll(vectorLines[indexN], "}", "");
               //printf("vectorLines[%d] = %s\n", indexN, vectorLines[indexN].c_str());
               // Splitting by vector's column.
               std::vector<std::string> vectorLineValues = split(vectorLines[indexN], ',');
               // M Loop...
               for (unsigned int indexM = 0; indexM < vectorLineValues.size(); indexM++) {
                  //printf("INDEX_N_M (%d_%d): %s\n", indexN, indexM, vectorLineValues[indexM].c_str());
                  // Filling PROCESSING_TIME_VECTOR_N_x_M (line N, column M).
                  PROCESSING_TIME_VECTOR_N_x_M[indexN][indexM] = atoi(vectorLineValues[indexM].c_str());
               }
            }
         }
      } else if (strcmp(parameterName, "COST_PER_UNIT_OF_TIME_VECTOR_M") == 0) {
         // Initialization value for COST_PER_UNIT_OF_TIME_VECTOR_M.
         int initializationValue = 0;
         // Setting size of COST_PER_UNIT_OF_TIME_VECTOR_M.
         COST_PER_UNIT_OF_TIME_VECTOR_M.resize(M_PROCESSORS, initializationValue);
         // Converting char array to string.
         std::string text(parameterValue);
         // Veryfing if Filling is RANDOM.
         if (strcmp(parameterValue, "RANDOM") == 0) {
            // M Loop...
            for (int indexM = 0; indexM < M_PROCESSORS; indexM++) {
               int minCM = 1; // Minimum interval range's value.
               int maxCM = 15; // Maximum interval range's value.
               // Filling COST_PER_UNIT_OF_TIME_VECTOR_M (column) with random integers.
               COST_PER_UNIT_OF_TIME_VECTOR_M[indexM] = getRandomInteger(minCM, maxCM);
            }
         } else {
            // Splitting by vector's line.
            std::vector<std::string> vectorLines = split(text, '|');
            // M Loop...
            for (unsigned int indexM = 0; indexM < vectorLines.size(); indexM++) {
               // Removing matrix garbage.
               replaceAll(vectorLines[indexM], "{", "");
               replaceAll(vectorLines[indexM], " ", "");
               replaceAll(vectorLines[indexM], "}", "");
               //printf("vectorLines[%d] = %s\n", indexM, vectorLines[indexM].c_str());
               // Filling COST_PER_UNIT_OF_TIME_VECTOR_M (column).
               COST_PER_UNIT_OF_TIME_VECTOR_M[indexM] = atoi(vectorLines[indexM].c_str());
            }
         }
      } else if (strcmp(parameterName, "DECODING_STRATEGY") == 0) {
         // Setting DECODING_STRATEGY's value.
         DECODING_STRATEGY = atoi(parameterValue);
      } else if (strcmp(parameterName, "MAX_THREADS_DECODING") == 0) {
         // Setting MAX_THREADS_DECODING's value.
         MAX_THREADS_DECODING = atoi(parameterValue);
      } else if (strcmp(parameterName, "P") == 0) {
         // Setting P's value.
         P = atoi(parameterValue);
      } else if (strcmp(parameterName, "Pe") == 0) {
         // Setting Pe's value.
         Pe = atof(parameterValue);
      } else if (strcmp(parameterName, "Pm") == 0) {
         // Setting Pm's value.
         Pm = atof(parameterValue);
      } else if (strcmp(parameterName, "RHOa") == 0) {
         // Setting RHOa's value.
         RHOa = atof(parameterValue);
      } else if (strcmp(parameterName, "PI") == 0) {
         // Setting PI's value.
         PI = atoi(parameterValue);
      } else if (strcmp(parameterName, "Kp") == 0) {
         // Setting Kp's value.
         Kp = atoi(parameterValue);
      } else if (strcmp(parameterName, "Km") == 0) {
         // Setting Km's value.
         Km = atoi(parameterValue);
      } else if (strcmp(parameterName, "STOPPING_CRITERION_MAX_GENERATIONS") == 0) {
         // Setting STOPPING_CRITERION_MAX_GENERATIONS's value.
         STOPPING_CRITERION_MAX_GENERATIONS = atoi(parameterValue);
      }

   }

   fclose(parametersStream);

   if (lineContent) {
      free(lineContent);
   }

   // Execute all decoding strategies available (implemented).
   if (DECODING_STRATEGY == 0) {

      // Execute DECODING_STRATEGY 1.
      {

         std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

         // Size n of chromosomes.
         unsigned n = 0;

         // Forcing execution of DECODING_STRATEGY 1.
         DECODING_STRATEGY = 1;

         // Setting n's value (size of chromosome).
         if (DECODING_STRATEGY == 1) {
            n = 2 * N_TASKS;
         } else {
            n = N_TASKS;
         }

         // Initialize the SimpleSchedulerDecoder.
         SimpleSchedulerDecoder simpleSchedulerDecoder(N_TASKS, M_PROCESSORS, PROCESSING_TIME_VECTOR_N_x_M, COST_PER_UNIT_OF_TIME_VECTOR_M, DECODING_STRATEGY);

         // Printing PROCESSING_TIME_VECTOR_N_x_M.
         //simpleSchedulerDecoder.printProcessingTimesVector();

         // Printing COST_PER_UNIT_OF_TIME_VECTOR_M.
         //simpleSchedulerDecoder.printCostPerUnitOfTimeVector();

         // Seed to the random number generator.
         const long unsigned RNG_SEED = 0;

         // Initialize the random number generator.
         MTRand randomNumberGenerator(RNG_SEED);

         // Initialize the BRKGA-based heuristic.
         BRKGA <SimpleSchedulerDecoder, MTRand> algorithm(n, P, Pe, Pm, RHOa, simpleSchedulerDecoder, randomNumberGenerator, PI, MAX_THREADS_DECODING);

         // Current generation.
         unsigned generation = 0;

         printf("\nRunning for %d generations...\n\n", STOPPING_CRITERION_MAX_GENERATIONS);

         while (generation < STOPPING_CRITERION_MAX_GENERATIONS) {
            // Evolve the population for one generation.
            algorithm.evolve();
            if ((++generation) % Kp == 0) {
               // Exchange top individuals.
               algorithm.exchangeElite(Km);
            }
         }

         // Print the fitness of the top 10 individuals of each population.
         printf("Fitness of the top 10 individuals of each population:\n\n");

         // Makes sure we have at least 10 individuals.
         const unsigned bound = std::min(P, unsigned(10));

         for (unsigned i = 0; i < PI; i++) {
            printf("Population #%d:\n", i);
            for (unsigned j = 0; j < bound; j++) {
               printf("\t%d) %.7f\n", j, algorithm.getPopulation(i).getFitness(j));
            }
            printf("\n");
         }

         std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

         printf("------- RESULTS -------\n\n");

         printf("Decoding strategy: %d.\n\n", DECODING_STRATEGY);

         printf("Elapsed time: %f second(s).\n\n", (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0);

         printf("Objective value (Best Solution): %f.\n\n", algorithm.getBestFitness());

         // Printing best tasks scheduling's plan.
         simpleSchedulerDecoder.printTaskSchedulingPlan();

      }

      // Execute DECODING_STRATEGY 2.
      {

         std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

         // Size n of chromosomes.
         unsigned n = 0;

         // Forcing execution of DECODING_STRATEGY 2.
         DECODING_STRATEGY = 2;

         // Setting n's value (size of chromosome).
         if (DECODING_STRATEGY == 1) {
            n = 2 * N_TASKS;
         } else {
            n = N_TASKS;
         }

         // Initialize the SimpleSchedulerDecoder.
         SimpleSchedulerDecoder simpleSchedulerDecoder(N_TASKS, M_PROCESSORS, PROCESSING_TIME_VECTOR_N_x_M, COST_PER_UNIT_OF_TIME_VECTOR_M, DECODING_STRATEGY);

         // Printing PROCESSING_TIME_VECTOR_N_x_M.
         //simpleSchedulerDecoder.printProcessingTimesVector();

         // Printing COST_PER_UNIT_OF_TIME_VECTOR_M.
         //simpleSchedulerDecoder.printCostPerUnitOfTimeVector();

         // Seed to the random number generator.
         const long unsigned RNG_SEED = 0;

         // Initialize the random number generator.
         MTRand randomNumberGenerator(RNG_SEED);

         // Initialize the BRKGA-based heuristic.
         BRKGA <SimpleSchedulerDecoder, MTRand> algorithm(n, P, Pe, Pm, RHOa, simpleSchedulerDecoder, randomNumberGenerator, PI, MAX_THREADS_DECODING);

         // Current generation.
         unsigned generation = 0;

         printf("\nRunning for %d generations...\n\n", STOPPING_CRITERION_MAX_GENERATIONS);

         while (generation < STOPPING_CRITERION_MAX_GENERATIONS) {
            // Evolve the population for one generation.
            algorithm.evolve();
            if ((++generation) % Kp == 0) {
               // Exchange top individuals.
               algorithm.exchangeElite(Km);
            }
         }

         // Print the fitness of the top 10 individuals of each population.
         printf("Fitness of the top 10 individuals of each population:\n\n");

         // Makes sure we have at least 10 individuals.
         const unsigned bound = std::min(P, unsigned(10));

         for (unsigned i = 0; i < PI; i++) {
            printf("Population #%d:\n", i);
            for (unsigned j = 0; j < bound; j++) {
               printf("\t%d) %.7f\n", j, algorithm.getPopulation(i).getFitness(j));
            }
            printf("\n");
         }

         std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

         printf("------- RESULTS -------\n\n");

         printf("Decoding strategy: %d.\n\n", DECODING_STRATEGY);

         printf("Elapsed time: %f second(s).\n\n", (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0);

         printf("Objective value (Best Solution): %f.\n\n", algorithm.getBestFitness());

         // Printing best tasks scheduling's plan.
         simpleSchedulerDecoder.printTaskSchedulingPlan();

      }

   // Execute DECODING_STRATEGY from BRKGA_and_Scheduler_Parameters.txt file.
   } else {

      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

      // Size n of chromosomes.
      unsigned n = 0;

      // Setting n's value (size of chromosome).
      if (DECODING_STRATEGY == 1) {
         n = 2 * N_TASKS;
      } else {
         n = N_TASKS;
      }

      // Initialize the SimpleSchedulerDecoder.
      SimpleSchedulerDecoder simpleSchedulerDecoder(N_TASKS, M_PROCESSORS, PROCESSING_TIME_VECTOR_N_x_M, COST_PER_UNIT_OF_TIME_VECTOR_M, DECODING_STRATEGY);

      // Printing PROCESSING_TIME_VECTOR_N_x_M.
      //simpleSchedulerDecoder.printProcessingTimesVector();

      // Printing COST_PER_UNIT_OF_TIME_VECTOR_M.
      //simpleSchedulerDecoder.printCostPerUnitOfTimeVector();

      // Seed to the random number generator.
      const long unsigned RNG_SEED = 0;

      // Initialize the random number generator.
      MTRand randomNumberGenerator(RNG_SEED);

      // Initialize the BRKGA-based heuristic.
      BRKGA <SimpleSchedulerDecoder, MTRand> algorithm(n, P, Pe, Pm, RHOa, simpleSchedulerDecoder, randomNumberGenerator, PI, MAX_THREADS_DECODING);

      // Current generation.
      unsigned generation = 0;

      printf("\nRunning for %d generations...\n\n", STOPPING_CRITERION_MAX_GENERATIONS);

      while (generation < STOPPING_CRITERION_MAX_GENERATIONS) {
         // Evolve the population for one generation.
         algorithm.evolve();
         if ((++generation) % Kp == 0) {
            // Exchange top individuals.
            algorithm.exchangeElite(Km);
         }
      }

      // Print the fitness of the top 10 individuals of each population.
      printf("Fitness of the top 10 individuals of each population:\n\n");

      // Makes sure we have at least 10 individuals.
      const unsigned bound = std::min(P, unsigned(10));

      for (unsigned i = 0; i < PI; i++) {
         printf("Population #%d:\n", i);
         for (unsigned j = 0; j < bound; j++) {
            printf("\t%d) %.7f\n", j, algorithm.getPopulation(i).getFitness(j));
         }
         printf("\n");
      }

      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

      printf("------- RESULTS -------\n\n");

      printf("Decoding strategy: %d.\n\n", DECODING_STRATEGY);

      printf("Elapsed time: %f second(s).\n\n", (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0);

      printf("Objective value (Best Solution): %f.\n\n", algorithm.getBestFitness());

      // Printing best tasks scheduling's plan.
      simpleSchedulerDecoder.printTaskSchedulingPlan();

   }

   return 0;
}
