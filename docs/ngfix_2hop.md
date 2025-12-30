# NGFix 2-Hop Neighbor Implementation

## Overview

This implementation adds a new NGFix edge construction method based on 2-hop neighbors, as specified in the requirements:

For each point in the graph:
1. Calculate the average Euclidean distance `d` to its bottom (base graph) neighbors
2. Find all 2-hop neighbors from this point
3. For each 2-hop neighbor, if the distance to it is ≤ 1.1×d, add an NGFix unidirectional edge

## API Reference

### `void NGFixBy2Hop(id_t u)`

Applies the 2-hop neighbor NGFix edge construction to a single node.

**Parameters:**
- `u`: The node ID to process

**Algorithm:**
1. Retrieves all bottom (base graph) neighbors of node `u`
2. Calculates the average distance `d` from `u` to its bottom neighbors
3. Sets threshold as `1.1 × d`
4. Traverses the graph to find all 2-hop neighbors (neighbors of neighbors, excluding direct neighbors and self)
5. For each 2-hop neighbor `v`, if `distance(u, v) ≤ threshold`, adds an NGFix edge from `u` to `v`

**Time Complexity:** O(k²) where k is the average degree of nodes

### `void NGFixBy2HopAll(size_t Threads = 32)`

Batch processing version that applies NGFixBy2Hop to all nodes in parallel.

**Parameters:**
- `Threads`: Number of parallel threads to use (default: 32)

**Example:**
```cpp
auto hnsw_ngfix = new HNSW_NGFix<float>(metric, base_index_path);
hnsw_ngfix->NGFixBy2HopAll(32);  // Process all nodes
hnsw_ngfix->StoreIndex(result_index_path);
```

## Usage

### Building with 2-Hop NGFix

1. Build the base HNSW graph:
```bash
./build_hnsw_bottom \
  --data_path /path/to/data.fbin \
  --metric l2_float \
  --result_index_path /path/to/base_graph.index
```

2. Apply 2-hop NGFix edge construction:
```bash
./test_ngfix_2hop \
  --base_graph_path /path/to/base_graph.index \
  --metric l2_float \
  --result_index_path /path/to/ngfix_2hop_graph.index
```

3. Search using the enhanced graph:
```bash
./search_hnsw_ngfix \
  --index_path /path/to/ngfix_2hop_graph.index \
  --query_path /path/to/queries.fbin \
  --gt_path /path/to/groundtruth.ibin \
  --metric l2_float
```

## Implementation Details

### Key Design Decisions

1. **Bottom Neighbors Only**: The algorithm only considers edges in the base graph (bottom layer) when finding 2-hop neighbors, not existing NGFix edges. This prevents cascading effects and keeps the construction deterministic.

2. **Distance Threshold**: The 1.1× multiplier provides a small buffer beyond the average distance to bottom neighbors, allowing slightly longer shortcuts that can still improve search performance.

3. **Edge Hardness (EH)**: New edges are added with `EH_INF` (infinite hardness), indicating these are general structural improvements rather than query-specific fixes.

4. **Thread Safety**: The implementation uses per-node locks to ensure thread-safe parallel processing.

### Comparison with Original NGFix

| Aspect | Original NGFix | 2-Hop NGFix |
|--------|----------------|-------------|
| Input | Requires query data and ground truth | Only requires the graph structure |
| Complexity | O(Nq²) per query | O(k²) per node (k = avg degree) |
| Edge Selection | Based on hardness metric and graph defects | Distance-based threshold to 2-hop neighbors |
| Use Case | Query-specific optimization | Structure-based optimization |

### Performance Considerations

- **Memory**: Additional memory overhead is minimal (only temporary sets for tracking visited nodes)
- **Computation**: Linear in the number of nodes; parallelizes well across multiple cores
- **Index Size**: May increase the number of NGFix edges, but bounded by MEX parameter

## Example Output

```
HNSW Bottom Layer Information:
current number of elements: 1000000
Average out-degree: 32.5
Average Capacity: 36.0

Applying NGFixBy2Hop to all nodes...
Processing node 0
Processing node 100000
Processing node 200000
...
NGFixBy2Hop latency: 45230 ms.

HNSW_NGFix Information after NGFixBy2Hop:
current number of elements: 1000000
Average out-degree: 38.2
Average Capacity: 42.0
```

## Notes

- This method is complementary to the original NGFix method and can be used independently or in combination
- The 2-hop approach provides structural improvements without requiring query data
- Edge threshold (1.1×) can be adjusted based on dataset characteristics
- Works with both L2 and inner product metrics
