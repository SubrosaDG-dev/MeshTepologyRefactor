// clang-format off

#include "mesh_tepology_refactor.h"

#ifdef _OPENMP
#include <omp.h>             // for omp_get_max_threads
#endif

#include <tqdm.hpp>          // for ProgressBar
#include <Eigen/Core>        // for Matrix, Index, Dynamic
#include <Eigen/SparseCore>  // for traits<>::Flags, SparseMatrix, traits<>::ColsAtCompileTime, traits<>::RowsAtComp...
#include <memory>            // for unique_ptr, make_unique

#include "mesh_io.h"         // for readMesh, writeMesh
#include "mesh_structure.h"  // for Mesh2d

// clang-format on

namespace MeshTopologyRefactor {

void initializeMesh(const std::size_t num_holes, const std::filesystem::path& mesh_input_file,
                    MeshTopologyRefactor::Mesh2d& mesh) {
  MeshTopologyRefactor::readMesh(mesh_input_file, mesh);
  mesh.num_edges_ =
      num_holes + 1 + mesh.num_elements_ + mesh.num_nodes_ - 2;  // R_in + R_out + R_mesh + V_mesh -  E_mesh = 2
  mesh.iedges_ = std::make_unique<Eigen::Matrix<std::size_t, 4, Eigen::Dynamic>>(4, mesh.num_edges_);
  mesh.iedges_->setZero();
}

}  // namespace MeshTopologyRefactor

void refactorMeshTopology(const std::size_t num_holes, const std::filesystem::path& mesh_input_file,
                          const std::filesystem::path& mesh_output_file) {
  MeshTopologyRefactor::Mesh2d mesh;
  MeshTopologyRefactor::readMesh(mesh_input_file, mesh);
  MeshTopologyRefactor::initializeMesh(num_holes, mesh_input_file, mesh);
  Eigen::SparseMatrix<std::size_t> adjacency(static_cast<Eigen::Index>(mesh.num_nodes_),
                                             static_cast<Eigen::Index>(mesh.num_nodes_));
  Tqdm::ProgressBar progress_bar{mesh.num_elements_};
  Eigen::Index count{0};
  std::size_t opposite_edge_tag;
  std::size_t node_1_tag;
  std::size_t node_2_tag;
#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(dynamic) shared(mesh, adjacency, count, progress_bar) private( \
    node_1_tag, node_2_tag, opposite_edge_tag) num_threads(omp_get_max_threads())
#endif
  for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(mesh.num_elements_); i++) {
    for (Eigen::Index j = 0; j < 3; j++) {
      node_1_tag = mesh.ielements_->operator()(j, i);
      node_2_tag = mesh.ielements_->operator()((j + 1) % 3, i);
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        opposite_edge_tag =
            adjacency.coeff(static_cast<Eigen::Index>(node_2_tag - 1), static_cast<Eigen::Index>(node_1_tag - 1));
        if (opposite_edge_tag == 0) [[likely]] {
          adjacency.coeffRef(static_cast<Eigen::Index>(node_1_tag - 1), static_cast<Eigen::Index>(node_2_tag - 1)) =
              static_cast<std::size_t>(++count - 1);
        }
      }
      if (opposite_edge_tag == 0) [[likely]] {
        mesh.iedges_->operator()(0, count - 1) = node_1_tag;
        mesh.iedges_->operator()(1, count - 1) = node_2_tag;
        mesh.iedges_->operator()(2, count - 1) = static_cast<std::size_t>(i + 1);
      } else [[unlikely]] {
        mesh.iedges_->operator()(3, static_cast<Eigen::Index>(opposite_edge_tag)) = static_cast<std::size_t>(i + 1);
      }
    }
#ifdef _OPENMP
#pragma omp critical
#endif
    { progress_bar.update(); }
  }
  MeshTopologyRefactor::writeMesh(mesh_output_file, mesh);
}
