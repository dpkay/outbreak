#pragma once

#include "subject.h"
#include "cell_grid.h"

constexpr double kDistanceToInfect = 0.005;
constexpr double kInfectionProbability = 0.02;

class Simulation {
public:
  const std::vector<Subject>& GetSubjects() { return subjects_; }

  void Init(int subject_count) {
    start_time_ = time_;
    subjects_.reserve(subject_count);

    const double recommended_cell_size =
        1.0 / std::sqrt(static_cast<double>(subject_count));
    const double cell_size = std::max(recommended_cell_size, kDistanceToInfect);
    cell_grid_ = std::make_unique<CellGrid<Subject*>>(cell_size);

    for (int i = 0; i < subject_count; ++i) {
      const Eigen::Vector2d p(GenerateNormalizedUniformRandomNumber(),
                              GenerateNormalizedUniformRandomNumber());
      subjects_.emplace_back(p);
      cell_grid_->Add(&subjects_.back(), p);
    }
    for (int i = 0; i < subject_count; ++i) {
      //std::cout << "-subject: " << &subjects_[i] << std::endl;
    }
    subjects_[0].MaybeInfect(time_);
  }

  void Update(Duration dt) {
    assert(cell_grid_);
    std::vector<Subject*> neighbors;

    time_ += dt;
    for (int i = 0; i < subjects_.size(); ++i) {
      Subject* subject1 = &subjects_[i];
      //std::cout << "subject: " << subject1 << std::endl;
      cell_grid_->Remove(subject1, subject1->GetPosition());
      subject1->Update(time_, dt);
      cell_grid_->Add(subject1, subject1->GetPosition());

      cell_grid_->GetNeighbors(subject1->GetPosition(), &neighbors);
      //std::cout << "neighbors: " << neighbors.size();
      for (int j = 0; j < neighbors.size(); ++j) {
        Subject* subject2 = neighbors[j];
        MaybePairwiseInfect(subject1, subject2);
      }
      //for (int j = i; j < subjects_.size(); ++j) {
      //  Subject* subject2 = &subjects_[j];
      //  MaybePairwiseInfect(subject1, subject2);
      //}
    }
  }

  void MaybePairwiseInfect(Subject *subject1, Subject *subject2) {
    const Eigen::Vector2d diff =
        (subject1->GetPosition() - subject2->GetPosition());
    if (diff.squaredNorm() < kDistanceToInfect * kDistanceToInfect) {
      if (GenerateNormalizedUniformRandomNumber() > kInfectionProbability)
        return;
      if (subject1->IsContagious())
        subject2->MaybeInfect(time_);
      if (subject2->IsContagious())
        subject1->MaybeInfect(time_);
    }
  }

  std::string ToString() const {
    std::stringstream ss;
    for (const auto &subject : subjects_) {
      ss << subject.ToString();
      ss << std::endl;
    }
    return ss.str();
  }


  std::vector<int> ComputeInfectionStateHistogram() {
    std::vector<int> infection_state_counts_(kNumInfectionStates);
    for (int i = 0; i < subjects_.size(); ++i) {
      const InfectionState infection_state = subjects_[i].GetInfectionState();
      ++infection_state_counts_[static_cast<int>(infection_state)];
    }
    return infection_state_counts_;
  }

  Duration GetElapsedSimulationTime() const { return time_ - start_time_; }

private:
  std::vector<Subject> subjects_;
  std::unique_ptr<CellGrid<Subject*>> cell_grid_;
  Time start_time_;
  Time time_;
};

