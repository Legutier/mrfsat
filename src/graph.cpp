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
namespace mrfsat {
    void Graph::addVariableToConstraint(int constraint_id, std::pair<int, int> variable_data) {
        adjacency_list[constraint_id][variable_data.first] = variable_data.second;
    }

    void Graph::addConstraintCoefficient(int constraint_id, int constraint_coefficient) {
        constraint_coefficients[constraint_id] = constraint_coefficient;
    }

    void Graph::buildFromConstraints() {
        std::unordered_map<int, NodeMap> new_adjacency_list;
        for (auto& [constraint_node, lit_nodes]: adjacency_list) {
            for (auto& [lit_node, value]: lit_nodes) {
                new_adjacency_list[lit_node][constraint_node + n_lits] = value / constraint_coefficients[constraint_node];
                new_adjacency_list[constraint_node + n_lits][lit_node] = value / constraint_coefficients[constraint_node];
            }
        }
        adjacency_list = new_adjacency_list;
    }

    void Graph::updateLiteralsAmount(int new_number) {
        n_lits = std::max(n_lits, new_number);
    }
}