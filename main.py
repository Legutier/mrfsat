import joblib
import numpy
import os
import sys
import warnings

from sklearn.exceptions import InconsistentVersionWarning


FEATURES = ["average_freedom", "std_dev_freedom", "ratio"]


def load_classfier():
    warnings.filterwarnings("ignore", module="sklearn")
    warnings.filterwarnings("ignore", category=InconsistentVersionWarning, module="sklearn")
    classifier = joblib.load('models/random_forest_model.joblib')
    return classifier


def get_predicting_data(filename: str) -> dict[str, int | float | str]:
    keys = ("variable_clusters", "total_clusters", "total_clauses", "total_variables", "average_freedom", "std_dev_freedom")
    raw_data = os.popen("build/mrfsat " + filename).read().split(",")
    print("predicting over", raw_data.pop(0))
    mapping = {k: v for k, v in zip(keys, raw_data)}
    mapping["average_freedom"] = float(mapping["average_freedom"])
    mapping["std_dev_freedom"] = float(mapping["std_dev_freedom"])
    mapping["total_variables"] = int(mapping["total_variables"])
    mapping["total_clauses"] = int(mapping["total_clauses"])
    mapping["total_clusters"] = int(mapping["total_clusters"])
    mapping["variable_clustesr"] = int(mapping["variable_clusters"])
    mapping["ratio"] = mapping["variable_clustesr"] / mapping["total_clusters"]
    return mapping


def get_features_from_instance(filename: str) -> numpy.array:
    """
    receives filename from sys arg and execute the program that will execute this file
    """
    predicting_data_mapping = get_predicting_data(filename)
    feature_vector = numpy.array([predicting_data_mapping[feature] for feature in FEATURES]).reshape(1, -1)
    return feature_vector


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Error: no filename provided")
        exit(0)
    filename = sys.argv[1]
    classifier = load_classfier()
    feature_vector = get_features_from_instance(filename)
    prediction = classifier.predict(feature_vector)
    print("Prediction:", prediction[0])
