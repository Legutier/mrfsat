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

    void Graph::updateAdjacencyList(std::unordered_map<int, NodeMap>& new_adjacency_list, int graph_node, int constraint_node, int value, int divisor, bool isEqualized) {
        if (divisor != 0) {
            new_adjacency_list[graph_node][constraint_node + n_lits + (isEqualized ? to_normalize_amount : 0)] = std::abs(value / divisor);
            new_adjacency_list[constraint_node + n_lits + (isEqualized ? to_normalize_amount : 0)][graph_node] = std::abs(value / divisor);
        } else {
            new_adjacency_list[graph_node][constraint_node + n_lits] = std::abs((value + 1) / (divisor + 1));
            new_adjacency_list[constraint_node + n_lits][graph_node] = std::abs((value + 1) / (divisor + 1));
        }
    }

    int Graph::getGraphNode(int lit_node) {
        if (lit_node < 0) {
            return -1 * lit_node + n_lits / 2;
        } else {
            return lit_node;
        }
    }

    void Graph::buildFromConstraints() {
        std::unordered_map<int, NodeMap> new_adjacency_list;
        int normalizer = 0;
        for (auto& [constraint_node, lit_nodes]: adjacency_list) {
            auto it = normalization_marks.find(constraint_node);
            bool isEqualized = it != normalization_marks.end() && it->second == 1;
            
            if (isEqualized) {
                normalizer = 0;
                for (const auto& pair : lit_nodes) {
                    normalizer += pair.second;
                }
                normalizer -= constraint_coefficients[constraint_node];
                to_normalize_amount--;
                n_constraints += 1;
            }

            for (auto& [lit_node, value]: lit_nodes) {
                int graph_node = getGraphNode(lit_node);

                if (isEqualized) {
                    updateAdjacencyList(new_adjacency_list, graph_node, constraint_node, value, normalizer, true);
                    updateAdjacencyList(new_adjacency_list, graph_node, constraint_node, value, constraint_coefficients[constraint_node], false);
                }
                else {
                    updateAdjacencyList(new_adjacency_list, graph_node, constraint_node, value, constraint_coefficients[constraint_node], false);
                }
            }
        }

        adjacency_list = std::move(new_adjacency_list);
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
            int cluster = adjacencyList[l].breakpoint;
            if (l == n_lits) std::cout << clusters.size() << ",";
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

    double sigmoid(double x, double c) {
        return 1 / (1 + std::exp(-c * x));
    }

    std::pair<double, double> Graph::calculateWeightedVariance() {
        std::unordered_map<int, double> communityStrengthA;
        std::unordered_map<int, double> communityStrengthB;
        int n = n_lits / 2;

        // Calculate the strength of nodes of type A and B in each community
        for (const auto& [node, neighbors] : adjacency_list) {
            int community = community_nodes[node];
            for (const auto& [neighbor, weight] : neighbors) {
                if (node < 2 * n && neighbor < 2 * n) {  // Both nodes are of type B
                    communityStrengthB[community] += weight;
                } else if (node >= 2 * n && neighbor >= 2 * n) {  // Both nodes are of type A
                    communityStrengthA[community] += weight;
                } else {  // One node is of type A, the other of type B
                    communityStrengthA[community] += weight / 2;
                    communityStrengthB[community] += weight / 2;
                }
            }
        }

        // Calculate weighted ratios
        std::vector<double> ratios;
        for (const auto& [community, strengthB] : communityStrengthB) {
            double totalStrength = communityStrengthA[community] + strengthB;
            if (totalStrength > 0) {
                double ratio = strengthB / totalStrength;
                ratios.push_back(ratio);
            }
        }

        // Calculate mean and standard deviation of ratios
        double mean = std::accumulate(ratios.begin(), ratios.end(), 0.0) / ratios.size();
        double variance = std::accumulate(ratios.begin(), ratios.end(), 0.0, [mean](double acc, double x) {
            return acc + (x - mean) * (x - mean);
        }) / ratios.size();
        double stdev = std::sqrt(variance);
        return {mean, stdev};
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