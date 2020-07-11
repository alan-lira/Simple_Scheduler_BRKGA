#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <vector>
#include <sstream>

#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "SimpleSchedulerDecoder.h"

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

   printf("Welcome to the SimpleSchedulerBRKGA.\n\n");

   // Tasks amount.
   int N_TASKS = 0;

   // Processors amount.
   int M_PROCESSORS = 0;

   // Size of chromosomes.
   unsigned N = 0;

   // Size of population.
   unsigned P = 0;

   // Fraction of population to be the elite-set.
   double PE = 0;

   // Fraction of population to be replaced by mutants.
   double PM = 0;

   // Probability that offspring inherit an allele from elite parent.
   double RHOE = 0;

   // Number of independent populations.
   unsigned K = 0;

   // Number of threads for parallel decoding.
   unsigned MAXT = 0;

   // Run for 1000 gens.
   unsigned MAX_GENS = 0;

   // Exchange best individuals at every 100 generations.
   unsigned X_INTVL = 0;

   // Exchange top 2 best.
   unsigned X_NUMBER = 0;

   // Initialization value for vectors.
   int initializationValue = 0;

   // Creation of processingTimesVector.
   std::vector< std::vector<int> > processingTimesVector;

   // Creation of costPerUnitOfTimeVector.
   std::vector<int> costPerUnitOfTimeVector;

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
         // Setting size of processingTimesVector.
         processingTimesVector.resize(N_TASKS, std::vector<int>(M_PROCESSORS, initializationValue));
         // Converting char array to string.
         std::string text(parameterValue);
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
               // Filling processingTimesVector (line N, column M).
               processingTimesVector[indexN][indexM] = atoi(vectorLineValues[indexM].c_str());
            }
         }
      } else if (strcmp(parameterName, "COST_PER_UNIT_OF_TIME_VECTOR_M") == 0) {
         // Setting size of costPerUnitOfTimeVector.
         costPerUnitOfTimeVector.resize(M_PROCESSORS, initializationValue);
         // Converting char array to string.
         std::string text(parameterValue);
         // Splitting by vector's line.
         std::vector<std::string> vectorLines = split(text, '|');
         // M Loop...
         for (unsigned int indexM = 0; indexM < vectorLines.size(); indexM++) {
            // Removing matrix garbage.
            replaceAll(vectorLines[indexM], "{", "");
            replaceAll(vectorLines[indexM], " ", "");
            replaceAll(vectorLines[indexM], "}", "");
            //printf("vectorLines[%d] = %s\n", indexM, vectorLines[indexM].c_str());
            // Filling costPerUnitOfTimeVector (column).
            costPerUnitOfTimeVector[indexM] = atoi(vectorLines[indexM].c_str());
         }
      } else if (strcmp(parameterName, "P") == 0) {
         // Setting P's value.
         P = atoi(parameterValue);
      } else if (strcmp(parameterName, "PE") == 0) {
         // Setting PE's value.
         PE = atof(parameterValue);
      } else if (strcmp(parameterName, "PM") == 0) {
         // Setting PM's value.
         PM = atof(parameterValue);
      } else if (strcmp(parameterName, "RHOE") == 0) {
         // Setting RHOE's value.
         RHOE = atof(parameterValue);
      } else if (strcmp(parameterName, "K") == 0) {
         // Setting K's value.
         K = atoi(parameterValue);
      } else if (strcmp(parameterName, "MAXT") == 0) {
         // Setting MAXT's value.
         MAXT = atoi(parameterValue);
      } else if (strcmp(parameterName, "MAX_GENS") == 0) {
         // Setting MAX_GENS's value.
         MAX_GENS = atoi(parameterValue);
      } else if (strcmp(parameterName, "X_INTVL") == 0) {
         // Setting X_INTVL's value.
         X_INTVL = atoi(parameterValue);
      } else if (strcmp(parameterName, "X_NUMBER") == 0) {
         // Setting X_NUMBER's value.
         X_NUMBER = atoi(parameterValue);
      }

   }

   fclose(parametersStream);

   if (lineContent) {
      free(lineContent);
   }

   // Setting N's value (size of chromosome).
   N = N_TASKS * M_PROCESSORS;

   // Initialize the SimpleSchedulerDecoder.
   SimpleSchedulerDecoder simpleSchedulerDecoder(N_TASKS, M_PROCESSORS, processingTimesVector, costPerUnitOfTimeVector);

   // Printing processingTimesVector.
   simpleSchedulerDecoder.printProcessingTimesVector();

   // Printing costPerUnitOfTimeVector.
   simpleSchedulerDecoder.printCostPerUnitOfTimeVector();

   // Seed to the random number generator.
   const long unsigned RNG_SEED = 0;

   // Initialize the random number generator.
   MTRand randomNumberGenerator(RNG_SEED);

   // Initialize the BRKGA-based heuristic.
   BRKGA <SimpleSchedulerDecoder, MTRand> algorithm(N, P, PE, PM, RHOE, simpleSchedulerDecoder, randomNumberGenerator, K, MAXT);

   // Current generation.
   unsigned generation = 0;

   printf("\nRunning for %d generations...\n\n", MAX_GENS);

   while (generation < MAX_GENS) {
      // Evolve the population for one generation.
      algorithm.evolve();
      if ((++generation) % X_INTVL == 0) {
         // Exchange top individuals.
         algorithm.exchangeElite(X_NUMBER);
      }
   }

   // Print the fitness of the top 10 individuals of each population.
   printf("Fitness of the top 10 individuals of each population:\n\n");

   // Makes sure we have at least 10 individuals.
   const unsigned bound = std::min(P, unsigned(10));

   for (unsigned i = 0; i < K; i++) {
      printf("Population #%d:\n", i);
      for (unsigned j = 0; j < bound; j++) {
         printf("\t%d) %.7f\n", j, algorithm.getPopulation(i).getFitness(j));
      }
      printf("\n");
   }

   printf("------- RESULTS -------\n\n");

   printf("Objective value (Best Solution): %f.\n\n", algorithm.getBestFitness());

   const std::vector<double> bestChromosome(algorithm.getBestChromosome());

   printf("|");
   for (unsigned int i = 0; i < bestChromosome.size(); i++) {
      printf(" %f |", bestChromosome[i]);
   }
   printf("\n");

   // Printing best task scheduling plan.
   simpleSchedulerDecoder.printTaskSchedulingPlan(bestChromosome);

   return 0;
}