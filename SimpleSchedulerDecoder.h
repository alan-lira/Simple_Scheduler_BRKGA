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
                             std::vector< std::vector<int> > processingTimeVector,
                             std::vector<int> costPerUnitOfProcessingTimeVector);

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

      // Get processing time vector.
      std::vector< std::vector<int> > getProcessingTimeVector() const;

      // Print processing time vector.
      void printProcessingTimeVector() const;

      // Get cost per unit of processing time vector.
      std::vector<int> getCostPerUnitOfProcessingTimeVector() const;

      // Print cost per unit of processing time vector.
      void printCostPerUnitOfProcessingTimeVector() const;

   private:

      // Tasks amount.
      int tasksAmount;

      // Processors amount.
      int processorsAmount;

      // Processing time vector.
      std::vector< std::vector<int> > processingTimeVector;

      // Cost per unit of processing time vector.
      std::vector<int> costPerUnitOfProcessingTimeVector;

};

#endif