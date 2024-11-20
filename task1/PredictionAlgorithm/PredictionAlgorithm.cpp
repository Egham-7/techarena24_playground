#include "PredictionAlgorithm.hpp"
#include <array>

// Definition of RoboMemory within RoboPredictor
struct RoboPredictor::RoboMemory {
    static constexpr std::size_t MaxPlanets = 32768;  // Limit memory usage
    std::array<bool, MaxPlanets> predictions;        // Stores true (DAY) or false (NIGHT)
    std::array<bool, MaxPlanets> hasPrediction;      // Tracks if a prediction exists
    std::array<int, MaxPlanets> previousStates;      // Stores the previous state for each planet
    std::array<std::array<float, 2>, 2> transitionMatrix; // Markov transition matrix (DAY to NIGHT, NIGHT to DAY)

    RoboMemory() {
        predictions.fill(false);       // Default to NIGHT
        hasPrediction.fill(false);     // No predictions initially
        previousStates.fill(-1);       // No previous state
        transitionMatrix[0][0] = 0.0f; // DAY to DAY
        transitionMatrix[0][1] = 0.0f; // DAY to NIGHT
        transitionMatrix[1][0] = 0.0f; // NIGHT to DAY
        transitionMatrix[1][1] = 0.0f; // NIGHT to NIGHT
    }

    void updateTransitionMatrix(bool previousState, bool currentState) {
        float smoothingFactor = 0.1f;  // Smooth updates to avoid overfitting
        transitionMatrix[previousState][currentState] =
            transitionMatrix[previousState][currentState] * (1 - smoothingFactor) + smoothingFactor;
    }

    bool predictNextState(bool currentState) {
        int totalTransitions = transitionMatrix[currentState][0] + transitionMatrix[currentState][1];
        if (totalTransitions == 0) {
            // Bias towards night if no transition data exists
            return false;  // Default to NIGHT (80% prior bias)
        }

        // Calculate probabilities with bias adjustment
        float probDay = static_cast<float>(transitionMatrix[currentState][0]) / totalTransitions;
        float probNight = static_cast<float>(transitionMatrix[currentState][1]) / totalTransitions;

        // Incorporate the prior probabilities
        float biasedProbNight = probNight + 0.8f;
        float biasedProbDay = probDay + 0.2f;

        // Normalize the probabilities to sum to 1
        float total = biasedProbNight + biasedProbDay;
        biasedProbNight /= total;
        biasedProbDay /= total;

        // Predict DAY if its biased probability is higher
        return biasedProbDay >= biasedProbNight;
    }
};

// Constructor to initialize RoboPredictor and allocate memory for RoboMemory
RoboPredictor::RoboPredictor() {
    roboMemory_ptr = new RoboMemory;  // Allocate memory for Robo's memory
}

// Destructor to clean up RoboMemory when RoboPredictor is destroyed
RoboPredictor::~RoboPredictor() {
    delete roboMemory_ptr;  // Free the memory allocated for Robo's memory
}

bool RoboPredictor::predictTimeOfDayOnNextPlanet(std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
    std::size_t index = nextPlanetID % RoboMemory::MaxPlanets;

    // Check if a prediction exists for this planet
    if (!roboMemory_ptr->hasPrediction[index]) {
        // If no prediction exists, use spaceship computer's prediction
        roboMemory_ptr->predictions[index] = spaceshipComputerPrediction;
        roboMemory_ptr->hasPrediction[index] = true;
    } else {
        // Use the Markov chain prediction with prior bias
        bool previousState = roboMemory_ptr->predictions[index];  // Get previous prediction
        roboMemory_ptr->predictions[index] = roboMemory_ptr->predictNextState(previousState);  // Predict next state
    }

    return roboMemory_ptr->predictions[index];  // Return the predicted time of day (DAY or NIGHT)
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
    std::size_t index = nextPlanetID % RoboMemory::MaxPlanets;

    // Record the observed time of day and update Robo's memory
    if (roboMemory_ptr->hasPrediction[index]) {
        // Update the transition matrix based on the previous state and the current observed state
        bool previousState = roboMemory_ptr->predictions[index];
        roboMemory_ptr->updateTransitionMatrix(previousState, timeOfDayOnNextPlanet);
    }

    // Record the current state as the new prediction
    roboMemory_ptr->predictions[index] = timeOfDayOnNextPlanet;
    roboMemory_ptr->hasPrediction[index] = true;
}
