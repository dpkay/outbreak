#pragma once
#include "common.h"
#include <Eigen/Core>
#include <optional>

constexpr double kDaysToSymptoms = 14;
constexpr double kDaysSymptomsToRecovery = 10;
constexpr double kSubjectVelocityUnitsPerSecond = 3e-7;
constexpr double kSubjectAngleVolatilityPerSecond = 1e-3;

struct Infection {
  Time symptom_start_time;
  Time recovery_time;
};

class Subject {
public:
  explicit Subject(const Eigen::Vector2d &position) : position_(position) {
    angle_radians_ = GenerateNormalizedUniformRandomNumber() * 2.0 * M_PI;
    velocity_per_second_ = kSubjectVelocityUnitsPerSecond *
                           (1.2 - GenerateNormalizedUniformRandomNumber() * 0.4);
  }
  const Eigen::Vector2d &GetPosition() const { return position_; }
  InfectionState GetInfectionState() const { return infection_state_; }
  bool IsContagious() const {
    return infection_state_ == InfectionState::kInfectedWithoutSymptoms ||
           infection_state_ == InfectionState::kInfectedWithSymptoms;
  }

  void MaybeInfect(Time time) {
    if (IsImmune() || infection_)
      return;

    infection_ = Infection{
        .symptom_start_time =
            time + Hours(static_cast<int>(kDaysToSymptoms * 24)),
        .recovery_time =
            time + Hours(static_cast<int>(
                       (kDaysToSymptoms + kDaysSymptomsToRecovery) * 24)),
    };
  }

  std::string ToString() const {
    std::stringstream ss;
    ss << "S[pos=(" << position_[0] << ", " << position_[1]
       << "), infection_state=" << GetInfectionState() << "]";
    return ss.str();
  }

  void Update(Time time, Duration dt) {
    const double dt_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(dt).count();

    // Update infection state.
    infection_state_ = ComputeInfectionState(time);

    // Update angle.
    angle_radians_ += (GenerateNormalizedUniformRandomNumber() - 0.5) *
                      kSubjectAngleVolatilityPerSecond * dt_seconds;
    // Determine next direction.
    const Eigen::Vector2d velocity_direction(std::cos(angle_radians_),
                                             std::sin(angle_radians_));
    const double velocity_magnitude_per_second =
        GenerateExponentiallyDistributedRandomNumber(2.0) * kSubjectVelocityUnitsPerSecond * 5.0;
    //const double velocity_magnitude_per_second = velocity_per_second_;
    const Eigen::Vector2d velocity_vector_per_second =
        velocity_direction * velocity_magnitude_per_second;
    position_ += velocity_vector_per_second * dt_seconds;

    // Handle domain boundaries.
    if (position_[0] > 1.0)
      position_[0] -= 1.0;
    if (position_[1] > 1.0)
      position_[1] -= 1.0;
    if (position_[0] < 0.0)
      position_[0] += 1.0;
    if (position_[1] < 0.0)
      position_[1] += 1.0;
  }

private:
  bool IsImmune() const {
    return infection_state_ == InfectionState::kRecovered;
  }

  InfectionState ComputeInfectionState(Time time) const {
    if (!infection_)
      return InfectionState::kUninfected;
    if (time < infection_->symptom_start_time)
      return InfectionState::kInfectedWithoutSymptoms;
    if (time < infection_->recovery_time)
      return InfectionState::kInfectedWithSymptoms;
    return InfectionState::kRecovered;
  }

  InfectionState infection_state_;
  std::optional<Infection> infection_;
  Eigen::Vector2d position_;
  double velocity_per_second_;
  double angle_radians_;
};

