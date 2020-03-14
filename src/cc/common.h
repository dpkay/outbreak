#pragma once
#include <chrono>
#include <random>

using Time = std::chrono::steady_clock::time_point;
using Duration = Time::duration;
using Hours = std::chrono::hours;

inline double GenerateNormalizedUniformRandomNumber() {
  static std::random_device r;
  static std::default_random_engine e1(r());
  static std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
  return uniform_dist(e1);
}

inline double GenerateExponentiallyDistributedRandomNumber(double lambda) {
  static std::default_random_engine generator;
  std::exponential_distribution<double> distribution(lambda);
  return distribution(generator);
}

enum class InfectionState {
  kUninfected,
  kInfectedWithoutSymptoms,
  kInfectedWithSymptoms,
  kRecovered,

  Count
};

constexpr int kNumInfectionStates = static_cast<int>(InfectionState::Count);

inline std::ostream &operator<<(std::ostream &os,
                                const InfectionState &infection_state) {
  switch (infection_state) {
  case InfectionState::kUninfected:
    os << "Uninfected";
    return os;
  case InfectionState::kInfectedWithoutSymptoms:
    os << "InfectedWithoutSymptoms";
    return os;
  case InfectionState::kInfectedWithSymptoms:
    os << "InfectedWithSymptoms";
    return os;
  case InfectionState::kRecovered:
    os << "Recovered";
    return os;
  default:
    assert(false);
  }
}
