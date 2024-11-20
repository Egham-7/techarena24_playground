// PredictionAlgorithm.hpp

#ifndef PREDICTION_ALGORITHM_HPP
#define PREDICTION_ALGORITHM_HPP

#include <cstdint>

struct RoboPredictor {
  struct RoboMemory;
  RoboMemory* roboMemory_ptr;

  // Function declarations
  bool predictTimeOfDayOnNextPlanet(std::uint64_t nextPlanetID,
                                    bool spaceshipComputerPrediction);

  void observeAndRecordTimeofdayOnNextPlanet(std::uint64_t nextPlanetID,
                                             bool timeOfDayOnNextPlanet);

  //---------------------------------------------------------------------
  // Declarations of constructor and destructor moved to the .cpp file
  RoboPredictor();
  ~RoboPredictor();
};

#endif // PREDICTION_ALGORITHM_HPP
