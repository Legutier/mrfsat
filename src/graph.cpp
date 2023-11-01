/*
    MRFSAT - Copyright (C) 2023  Lukas Esteban Gutierrez Lisboa

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "graph.hpp"
#include "mrf/min_closure.hpp"


namespace mrfsat {
    void Graph::addVariableToConstraint(int constraint_id, std::pair<int, int> variable_data) {
        adjacency_list[constraint_id][variable_data.first] = variable_data.second;
    }

    void Graph::addConstraintCoefficient(int constraint_id, int constraint_coefficient) {
        constraint_coefficients[constraint_id] = constraint_coefficient;
    }

    void Graph::buildFromConstraints() {
        std::unordered_map<int, NodeMap> new_adjacency_list;
        int normalizer = 0;
        for (auto& [constraint_node, lit_nodes]: adjacency_list) {
             auto it = normalization_marks.find(constraint_node);
            if (it != normalization_marks.end() && it->second == 1){
                for (const auto& pair : lit_nodes) {
                    normalizer += pair.second;
                }
                normalizer -= constraint_coefficients[constraint_node];
                to_normalize_amount--;
                n_constraints += 1;
            }
            for (auto& [lit_node, value]: lit_nodes) {
                int graph_node;
                auto it = normalization_marks.find(constraint_node);
                if (it != normalization_marks.end() && it->second == 1) {
                    int graph_node_normalized;
                    if (lit_node < 0) {
                        graph_node_normalized = -1 * lit_node;
                    } else {
                        graph_node_normalized = lit_node + n_lits/2;
                    }
                    new_adjacency_list[graph_node_normalized][constraint_node + n_lits + to_normalize_amount] = value / (normalizer);
                    new_adjacency_list[constraint_node + n_lits + to_normalize_amount][graph_node_normalized] = value / (normalizer);
                }
                if (lit_node < 0) {
                    graph_node = -1 * lit_node + n_lits/2;
                } else {
                    graph_node = lit_node;
                }

                new_adjacency_list[graph_node][constraint_node + n_lits] = value / constraint_coefficients[constraint_node];
                new_adjacency_list[constraint_node + n_lits][graph_node] = value / constraint_coefficients[constraint_node];
            }
        }
        adjacency_list = new_adjacency_list;
    }

    void Graph::updateLiteralsAmount(int new_number) {
        n_lits = std::max(n_lits, new_number);
    }

    void Graph::calculateMRFClusters() {
        numNodes = 0;
        numArcs = 0;
        source = 0;
        sink = 0;
        numParams = n_lits;
        highestStrongLabel = 1;
        int n_var = n_lits / 2;
        adjacencyList = NULL;
        strongRoots = NULL;
        labelCount = NULL;
        arcList = NULL;
        int nodes_amount = n_constraints + n_lits;
        graphInput(adjacency_list, nodes_amount, n_var);
        simpleInitialization();
        pseudoflowPhase1();
        for (int l = 0; l < numNodes; l++) {
            int cluster = (adjacencyList[l].breakpoint + adjacencyList[l * 2].breakpoint) / 2;
            if (l == n_var) std::cout << clusters.size() << ",";
            if(clusters.find(cluster) == clusters.end()) {
                clusters[cluster] = std::vector<int>();
            }
            clusters[cluster].push_back(l + 1);
        }
        for (int l = 0; l < numNodes; l++) {
        community_nodes.push_back(adjacencyList[l].breakpoint);
        }
    }

    std::pair<double, double> Graph::calculateVariance() {
        std::unordered_map<int, int> communityCountA;
        std::unordered_map<int, int> communityCountB;
        int n = n_lits / 2;
        // Count the frequency of nodes in each community for type A and type B
        for (int i = 0; i < 2 * n; ++i) {
            communityCountB[community_nodes[i]]++;
        }
        for (unsigned long i = 2 * n; i < community_nodes.size(); ++i) {
            communityCountA[community_nodes[i]]++;
        }

        // for each community in B get the proportion of elemnts that are from B and from Aa and calculate the ratio
        std::vector<double> ratios;
        for (auto& it : communityCountB) {
            double ratio = static_cast<double>(it.second) / (communityCountA[it.first] + it.second);
            ratios.push_back(ratio);
        }
        // return average ratio
        double sum = std::accumulate(ratios.begin(), ratios.end(), 0.0);
        double mean = sum / ratios.size();
        double stdev = 0;
        if (ratios.size() > 1) {
            double sq_sum = std::inner_product(ratios.begin(), ratios.end(), ratios.begin(), 0.0);
            stdev = std::sqrt(sq_sum / ratios.size() - mean * mean);
        }
        return std::make_pair(mean, stdev);
    }

    void Graph::calculateGraphData() {
        calculateMRFClusters();
        std::cout << clusters.size() << "," << n_constraints << "," << n_lits / 2 << ",";
        std::pair<double, double> variance_diff = calculateVariance();
        std::cout << variance_diff.first << "," << variance_diff.second;
        std::cout << std::endl;
    }

    void Graph::NormalizeEqualConstraint(int constraint_id) {
        to_normalize_amount++;
        normalization_marks[constraint_id] = 1;        
    }
}