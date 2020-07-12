#include <iostream>
#include <stdio.h>
#include<tuple>
#include<vector>
#include<set>
#include<random>

#include "SimpleSchedulerDecoder.h"

SimpleSchedulerDecoder::SimpleSchedulerDecoder(int tasksAmount,
                                               int processorsAmount,
                                               std::vector<std::vector<int> > processingTimesVector,
                                               std::vector<int> costPerUnitOfTimeVector,
                                               int schedulingStrategy)
                                               : tasksAmount(tasksAmount),
                                                 processorsAmount(processorsAmount),
                                                 processingTimesVector(processingTimesVector),
                                                 costPerUnitOfTimeVector(costPerUnitOfTimeVector),
                                                 schedulingStrategy(schedulingStrategy),
                                                 processorsRandomKeyIntervalVector(),
                                                 schedulingPlanVector() {
   // Setting processors' random-key interval.
   processorsRandomKeyIntervalVector = SimpleSchedulerDecoder::setProcessorsRandomKeyInterval();
}

SimpleSchedulerDecoder::~SimpleSchedulerDecoder() {

}

bool compareFirstStrategy(const std::tuple<int, double, double> &i, const std::tuple<int, double, double> &j) {
   return std::get<1>(i) < std::get<1>(j);
}

bool compareSecondStrategy(const std::tuple<int, double> &i, const std::tuple<int, double> &j) {
   return std::get<1>(i) < std::get<1>(j);
}

bool allTasksScheduled(std::vector<int> &E) {
   bool result = true;
   for (unsigned int i = 0; i < E.size(); i++) {
      if (E[i] == 0) {
         result = false;
         break;
      }
   }
   return result;
}

void selectTask(std::vector<int> &E, int taskID) {
   E[taskID] = 1;
}

bool processorBusy(std::vector<std::vector<int> > &A, int processID) {
   return A[processID][0] != 0;
}

bool allProcessorsBusy(std::vector<std::vector<int> > &A) {
   bool result = true;
   for (unsigned int i = 0; i < A.size(); i++) {
      if (A[i][0] == 0) {
         result = false;
         break;
      }
   }
   return result;
}

bool anyProcessorBusy(std::vector<std::vector<int> > &A) {
   bool result = false;
   for (unsigned int i = 0; i < A.size(); i++) {
      if (A[i][0] != 0) {
         result = true;
         break;
      }
   }
   return result;
}

void verifyReturningProcessors(std::vector< std::vector<int> > &A, int T) {
   for (unsigned int i = 0; i < A.size(); i++) {
      if (A[i][0] != 0 && A[i][0] <= T) {
         A[i][0] = 0;
      }
   }
}

int SimpleSchedulerDecoder::searchRandomAvailableProcessor(std::vector<std::vector<int> > &A) const {
   std::random_device rd; // Only used once to initialise (seed) engine.
   std::mt19937 rng(rd()); // Random-number engine used (Mersenne-Twister in this case).
   int min = 0; // Minimum interval range's value.
   int max = getProcessorsAmount() - 1; // Maximum interval range's value.
   std::uniform_int_distribution<int> uni(min, max); // Guaranteed unbiased.
   bool successful = false;
   int randomProcessorID;
   while (!successful) {
      randomProcessorID = uni(rng);
      bool isProcessBusy = processorBusy(A, randomProcessorID);
      if (!isProcessBusy) {
         successful = true;
      }
   }
   return randomProcessorID;
}

void selectProcessor(std::vector< std::vector<int> > &A, int processID, int coolDown) {
   A[processID][0] = coolDown;
}

int SimpleSchedulerDecoder::calculateSelectedProcessorCoolDown(int idProcessorSelected, int idTaskSelected, int T) const {
   return processingTimesVector[idTaskSelected][idProcessorSelected] + T;
}

int SimpleSchedulerDecoder::calculateSelectedProcessorCost(int idProcessorSelected, int idTaskSelected) const {
   return processingTimesVector[idTaskSelected][idProcessorSelected] * getCostPerUnitOfTimeVector()[idProcessorSelected];
}

