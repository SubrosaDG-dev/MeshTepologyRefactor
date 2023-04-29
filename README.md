## Mesh Tepology Refactor

For numerical methods that require integration or interpolation on internal edges or faces (such as discontinuous Galerkin methods), it is necessary to obtain information about low-dimensional elements. This project can reconstruct information about low-dimensional elements from high-dimensional meshes. For example, in a 2D mesh, it can obtain boundary information of the elements (including nodes included in the boundary, upstream and downstream elements of the boundary, and boundary types).

The data format of the input mesh is `example.dat`
```
numNodes(size_t)
x(double) y(double) z(double)
    ...(numNodes rows)
numElements(size_t)
nodeTag(size_t) ... (numNodeTag columns)
    ...(numElements rows)
```
The data format of the output mesh is `example.grd`
```
numNodes(size_t)
x(double) y(double) z(double)
    ...(numNodes rows)
numEdges(size_t)
edgeStartNodeTag(size_t) edgeEndNodeTag(size_t) edgeLeftElement(size_t) edgeRightElement(size_t)
    ...(numEdges rows)
numElements(size_t)
nodeTag(size_t) ... (numNodeTag columns)
    ...(numElements rows)
volumnOfElement(double)
    ...(numElements rows)
```

The problem can be abstracted as follows. Taking a 2D mesh as an example, we can view the mesh as a graph, with mesh nodes as the vertices of the graph and mesh edges as the edges of the graph. We assign directions to the edges based on the orientation of the elements (that is, when traversing the connectivity between elements and nodes, we assign a positive direction to an edge that is encountered for the first time, assuming that all elements in the mesh are oriented consistently in either a clockwise or counterclockwise direction). This creates a directed graph, and we can obtain its adjacency matrix. By traversing the adjacency matrix, we can reconstruct the boundary information of the 2D elements.
