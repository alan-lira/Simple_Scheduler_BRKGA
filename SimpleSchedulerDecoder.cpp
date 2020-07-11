#include <iostream>
#include <stdio.h>
#include<tuple>
#include<vector>

#include "SimpleSchedulerDecoder.h"

SimpleSchedulerDecoder::SimpleSchedulerDecoder(int tasksAmount,
                                               int processorsAmount,
                                               std::vector<std::vector<int> > processingTimesVector,
                                               std::vector<int> costPerUnitOfTimeVector)
                                               : tasksAmount(tasksAmount),
                                                 processorsAmount(processorsAmount),
                                                 processingTimesVector(processingTimesVector),
                                                 costPerUnitOfTimeVector(costPerUnitOfTimeVector),
                                                 processorsRandomKeyIntervalVector() {
   // 
   processorsRandomKeyIntervalVector = SimpleSchedulerDecoder::setProcessorsRandomKeyInterval();
}

SimpleSchedulerDecoder::~SimpleSchedulerDecoder() {

}

bool compare(const std::tuple<int, double, double> &i, const std::tuple<int, double, double> &j) {
   return std::get<1>(i) < std::get<1>(j);
}

double SimpleSchedulerDecoder::decode(const std::vector<double> &chromosome) const {

   // Initialize fitnessValue.
   double fitnessValue = 0.0;

   std::vector<std::tuple<int, double, double> > TaskID_TaskRK_ProcessorRK_Vector(getTasksAmount());

   for (int i = 0; i < getTasksAmount(); i++) {
      TaskID_TaskRK_ProcessorRK_Vector[i] = std::make_tuple(i, chromosome[i], chromosome[i + getTasksAmount()]);
      //printf("TaskID_TaskRK_ProcessorRK_Vector[%d] = {%d, %f, %f}.\n", i, std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<1>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i]));
   }

   // Sorting by task random-key value (ascending order).
   std::sort(TaskID_TaskRK_ProcessorRK_Vector.begin(), TaskID_TaskRK_ProcessorRK_Vector.end(), compare);

//   for (int i = 0; i < getTasksAmount(); i++) {
//      printf("TaskID_TaskRK_ProcessorRK_Vector[%d] = {%d, %f, %f}.\n", i, std::get<0>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<1>(TaskID_TaskRK_ProcessorRK_Vector[i]), std::get<2>(TaskID_TaskRK_ProcessorRK_Vector[i]));
//   }

   // Return fitnessValue.
   return fitnessValue;
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

std::vector<std::vector<int> > SimpleSchedulerDecoder::getProcessingTimesVector() const {
   return processingTimesVector;
}

void SimpleSchedulerDecoder::printProcessingTimesVector() const {
   for (unsigned int n = 0; n < getProcessingTimesVector().size(); n++) { // printf("LINHA %d.\n", n);
      for (unsigned int m = 0; m < getProcessingTimesVector()[n].size(); m++) { // printf("COLUNA %d.\n", m);
         // Tempos de processamento das tarefas n [0, ..., tasksCount] nas máquinas m [0, ..., processorCount].
         //printf("processingTimesVector[%d][%d] = %d.\n", n, m, getProcessingTimesVector()[n][m]);
         printf("A tarefa n%d é processada na máquina m%d em %d unidade(s) de tempo.\n", (n + 1), (m + 1), getProcessingTimesVector()[n][m]);
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
