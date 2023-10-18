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
#include <iostream>


namespace mrfsat {
class Graph {
    public:
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
    private:
        std::unordered_map<int,  std::unordered_map<int, long double> > adjacency_list;
        std::unordered_map<int, int> constraint_coefficients;
};
}
