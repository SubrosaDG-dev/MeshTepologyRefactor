#ifndef MESH_TEPOLOGY_REFACTOR_MESH_STRUCTURE_H_
#define MESH_TEPOLOGY_REFACTOR_MESH_STRUCTURE_H_

// clang-format off

#include <cstddef>     // for size_t
#include <Eigen/Core>  // for Matrix, Matrix3Xd, Dynamic
#include <memory>      // for unique_ptr

// clang-format on

namespace MeshTopologyRefactor {

struct Mesh2d {
  std::size_t num_nodes_;
  std::size_t num_edges_;
  std::size_t num_elements_;

  std::unique_ptr<Eigen::Matrix3Xd> nodes_;
  std::unique_ptr<Eigen::Matrix<std::size_t, 4, Eigen::Dynamic>> iedges_;
  std::unique_ptr<Eigen::Matrix<std::size_t, 3, Eigen::Dynamic>> ielements_;
};

}  // namespace MeshTopologyRefactor

#endif  // MESH_TEPOLOGY_REFACTOR_MESH_STRUCTURE_H_
