import os
import sys


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
    return mapping


def main() -> None:
    """
    receive filename from sys arg and execute the program that will execute this file
    """
    if len(sys.argv) < 2:
        print("Error: no filename provided")
        return
    filename = sys.argv[1]
    predicting_data_mapping = get_predicting_data(filename)
    print(predicting_data_mapping)
    return


if __name__ == "__main__":
    main()
