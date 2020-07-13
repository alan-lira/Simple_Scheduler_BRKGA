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
                             std::vector<int> costPerUnitOfTimeVector,
                             int decodingStrategy);

      // Destructor.
      ~SimpleSchedulerDecoder();

      // Decode a chromosome, returning its fitness as a double-precision floating point.
      double decode(const std::vector<double> &chromosome) const;

      // Print best chromosome's tasks scheduling plan.
      void printTaskSchedulingPlan() const;

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

      // Get decoding strategy to be used by decoder.
      int getDecodingStrategy() const;

   private:

      // Tasks amount.
      int tasksAmount;

      // Processors amount.
      int processorsAmount;

      // Processing times' vector.
      std::vector<std::vector<int> > processingTimesVector;

      // Cost per unit of time's vector.
      std::vector<int> costPerUnitOfTimeVector;

      // Decoding strategy to be used by decoder.
      int decodingStrategy;

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

      // Best Scheduling's plan.
      std::vector<std::tuple<double, int, int, std::vector<std::tuple<int, int, int, int> >> > bestSchedulingPlan;

      // Initializing Best Scheduling's plan.
      void initBestSchedulingPlan(std::vector<std::tuple<double, int, int, std::vector<std::tuple<int, int, int, int> >> > &bestSchedulingPlan);

      // Get Best Scheduling's plan.
      std::vector<std::tuple<double, int, int, std::vector<std::tuple<int, int, int, int> >> > getBestSchedulingPlan();

      // Set Best Scheduling's plan.
      void setBestSchedulingPlan(double &fitness, int &T, int &C, std::vector<std::tuple<int, int, int, int> > &schedulingPlan);

      // First decoding strategy.
      std::vector<std::tuple<int, int, int, int> > executeFirstDecodingStrategy(int &T, int &C, const std::vector<double> &chromosome) const;

      // Second decoding strategy.
      std::vector<std::tuple<int, int, int, int> > executeSecondDecodingStrategy(int &T, int &C, const std::vector<double> &chromosome) const;

      // Search for random available processor at set A.
      int searchRandomAvailableProcessor(std::vector<std::vector<int> > &A) const;

};

#endif