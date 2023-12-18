"""
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
"""
import numpy
import pandas

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler


def split_target_features_from_mrfsat(mrfsat_dataframe: pandas.DataFrame) -> tuple[pandas.DataFrame, pandas.Series]:
    features = ["average_intersection", "std_dev_intersection", "ratio", "formula_ratio"]
    X = mrfsat_dataframe[features]
    y = mrfsat_dataframe["target"]
    return X, y


def split_target_features_from_satzilla_full(satzilla_dataframe: pandas.DataFrame) -> tuple[pandas.DataFrame, pandas.Series]:
    features = [feature for feature in satzilla_dataframe.columns if feature not in {"is_sat", "target", "name", "dec_opt", "family"}]
    X = satzilla_dataframe[features]
    y = satzilla_dataframe["target"]
    X = X.astype('float32')
    max_float = numpy.finfo(numpy.float32).max
    min_float = numpy.finfo(numpy.float32).min
    X = X.replace([numpy.inf, -numpy.inf], [max_float, min_float])
    scaler = StandardScaler()
    X = scaler.fit_transform(X)
    X = numpy.nan_to_num(X)
    return X, y
