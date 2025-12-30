#!/bin/bash

# Example script to build NGFix index using 2-hop neighbor method
# This script demonstrates how to use the NGFixBy2Hop functionality

MEX=48
M=16
efC=500

# Usage: Update the paths below to point to your data
# The base graph should be built first using build_hnsw_bottom

./test/test_ngfix_2hop \
--base_graph_path /path/to/base_graph.index \
--metric ip_float \
--result_index_path /path/to/result_ngfix_2hop.index

# Note: This is an example script. Update the paths according to your setup.
# The NGFixBy2Hop method will:
# 1. For each node, calculate average distance to its bottom (base graph) neighbors
# 2. Find all 2-hop neighbors by traversing base graph edges
# 3. Add NGFix edges to 2-hop neighbors if distance <= 1.1 * average_distance
