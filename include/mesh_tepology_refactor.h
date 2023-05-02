#ifndef MESH_TEPOLOGY_REFACTOR_H_
#define MESH_TEPOLOGY_REFACTOR_H_

// clang-format off

#include <cstddef>     // for size_t
#include <filesystem>  // for path

// clang-format on

namespace MeshTopologyRefactor {

struct Mesh2d;

void initializeMesh(std::size_t num_holes, const std::filesystem::path& mesh_input_file,
                    MeshTopologyRefactor::Mesh2d& mesh);

}  // namespace MeshTopologyRefactor

void refactorMeshTopology(std::size_t num_holes, const std::filesystem::path& mesh_input_file,
                          const std::filesystem::path& mesh_output_file);

#endif  // MESH_TEPOLOGY_REFACTOR_H_
