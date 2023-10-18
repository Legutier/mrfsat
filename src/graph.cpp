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

}