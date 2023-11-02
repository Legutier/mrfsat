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
#pragma once


#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>

namespace mrfsat {

using NodeMap = std::unordered_map<int, long double>;
class Graph {
    public:
        Graph() {
            n_lits = 0;
            to_normalize_amount = 0;
        }
        void addVariableToConstraint(int constraint_id, std::pair <int, int> variable_data);
        void addConstraintCoefficient(int constraint_id, int constraint_coefficient);
        void showGraph() {
            for(auto& [node, adj_node]: adjacency_list) {
                std::cout << "Constraint #" << node << std::endl;
                std::cout << constraint_coefficients[node] << std::endl;
                std::cout << "-------" << std::endl;
                for (auto& [var_node, coeff]: adj_node) {
                    std::cout << var_node << " " << coeff << std::endl;
                }
            }
        }
        void buildFromConstraints();
        void updateLiteralsAmount(int new_number);
        void setConstraintsNumber(int new_n_constraints) {n_constraints = new_n_constraints;}
        void calculateGraphData();
        void NormalizeEqualConstraint(int constraint_id);
        std::vector<int> community_nodes;
        std::unordered_map<int, std::vector<int> > clusters;
    private:
        void calculateMRFClusters();
        std::pair<double, double> calculateVariance();
        std::pair<double, double> calculateWeightedVariance();
        std::unordered_map<int, NodeMap> adjacency_list;
        std::unordered_map<int, int> constraint_coefficients;
        std::unordered_map<int, int> normalization_marks;
        int to_normalize_amount = 0;
        int n_lits;
        int n_constraints;
};
}
