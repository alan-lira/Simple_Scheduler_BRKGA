#ifndef SIMPLESCHEDULERDECODER_H
#define SIMPLESCHEDULERDECODER_H

#include <list>
#include <vector>
#include <algorithm>

class SimpleSchedulerDecoder {

   public:

      // Constructor.
      SimpleSchedulerDecoder(int tasksAmount,
                             int processorsAmount,
                             std::vector<std::vector<int> > processingTimesVector,
                             std::vector<int> costPerUnitOfTimeVector);

      // Destructor.
      ~SimpleSchedulerDecoder();

      // Decode a chromosome, returning its fitness as a double-precision floating point.
      double decode(const std::vector<double> &chromosome) const;

      // Print candidate-chromosome's tasks scheduling plan.
      void printTaskSchedulingPlan(const std::vector<double> &candidate) const;

      // Get tasks amount.
      int getTasksAmount() const;

      // Get processors amount.
      int getProcessorsAmount() const;

      // Get processing times' vector.
      std::vector<std::vector<int> > getProcessingTimesVector() const;

      // Print processing times' vector.
      void printProcessingTimesVector() const;

      // Get cost per unit of time's vector.
      std::vector<int> getCostPerUnitOfTimeVector() const;

      // Print cost per unit of time's vector.
      void printCostPerUnitOfTimeVector() const;

   private:

      // Tasks amount.
      int tasksAmount;

      // Processors amount.
      int processorsAmount;

      // Processing times' vector.
      std::vector<std::vector<int> > processingTimesVector;

      // Cost per unit of time's vector.
      std::vector<int> costPerUnitOfTimeVector;

      // Processors random-key interval's vector.
      std::vector<std::vector<double> > processorsRandomKeyIntervalVector;

      // Get processors random-key interval's vector.
      std::vector<std::vector<double> > getProcessorsRandomKeyIntervalVector() const;

      // Set processors' random-key interval.
      std::vector<std::vector<double> > setProcessorsRandomKeyInterval() const;

      // Find processor id by random-key interval.
      int findProcessorIDByRandomKeyInterval(double randomKey) const;

      // Calculate selected processor cooldown.
      int calculateSelectedProcessorCoolDown(int idProcessorSelected, int idTaskSelected, int T) const;

      // Calculate selected processor cost.
      int calculateSelectedProcessorCost(int idProcessorSelected, int idTaskSelected) const;

};

#endif