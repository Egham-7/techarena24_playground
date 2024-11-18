#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {

  static const uint32_t HASH_SIZE = 8192; // Power of 2 for fast modulo
  static const uint32_t RECENT_HISTORY_SIZE = 16;

  struct PatternEntry {
    uint32_t dayCount : 16;
    uint32_t totalCount : 16;
  };

  PatternEntry patternTable[HASH_SIZE];
  uint32_t recentPlanets[RECENT_HISTORY_SIZE];
  uint32_t recentDays[RECENT_HISTORY_SIZE];
  uint32_t historyIndex;
  uint32_t lastPlanet;
  bool lastDay;
};

static inline uint32_t hashPlanets(uint64_t planet1, uint64_t planet2) {
  uint32_t hash = (uint32_t)(planet1 ^ (planet1 >> 32));
  hash ^= (uint32_t)(planet2 ^ (planet2 >> 32));
  hash ^= (hash >> 16);
  return hash & (RoboPredictor::RoboMemory::HASH_SIZE - 1);
}

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  uint32_t hash = hashPlanets(roboMemory_ptr->lastPlanet, nextPlanetID);
  const RoboMemory::PatternEntry &entry = roboMemory_ptr->patternTable[hash];

  // If we have enough data, use pattern-based prediction
  if (entry.totalCount > 0) {
    uint32_t confidence = (entry.dayCount << 8) / entry.totalCount;
    bool patternPrediction = confidence > 128; // Threshold at 50%

    // Combine with spaceship prediction using confidence
    if (entry.totalCount > 4) {
      return patternPrediction;
    }
  }

  // Fall back to spaceship prediction if no strong pattern
  return spaceshipComputerPrediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
  uint32_t hash = hashPlanets(roboMemory_ptr->lastPlanet, nextPlanetID);
  RoboMemory::PatternEntry &entry = roboMemory_ptr->patternTable[hash];

  // Update pattern statistics using saturating arithmetic
  if (entry.totalCount < 0xFFFF) {
    entry.totalCount++;
    if (timeOfDayOnNextPlanet && entry.dayCount < 0xFFFF) {
      entry.dayCount++;
    }
  }

  // Update recent history
  roboMemory_ptr->recentPlanets[roboMemory_ptr->historyIndex] = nextPlanetID;
  roboMemory_ptr->recentDays[roboMemory_ptr->historyIndex] =
      timeOfDayOnNextPlanet;
  roboMemory_ptr->historyIndex = (roboMemory_ptr->historyIndex + 1) &
                                 (RoboMemory::RECENT_HISTORY_SIZE - 1);

  // Update last state
  roboMemory_ptr->lastPlanet = nextPlanetID;
  roboMemory_ptr->lastDay = timeOfDayOnNextPlanet;
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

