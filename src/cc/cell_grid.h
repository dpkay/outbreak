#include "gtest/gtest_prod.h"
#include <Eigen/Core>
#include <iostream>
#include <set>
#include <vector>

template <typename T>
class CellGrid {
public:
  explicit CellGrid(double cell_size) {
    cell_size_ = cell_size;
    resolution_ = std::ceil(1.0 / cell_size_);
    cells_.resize(resolution_ * resolution_);
  }

  void Add(T t, const Eigen::Vector2d& position) {
    //std::cout << "adding" << t << " at " << position.transpose() << std::endl;
    const int cell_id = CellIdFromPosition(position);
    const auto result = cells_[cell_id].insert(t);
    assert(result.second);
  }

  void Remove(T t, const Eigen::Vector2d& position) {
    //std::cout << "removing " << t << " at " << position.transpose() << std::endl;
    const int cell_id = CellIdFromPosition(position);
    const int num_elements_removed = cells_[cell_id].erase(t);
    assert(num_elements_removed == 1);
  }

  void GetNeighbors(const Eigen::Vector2d& position, std::vector<T>* neighbors) {
    neighbors->clear();

    using Eigen::Vector2i;

    const Vector2i cell_coordinate = CellCoordinateFromPosition(position);

    // Left column.
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(-1, -1)), neighbors);
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(-1, 0)), neighbors);
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(-1, 1)), neighbors);

    // Center column.
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(0, -1)), neighbors);
    AddCellContentsToVector(cell_coordinate, neighbors);
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(0, 1)), neighbors);

    // Right column.
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(1, -1)), neighbors);
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(1, 0)), neighbors);
    AddCellContentsToVector(
        AdjacentCellCoordinate(cell_coordinate, Vector2i(1, 1)), neighbors);
  }

private:
  void AddCellContentsToVector(const Eigen::Vector2i& cell_coordinate, std::vector<T>* result) {
    const int cell_id = CellIdFromCellCoordinate(cell_coordinate);
    const auto& cell = cells_[cell_id];
    for (const auto& entry : cell) {
      result->push_back(entry);
    }
  }

  Eigen::Vector2i AdjacentCellCoordinate(const Eigen::Vector2i &coordinate,
                                         const Eigen::Vector2i &offset) {
    Eigen::Vector2i result = coordinate + offset;
    if (result[0] < 0)
      result[0] += resolution_;
    if (result[1] < 0)
      result[1] += resolution_;
    if (result[0] >= resolution_)
      result[0] -= resolution_;
    if (result[1] >= resolution_)
      result[1] -= resolution_;
    return result;
  }
  Eigen::Vector2i CellCoordinateFromPosition(const Eigen::Vector2d& position) {
    return Eigen::Vector2i(std::floor(position[0] / cell_size_),
                           std::floor(position[1] / cell_size_));
  }

  int CellIdFromCellCoordinate(const Eigen::Vector2i& coordinate) {
    return coordinate[1] * resolution_ + coordinate[0];
  }

  int CellIdFromPosition(const Eigen::Vector2d& position) {
    return CellIdFromCellCoordinate(CellCoordinateFromPosition(position));
  }

  FRIEND_TEST(CellGridTest, CellIdFromPosition);

  std::vector<std::set<T>> cells_;
  double cell_size_;
  int resolution_;
};
