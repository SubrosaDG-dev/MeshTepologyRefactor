#ifndef MESH_TEPOLOGY_REFACTOR_MESH_IO_H_
#define MESH_TEPOLOGY_REFACTOR_MESH_IO_H_

// clang-format off

#include <filesystem>  // for path

// clang-format on

namespace MeshTopologyRefactor {

struct Mesh2d;

void readMesh(const std::filesystem::path& mesh_input_file, Mesh2d& mesh);

void writeMesh(const std::filesystem::path& mesh_output_file, Mesh2d& mesh);

}  // namespace MeshTopologyRefactor

#endif  // MESH_TEPOLOGY_REFACTOR_MESH_IO_H_
