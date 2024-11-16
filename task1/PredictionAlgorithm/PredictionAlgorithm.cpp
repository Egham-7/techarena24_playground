// This file contains a template for the implementation of Robo prediction
// algorithm

#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
  // Increased table size for better coverage while staying within memory limits
  static const uint32_t TABLE_SIZE = 4096;

  struct Entry {
    uint32_t planetId;
    uint8_t dayCount;
    uint8_t totalCount;
    uint8_t streak; // Track consecutive correct/incorrect predictions
    bool lastPrediction;
    bool valid;
  };

  Entry hashTable[TABLE_SIZE];
  uint16_t spaceshipCorrect;
  uint16_t spaceshipTotal;
  uint16_t globalDayCount;
  uint16_t globalTotal;
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  // Improved hash function using xor-shift for better distribution
  uint32_t hash = nextPlanetID;
  hash ^= hash >> 16;
  hash *= 0x85ebca6b;
  hash ^= hash >> 13;
  hash *= 0xc2b2ae35;
  hash &= (RoboMemory::TABLE_SIZE - 1);

  auto &entry = roboMemory_ptr->hashTable[hash];

  // Strong pattern detected for this planet
  if (entry.valid && entry.planetId == nextPlanetID) {
    if (entry.streak >= 3) {
      return entry.lastPrediction;
    }
    if (entry.totalCount >= 4) {
      // Use weighted probability
      uint16_t threshold = (entry.dayCount * 256) / entry.totalCount;
      return threshold > 128;
    }
  }

  // Global pattern detection
  if (roboMemory_ptr->globalTotal > 1000) {
    bool globalTrend =
        (roboMemory_ptr->globalDayCount * 2 > roboMemory_ptr->globalTotal);

    // If spaceship has good accuracy, combine both predictions
    if (roboMemory_ptr->spaceshipTotal > 100) {
      bool spaceshipReliable = (roboMemory_ptr->spaceshipCorrect * 2 >
                                roboMemory_ptr->spaceshipTotal);
      return spaceshipReliable ? spaceshipComputerPrediction : globalTrend;
    }
    return globalTrend;
  }

  return spaceshipComputerPrediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
  uint32_t hash = nextPlanetID;
  hash ^= hash >> 16;
  hash *= 0x85ebca6b;
  hash ^= hash >> 13;
  hash *= 0xc2b2ae35;
  hash &= (RoboMemory::TABLE_SIZE - 1);

  auto &entry = roboMemory_ptr->hashTable[hash];

  // Update global statistics
  if (roboMemory_ptr->globalTotal < 65000) {
    roboMemory_ptr->globalDayCount += timeOfDayOnNextPlanet ? 1 : 0;
    roboMemory_ptr->globalTotal++;
  }

  // Update planet-specific statistics
  if (!entry.valid || entry.planetId != nextPlanetID) {
    entry.planetId = nextPlanetID;
    entry.dayCount = timeOfDayOnNextPlanet ? 1 : 0;
    entry.totalCount = 1;
    entry.streak = 0;
    entry.lastPrediction = timeOfDayOnNextPlanet;
    entry.valid = true;
  } else {
    if (entry.totalCount < 255) {
      entry.dayCount += timeOfDayOnNextPlanet ? 1 : 0;
      entry.totalCount++;
    }

    // Update streak counter
    if (timeOfDayOnNextPlanet == entry.lastPrediction) {
      entry.streak = (entry.streak < 255) ? entry.streak + 1 : 255;
    } else {
      entry.streak = 0;
    }
    entry.lastPrediction = timeOfDayOnNextPlanet;
  }
}

//------------------------------------------------------------------------------
// Please don't modify this file below
//
// Check if RoboMemory does not exceed 64KiB
static_assert(
    sizeof(RoboPredictor::RoboMemory) <= 65536,
    "Robo's memory exceeds 65536 bytes (64KiB) in your implementation. "
    "Prediction algorithms using so much "
    "memory are ineligible. Please reduce the size of your RoboMemory struct.");

// Declare constructor/destructor for RoboPredictor
RoboPredictor::RoboPredictor() { roboMemory_ptr = new RoboMemory; }
RoboPredictor::~RoboPredictor() { delete roboMemory_ptr; }