std::vector<std::tuple<int, int, int, int> > SimpleSchedulerDecoder::executeFirstSchedulingStrategy(int &T, int &C, const std::vector<double> &chromosome) const {
   // Initializing schedulingPlanVector.
   std::vector<std::tuple<int, int, int, int> > schedulingPlanVector(getTasksAmount());

   // Initializing TaskID_TaskRK_ProcessorRK_Vector.
   std::vector<std::tuple<int, double, double> > TaskID_TaskRK_ProcessorRK_Vector(getTasksAmount());

   for (int i = 0; i < getTasksAmount(); i++) {
      TaskID_TaskRK_ProcessorRK_Vector[i] = std::make_tuple(i, chromosome[i], chromosome[i + getTasksAmount()]);
      //printf("(BEFORE) TaskID_TaskRK_ProcessorRK_Vector[%d] = {%d, %f, %f}.\n", i, std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<1>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i]));
   }

   // Sorting by task random-key value (ascending order).
   std::sort(TaskID_TaskRK_ProcessorRK_Vector.begin(), TaskID_TaskRK_ProcessorRK_Vector.end(), compareFirstStrategy);

   //for (int i = 0; i < getTasksAmount(); i++) {
      //printf("(NOW) TaskID_TaskRK_ProcessorRK_Vector[%d] = {%d, %f, %f}.\n", i, std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[i]) + 1, std::get<1>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i]));
      //printf("(NOW) TaskID_TaskRK_ProcessorRK_Vector[%d] = {%d, %f, %f, %d}.\n", i, std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[i]) + 1, std::get<1>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i]), findProcessorIDByRandomKeyInterval(std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i])) + 1);
   //}

   // Creation of set E of tasks already selected for scheduling.
   int initialValueE = 0;
   std::vector<int> E;
   // Setting size of A.
   E.resize(getTasksAmount(), initialValueE);

   // Creation of set A of availables processors that can be chosen.
   std::vector< std::vector<int> > A;
   int initialValueA = 0;
   // Setting size of A.
   A.resize(getProcessorsAmount(), std::vector<int>(1, initialValueA));

   // Current task selected for scheduling.
   int indexTaskSelected = 0;

   while (!allTasksScheduled(E)) { // Enquanto houver tarefa a ser escalonada...
      int idTaskSelected = std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[indexTaskSelected]); // id da pŕoxima tarefa Ni a ser escalonada.
      int idProcessorSelected = findProcessorIDByRandomKeyInterval(std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[indexTaskSelected])); // id do processador Mj que deverá escalonar a tarefa Ni.
      while (processorBusy(A, idProcessorSelected)) { // Enquanto o processador definido para ser selecionado estiver indisponível...
         T = T + 1; // Incrementar em uma unidade o contador T (makespan).
         verifyReturningProcessors(A, T); // Verificar, no conjunto A, o retorno dos processadores que terminaram seus cooldowns (estão livres para serem escolhidos).
      }
      int processorCoolDown = calculateSelectedProcessorCoolDown(idProcessorSelected, idTaskSelected, T); // Cooldown que o processador Mj passará até tornar-se disponível novamente.
                                                                                                 // É definido por T + pij, onde pij é o tempo de processamento da tarefa Ni no processador Mj;
      selectProcessor(A, idProcessorSelected, processorCoolDown); // Selecionando o processador Mj (e tornando-o ocupado).
      selectTask(E, idTaskSelected); // Marcando no conjunto E que a tarefa Ni foi escalonada.
      int processorCost = calculateSelectedProcessorCost(idProcessorSelected, idTaskSelected); // Custo de uso do processador Mj para processar a tarefa Ni.
      C = C + processorCost; // Incrementando o custo total C.
      schedulingPlanVector[indexTaskSelected] = std::make_tuple(idTaskSelected, idProcessorSelected, getProcessingTimesVector()[idTaskSelected][idProcessorSelected], T); // Adicionando {Mj,Ni} ao plano de escalonamento.
      indexTaskSelected = indexTaskSelected + 1;
   }

   while (anyProcessorBusy(A)) { // Enquanto houver algum processador ocupado...
      T = T + 1; // Incrementar em uma unidade o contador T (makespan).
      verifyReturningProcessors(A, T); // Verificar, no conjunto A, o retorno dos processadores que terminaram seus cooldowns (encerraram suas últimas tarefas).
   }
   return schedulingPlanVector;
}

