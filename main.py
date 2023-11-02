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
import joblib
import numpy
import os
import sys
import warnings
import argparse


from sklearn.exceptions import InconsistentVersionWarning


FEATURES = [
    "average_freedom",
    "std_dev_freedom",
    "ratio",
    "average_strength",
    "std_dev_strength",
]
MODEL_NAME = "random_forest_model.joblib"
DATA_KEYS = (
    "variable_clusters",
    "total_clusters",
    "total_clauses",
    "total_variables",
    "average_freedom",
    "std_dev_freedom",
    "average_strength",
    "std_dev_strength",
)


def load_classfier():
    warnings.filterwarnings("ignore", module="sklearn")
    warnings.filterwarnings("ignore", category=InconsistentVersionWarning, module="sklearn")
    classifier = joblib.load(f'models/{MODEL_NAME}')
    return classifier


def get_predicting_data(filename: str) -> dict[str, int | float | str]:
    raw_data = os.popen("build/mrfsat " + filename).read()
    raw_data = raw_data.split(",")
    raw_data.pop(0)
    mapping = {k: v for k, v in zip(DATA_KEYS, raw_data)}
    mapping["average_freedom"] = float(mapping["average_freedom"])
    mapping["std_dev_freedom"] = float(mapping["std_dev_freedom"])
    mapping["total_variables"] = int(mapping["total_variables"])
    mapping["total_clauses"] = int(mapping["total_clauses"])
    mapping["total_clusters"] = int(mapping["total_clusters"])
    mapping["variable_clusters"] = int(mapping["variable_clusters"])
    mapping["average_strength"] = float(mapping["average_strength"])
    mapping["std_dev_strength"] = float(mapping["std_dev_strength"])
    mapping["ratio"] = mapping["variable_clusters"] / mapping["total_clusters"]
    mapping["variables_per_clusters"] = mapping["total_variables"] / mapping["variable_clusters"]
    mapping["clauses_per_cluster"] = mapping["total_clauses"] / mapping["total_clusters"]
    return mapping


def get_features_from_instance(filename: str) -> numpy.array:
    """
    receives filename from sys arg and execute the program that will execute this file
    """
    predicting_data_mapping = get_predicting_data(filename)
    feature_vector = numpy.array([predicting_data_mapping[feature] for feature in FEATURES]).reshape(1, -1)
    return feature_vector



def make_prediction(filename: str) -> None:
    classifier = load_classfier()
    feature_vector = get_features_from_instance(filename)
    if numpy.isnan(feature_vector).any():
        print("Cant predict on", filename, "NaN values in feature vector")
        return
    prediction = classifier.predict(feature_vector)
    print(f"Prediction on {filename}:",  prediction[0])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="mrfsat",
        description="Predicts the satisfiability of a given pseudo-boolean SAT instance using a trained model",
    )
    parser.add_argument("-f", "--file", help = "specify File to be predicted")
    parser.add_argument("-d", "--dir", help = "Folder with files to predict")
    args = parser.parse_args()
    if args.file:
        make_prediction(args.file)
    elif args.dir:
        for file in os.listdir(args.dir):
            try:
                filename = os.path.join(args.dir, file)
                make_prediction(filename)
            except KeyError:
                continue
    else:
        print("Error: no filename provided")
        exit(0)
