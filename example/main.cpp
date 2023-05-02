// clang-format off

#include <filesystem>                // for operator/, path

#include "mesh_tepology_refactor.h"  // for refactorMeshTopology
#include "cmake.h"                   // for kProjectSourceDir

// clang-format on

int main() {
  std::filesystem::path mesh_input_file = kProjectSourceDir / "dat/test.dat";
  std::filesystem::path mesh_output_file = kProjectSourceDir / "dat/test.grd";
  refactorMeshTopology(0, mesh_input_file, mesh_output_file);
  return 0;
}