std::vector<std::tuple<int, int, int, int> > SimpleSchedulerDecoder::executeSecondSchedulingStrategy(int &T, int &C, const std::vector<double> &chromosome) const {
   // Initializing schedulingPlanVector.
   std::vector<std::tuple<int, int, int, int> > schedulingPlanVector(getTasksAmount());

   // Initializing TaskID_TaskRK_Vector.
   std::vector<std::tuple<int, double> > TaskID_TaskRK_Vector(getTasksAmount());

   for (int i = 0; i < getTasksAmount(); i++) {
      TaskID_TaskRK_Vector[i] = std::make_tuple(i, chromosome[i]);
      //printf("(BEFORE) TaskID_TaskRK_Vector[%d] = {%d, %f}.\n", i, std::get<0>(TaskID_TaskRK_Vector[i]), std::get<1>(TaskID_TaskRK_Vector[i]));
   }

   // Sorting by task random-key value (ascending order).
   std::sort(TaskID_TaskRK_Vector.begin(), TaskID_TaskRK_Vector.end(), compareSecondStrategy);

   //for (int i = 0; i < getTasksAmount(); i++) {
      //printf("(NOW) TaskID_TaskRK_Vector[%d] = {%d, %f}.\n", i, std::get<0>(TaskID_TaskRK_Vector[i]) + 1, std::get<1>(TaskID_TaskRK_Vector[i]));
   //}

   // Creation of set E of tasks already selected for scheduling.
   int initialValueE = 0;
   std::vector<int> E;
   // Setting size of A.
   E.resize(getTasksAmount(), initialValueE);

   // Creation of set A of availables processors that can be chosen.
   std::vector< std::vector<int> > A;
   int initialValueA = 0;
   // Setting size of A.
   A.resize(getProcessorsAmount(), std::vector<int>(1, initialValueA));

   // Current task selected for scheduling.
   int indexTaskSelected = 0;

   while (!allTasksScheduled(E)) { // Enquanto houver tarefa a ser escalonada...
      while (allProcessorsBusy(A)) { // Enquanto não existir processador disponível...
         T = T + 1; // Incrementar em uma unidade o contador T (makespan).
         verifyReturningProcessors(A, T); // Verificar, no conjunto A, o retorno dos processadores que terminaram seus cooldowns (estão livres para serem escolhidos).
      }
      int idProcessorSelected = searchRandomAvailableProcessor(A); // Procurar, aleatoriamente, um processador disponível Mj.
      int idTaskSelected = std::get<0>(TaskID_TaskRK_Vector[indexTaskSelected]); // id da pŕoxima tarefa Ni a ser escalonada.
      int processorCoolDown = calculateSelectedProcessorCoolDown(idProcessorSelected, idTaskSelected, T); // Cooldown que o processador Mj passará até tornar-se disponível novamente.
                                                                                                           // É definido por T + pij, onde pij é o tempo de processamento da tarefa Ni no processador Mj;
      selectProcessor(A, idProcessorSelected, processorCoolDown); // Selecionando o processador Mj (e tornando-o ocupado).
      selectTask(E, idTaskSelected); // Marcando no conjunto E que a tarefa Ni foi escalonada.
      int processorCost = calculateSelectedProcessorCost(idProcessorSelected, idTaskSelected); // Custo de uso do processador Mj para processar a tarefa Ni.
      C = C + processorCost; // Incrementando o custo total C.
      schedulingPlanVector[indexTaskSelected] = std::make_tuple(idTaskSelected, idProcessorSelected, getProcessingTimesVector()[idTaskSelected][idProcessorSelected], T); // Adicionando {Mj,Ni} ao plano de escalonamento.
      indexTaskSelected = indexTaskSelected + 1;
   }
   while (anyProcessorBusy(A)) { // Enquanto houver algum processador ocupado...
      T = T + 1; // Incrementar em uma unidade o contador T (makespan).
      verifyReturningProcessors(A, T); // Verificar, no conjunto A, o retorno dos processadores que terminaram seus cooldowns (encerraram suas últimas tarefas).
   }
   return schedulingPlanVector;
}

double SimpleSchedulerDecoder::decode(const std::vector<double> &chromosome) const {
   // Initializing T (makespan).
   int T = 0;

   // Initializing C (total monetary cost).
   int C = 0;

   // Initializing schedulingPlanVector.
   std::vector<std::tuple<int, int, int, int> > schedulingPlanVector(getTasksAmount());

   switch (schedulingStrategy) {

      case 1:
         // Execution of first scheduling strategy.
         schedulingPlanVector = executeFirstSchedulingStrategy(T, C, chromosome);
         break;

      case 2:
         // Execution of second scheduling strategy.
         schedulingPlanVector = executeSecondSchedulingStrategy(T, C, chromosome);
         break;

   }

   /*
   for (int i = 0; i < getTasksAmount(); i++) {
      printf("schedulingPlanVector[%d] {TASK, PROCESSOR, PROCESSING_TIME, START_TIME} = {%d, %d, %d, %d}.\n", i, std::get<0>(schedulingPlanVector[i]) + 1, std::get<1>(schedulingPlanVector[i]) + 1, std::get<2>(schedulingPlanVector[i]), std::get<3>(schedulingPlanVector[i]));
   }
   */

   // Initializing fitness.
   double fitness = 0.0;

   // Calculating fitness.
   fitness = - ((1 - T) + (1 - C));

   //printf("MAKESPAN = %d | CUSTO = %d | FITNESS = %f.\n", T, C, fitness);

   // Returning fitness value.
   return fitness;
}

