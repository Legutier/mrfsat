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
import pandas


def enrich_dataframe_with_sat_status_and_instance_type(dataframe: pandas.DataFrame, add_family: bool = True) -> pandas.DataFrame:
    """
    enriches dataframe with the feasibility status, also adds benchmark family and type of instance.
    """
    baseline_dataframe = pandas.read_csv("baseline.csv",on_bad_lines='warn')
    dec_opt_mapping = pandas.read_csv("dec_opt_mapping.csv", on_bad_lines = 'warn')
    family_dataframe = pandas.read_csv("family_mapping.csv")
    baseline_dataframe = pandas.merge(baseline_dataframe, dec_opt_mapping, on="name")
    baseline_dataframe['name'] = baseline_dataframe['name'].str.replace('.opb', '', regex=False)
    family_dataframe['name'] = family_dataframe['name'].str.replace('.opb', '', regex=False)
    dataframe = dataframe.merge(
        baseline_dataframe[["name", "is_sat", "dec_opt",]],
        on="name",
    ).merge(
        family_dataframe,
        on="name",
        how="inner",
    )
    return dataframe


def filter_out_instances_from_dataframe(dataframe: pandas.DataFrame, constraints_param: str = "formula_constraints") -> pandas.DataFrame:
    """
    removes unknown instances and balances dataframe, also removes invalid rows
    """
    dataframe = dataframe[dataframe["is_sat"] != -1]
    min_formula_constraints = dataframe[dataframe["dec_opt"] == "DEC"]["formula_constraints"].min()
    max_formula_constraints = dataframe[dataframe["dec_opt"] == "DEC"]["formula_constraints"].max()
    new_condition = (
        (dataframe["dec_opt"] == "DEC") |
        ((dataframe["dec_opt"] == "OPT") & 
        (dataframe[constraints_param] >= min_formula_constraints * 1.2) & 
        (dataframe[constraints_param] <= max_formula_constraints * 0.8))
    )
    dataframe = dataframe[new_condition]
    dataframe.dropna(inplace=True)
    return dataframe


def read_and_format_mrfsat_dataframe(csv_route: str) -> pandas.DataFrame:
    mrfsat_dataframe = pandas.read_csv(
        csv_route, header=None, on_bad_lines = 'skip',
    )
    mrfsat_dataframe['name'] = mrfsat_dataframe['name'].str.replace('.opb', '', regex=False)
    mrfsat_dataframe.columns = [
        "name",
        "variable_clusters",
        "total_clusters",
        "formula_constraints",
        "variables",
        "average_intersection",
        "std_dev_intersection",
    ]
    enrich_dataframe_with_sat_status_and_instance_type(dataframe=mrfsat_dataframe)
    filter_out_instances_from_dataframe(dataframe=mrfsat_dataframe)
    mrfsat_dataframe = mrfsat_dataframe[~(mrfsat_dataframe["name"].str.contains("aim"))]
    mrfsat_dataframe["is_sat"] = mrfsat_dataframe["is_sat"].apply(lambda x: x == 1)
    return mrfsat_dataframe


def read_and_format_satzilla_dataframe(csv_route: str) -> pandas.DataFrame:
    satzilla_dataframe = pandas.read_csv(
        csv_route, header=None, on_bad_lines = 'skip',
    )
    satzilla_dataframe['name'] = satzilla_dataframe['name'].str.replace('.cnf', '', regex=False)
    enrich_dataframe_with_sat_status_and_instance_type(dataframe=satzilla_dataframe)
    filter_out_instances_from_dataframe(dataframe=satzilla_dataframe, constraints_param="c")
    satzilla_dataframe = satzilla_dataframe[~(satzilla_dataframe["name"].str.contains("aim"))]
    satzilla_dataframe["is_sat"] = satzilla_dataframe["is_sat"].apply(lambda x: x == 1)
    satzilla_dataframe.drop_duplicates(subset=["name"], inplace=True)
    satzilla_dataframe.dropna(inplace=True)
    return satzilla_dataframe
