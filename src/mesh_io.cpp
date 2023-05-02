// clang-format off

#include "mesh_io.h"

#include <iostream>          // for cerr
#include <cstdlib>           // for exit
#include <Eigen/Core>        // for Matrix, Matrix3Xd, operator<<, DenseCoeffsBase, Index, Dynamic
#include <fstream>           // for operator<<, char_traits, endl, basic_ostream, ofstream, ifstream, ostream, basic...
#include <cstddef>           // for EXIT_FAILURE, size_t
#include <memory>            // for unique_ptr, make_unique

#include "mesh_structure.h"  // for Mesh2d

// clang-format on

namespace MeshTopologyRefactor {

void readMesh(const std::filesystem::path& mesh_input_file, Mesh2d& mesh) {
  int ignore;
  std::ifstream fin;
  fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    fin.open(mesh_input_file, std::ios_base::in);
    fin >> mesh.num_nodes_;
    mesh.nodes_ = std::make_unique<Eigen::Matrix3Xd>(3, mesh.num_nodes_);
    for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(mesh.num_nodes_); i++) {
      fin >> mesh.nodes_->operator()(0, i) >> mesh.nodes_->operator()(1, i) >> mesh.nodes_->operator()(2, i);
    }
    fin >> mesh.num_elements_;
    mesh.ielements_ = std::make_unique<Eigen::Matrix<std::size_t, 3, Eigen::Dynamic>>(3, mesh.num_elements_);
    for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(mesh.num_elements_); i++) {
      fin >> mesh.ielements_->operator()(0, i) >> mesh.ielements_->operator()(1, i) >>
          mesh.ielements_->operator()(2, i) >> ignore;
    }
    fin.close();
  } catch (const std::ifstream::failure& exception) {
    std::cerr << "Explanatory string: " << exception.what() << "." << std::endl;
    std::cerr << "File: " << mesh_input_file.string() << "." << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void writeMesh(const std::filesystem::path& mesh_output_file, Mesh2d& mesh) {
  std::ofstream fout;
  fout.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  fout.setf(std::ios_base::left, std::ios_base::adjustfield);
  fout.setf(std::ios_base::scientific, std::ios_base::floatfield);
  try {
    fout.open(mesh_output_file, std::ios_base::out | std::ios_base::trunc);
    fout << mesh.num_nodes_ << std::endl;
    fout << mesh.nodes_->transpose() << std::endl;
    fout << mesh.num_edges_ << std::endl;
    fout << mesh.iedges_->transpose() << std::endl;
    fout << mesh.num_elements_ << std::endl;
    fout << mesh.ielements_->transpose() << std::endl;
    fout.close();
  } catch (const std::ofstream::failure& exception) {
    std::cerr << "Explanatory string: " << exception.what() << "." << std::endl;
    std::cerr << "File: " << mesh_output_file.string() << "." << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

}  // namespace MeshTopologyRefactor