void SimpleSchedulerDecoder::printTaskSchedulingPlan(const std::vector<double> &bestChromosome) const {
   if (schedulingStrategy == 1) {
      // Initializing T (makespan).
      int T = 0;

      // Initializing C (total monetary cost).
      int C = 0;

      // Initializing schedulingPlanVector.
      std::vector<std::tuple<int, int, int, int> > schedulingPlanVector(getTasksAmount());

      // Execution of first scheduling strategy.
      schedulingPlanVector = executeFirstSchedulingStrategy(T, C, bestChromosome);

      // Printing results...
      printf("Number of Tasks: %d.\n\n", getTasksAmount());
      printf("Number of Processors: %d.\n\n", getProcessorsAmount());

      printf("Task Scheduling Plan (Best Solution):\n\n");
      for (int i = 0; i < getTasksAmount(); i++) {
         printf("Execute Task %d on Processor %d when clock time = %d (processing duration: %d unit(s) of time).\n", std::get<0>(schedulingPlanVector[i]) + 1, std::get<1>(schedulingPlanVector[i]) + 1, std::get<3>(schedulingPlanVector[i]), std::get<2>(schedulingPlanVector[i]));
      }

      printf("\nMakespan: %d.\n\n", T);

      printf("Total monetary cost: %d.\n\n", C);
   }
}

int SimpleSchedulerDecoder::getTasksAmount() const {
   return tasksAmount;
}

int SimpleSchedulerDecoder::getProcessorsAmount() const {
   return processorsAmount;
}

std::vector<std::vector<int> > SimpleSchedulerDecoder::getProcessingTimesVector() const {
   return processingTimesVector;
}

void SimpleSchedulerDecoder::printProcessingTimesVector() const {
   for (unsigned int n = 0; n < getProcessingTimesVector().size(); n++) { // printf("LINHA %d.\n", n);
      for (unsigned int m = 0; m < getProcessingTimesVector()[n].size(); m++) { // printf("COLUNA %d.\n", m);
         // Tempos de processamento das tarefas n [0, ..., tasksCount] nas máquinas m [0, ..., processorCount].
         //printf("processingTimesVector[%d][%d] = %d.\n", n, m, getProcessingTimesVector()[n][m]);
         //printf("A tarefa n%d é processada na máquina m%d em %d unidade(s) de tempo.\n", (n + 1), (m + 1), getProcessingTimesVector()[n][m]);
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
      //printf("A máquina m%d custa %d unidade(s) monetária(s) por unidade de tempo de processamento.\n", (m + 1), getCostPerUnitOfTimeVector()[m]);
   }
}

std::vector<std::vector<double> > SimpleSchedulerDecoder::getProcessorsRandomKeyIntervalVector() const {
   return processorsRandomKeyIntervalVector;
}

std::vector<std::vector<double> > SimpleSchedulerDecoder::setProcessorsRandomKeyInterval() const {
   // Calculating random-key interval for each processor.
   const int intervalValues = 2;
   const double a = 0.0, b = 1.0;
   std::vector< std::vector<double> > processorsRandomKeyIntervalVector;
   processorsRandomKeyIntervalVector.resize(getProcessorsAmount(), std::vector<double>(intervalValues, 0));
   // M Loop...
   for (int indexM = 0; indexM < getProcessorsAmount(); indexM++) {
      processorsRandomKeyIntervalVector[indexM][0] = a + (indexM * (b / getProcessorsAmount()));
      processorsRandomKeyIntervalVector[indexM][1] = a + ((indexM + 1) * (b / getProcessorsAmount()));
      //printf("processorsRandomKeyIntervalVector[%d][0] = %f.\n", indexM, processorsRandomKeyIntervalVector[indexM][0]);
      //printf("processorsRandomKeyIntervalVector[%d][1] = %f.\n", indexM, processorsRandomKeyIntervalVector[indexM][1]);
   }
   return processorsRandomKeyIntervalVector;
}

int SimpleSchedulerDecoder::findProcessorIDByRandomKeyInterval(double randomKey) const {
   int processorID = -1;
   for (int indexM = 0; indexM < getProcessorsAmount(); indexM++) {
      if (randomKey >= getProcessorsRandomKeyIntervalVector()[indexM][0] && randomKey < getProcessorsRandomKeyIntervalVector()[indexM][1]) {
         processorID = indexM;
         break;
      }
   }
   return processorID;
}
