#include "cell_grid.h"
#include "gtest/gtest.h"

using Eigen::Vector2d;

TEST(CellGridTest, CellIdFromPosition) {
  CellGrid<int> cg(0.4);
  EXPECT_EQ(cg.CellIdFromPosition(Vector2d(0.3, 0.1)), 0);
  EXPECT_EQ(cg.CellIdFromPosition(Vector2d(0.9, 0.1)), 2);
  EXPECT_EQ(cg.CellIdFromPosition(Vector2d(0.5, 0.5)), 4);
  EXPECT_EQ(cg.CellIdFromPosition(Vector2d(0.6, 0.9)), 7);
  EXPECT_EQ(cg.CellIdFromPosition(Vector2d(0.9, 0.9)), 8);
}

TEST(CellGridTest, AddRemove) {
  CellGrid<int> cg(0.1);
  cg.Add(1, Vector2d(0.45, 0.47));
  cg.Add(2, Vector2d(0.47, 0.45));

  std::vector<int> neighbors;
  cg.GetNeighbors(Vector2d(0.46, 0.46), &neighbors);
  EXPECT_EQ(neighbors.size(), 2);

  cg.Remove(2, Vector2d(0.47, 0.45));
  cg.GetNeighbors(Vector2d(0.46, 0.46), &neighbors);
  EXPECT_EQ(neighbors.size(), 1);

  cg.Remove(1, Vector2d(0.45, 0.47));
  cg.GetNeighbors(Vector2d(0.46, 0.46), &neighbors);
  EXPECT_EQ(neighbors.size(), 0);
}

TEST(CellGridTest, GetNeighbors) {
  CellGrid<int> cg(0.1);
  // Add cells as follows:
  //
  //       0.3  0.4  0.5  0.6
  //        |    | 1  |    |
  // 0.3 ---+----+----+----+---
  //        |  2 |  3 |  4 |
  // 0.4 ---+----+----+----+---
  //      5 | 6,7| (8)| 9  | 10
  // 0.5 ---+----+----+----+---
  //        | 11 | 12 | 13 |
  // 0.6 ---+----+----+----+---
  //        |    | 14 |    |
  //
  cg.Add(1, Vector2d(0.45, 0.25));
  cg.Add(2, Vector2d(0.35, 0.35));
  cg.Add(3, Vector2d(0.45, 0.35));
  cg.Add(4, Vector2d(0.55, 0.35));
  cg.Add(5, Vector2d(0.25, 0.45));
  cg.Add(6, Vector2d(0.35, 0.45));
  cg.Add(7, Vector2d(0.35, 0.45));
  cg.Add(8, Vector2d(0.45, 0.45));
  cg.Add(9, Vector2d(0.55, 0.45));
  cg.Add(10, Vector2d(0.65, 0.45));
  cg.Add(11, Vector2d(0.35, 0.55));
  cg.Add(12, Vector2d(0.45, 0.55));
  cg.Add(13, Vector2d(0.55, 0.55));
  cg.Add(14, Vector2d(0.45, 0.65));

  std::vector<int> neighbors;
  cg.GetNeighbors(Vector2d(0.47, 0.47), &neighbors);
  EXPECT_EQ(std::find(neighbors.begin(), neighbors.end(), 1), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 2), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 3), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 4), neighbors.end());
  EXPECT_EQ(std::find(neighbors.begin(), neighbors.end(), 5), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 6), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 7), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 8), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 9), neighbors.end());
  EXPECT_EQ(std::find(neighbors.begin(), neighbors.end(), 10), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 11), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 12), neighbors.end());
  EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 13), neighbors.end());
  EXPECT_EQ(std::find(neighbors.begin(), neighbors.end(), 14), neighbors.end());
}
