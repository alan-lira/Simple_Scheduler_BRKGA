#include <iostream>
#include <stdio.h>

#include "SimpleSchedulerDecoder.h"

SimpleSchedulerDecoder::SimpleSchedulerDecoder(int tasksAmount,
                                               int processorsAmount,
                                               std::vector< std::vector<int> > processingTimeVector,
                                               std::vector<int> costPerUnitOfTimeVector)
                                               : tasksAmount(tasksAmount),
                                                 processorsAmount(processorsAmount),
                                                 processingTimeVector(processingTimeVector),
                                                 costPerUnitOfTimeVector(costPerUnitOfTimeVector) {
}

SimpleSchedulerDecoder::~SimpleSchedulerDecoder() {

}

double SimpleSchedulerDecoder::decode(const std::vector<double> &chromosome) const {

   double myFitness = 0.0;

   typedef std::pair<double, unsigned> ValueKeyPair;

   std::vector<ValueKeyPair> rank(chromosome.size());

   for (unsigned i = 0; i < chromosome.size(); i++) {
/*
      std::cout
         <<"VALOR DE chromosome[" <<i <<"]:" <<chromosome[i]
         <<
      std::endl;
*/
      rank[i] = ValueKeyPair(chromosome[i], i);
      myFitness += (double(i + 1) * chromosome[i]);
   }

   // Here we sort 'permutation', which will then produce a permutation of [n]
   // stored in ValueKeyPair::second:
   std::sort(rank.begin(), rank.end());

   // permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
   std::list<unsigned> permutation;

   for (std::vector<ValueKeyPair>::const_iterator i = rank.begin(); i != rank.end(); i++) {
      permutation.push_back(i->second);
   }

   // Return the fitness:
   return myFitness;
}

void SimpleSchedulerDecoder::printTaskSchedulingPlan(const std::vector<double> &candidate) const {
   printf("TASK SCHEDULER PLAN:\n");
   printf("TASKS AMOUNT = %d.\n", getTasksAmount());
   printf("PROCESSORS AMOUNT = %d.\n", getProcessorsAmount());
   for (unsigned i = 0; i < candidate.size(); i++) {
   }
}

int SimpleSchedulerDecoder::getTasksAmount() const {
   return tasksAmount;
}

int SimpleSchedulerDecoder::getProcessorsAmount() const {
   return processorsAmount;
}

std::vector< std::vector<int> > SimpleSchedulerDecoder::getProcessingTimeVector() const {
   return processingTimeVector;
}

void SimpleSchedulerDecoder::printProcessingTimeVector() const {
   for (unsigned int n = 0; n < getProcessingTimeVector().size(); n++) { // printf("LINHA %d.\n", n);
      for (unsigned int m = 0; m < getProcessingTimeVector()[n].size(); m++) { // printf("COLUNA %d.\n", m);
         // Tempos de processamento das tarefas n [0, ..., tasksCount] nas máquinas m [0, ..., processorCount].
         //printf("processingTimeVector[%d][%d] = %d.\n", n, m, getProcessingTimeVector()[n][m]);
         printf("A tarefa n%d é processada na máquina m%d em %d unidade(s) de tempo.\n", (n + 1), (m + 1), getProcessingTimeVector()[n][m]);
      }
      printf("\n");
   }
}

std::vector<int> SimpleSchedulerDecoder::getCostPerUnitOfTimeVector() const {
   return costPerUnitOfTimeVector;
}

void SimpleSchedulerDecoder::printCostPerUnitOfTimeVector() const {
   for (unsigned int m = 0; m < getCostPerUnitOfTimeVector().size(); m++) { // printf("COLUNA %d.\n", m);
      // Custo monetário de processamento por unidade de tempo na máquina m [0, ..., processorCount]:
      //printf("costPerUnitOfTimeVector[%d] = %d.\n", m, getCostPerUnitOfProcessingTimeVector()[m]);
      printf("A máquina m%d custa %d unidade(s) monetária(s) por unidade de tempo de processamento.\n", (m + 1), getCostPerUnitOfTimeVector()[m]);
   }
}
