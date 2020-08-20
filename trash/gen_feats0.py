import numpy as np
import pandas as pd

def gen_feats(constits_info, j_info, n_constits=50, label=0):
    """Takes two pandas DataFrames (created by events_to_pd) and returns two pandas DataFrames with features."""

    # Initialize
    j_info = j_info.copy()
    constits_info = constits_info.copy()

    # Add transverse momentum components to constituents array.
    constits_info["Px"] = constits_info.PT * np.cos(constits_info["Phi"])
    constits_info["Py"] = constits_info.PT * np.sin(constits_info["Phi"])

    # Compute tower/track multiplicities per jet.
    mult_track = constits_info[["Event", "Jet", "T/T"]][constits_info["T/T"] == 1].groupby(["Event", "Jet"])\
        .agg("count").reset_index([0, 1]).rename({"T/T": "mult_track"}, axis="columns")
    mult_tower = constits_info[["Event", "Jet", "T/T"]][constits_info["T/T"] == 2].groupby(["Event", "Jet"])\
        .agg("count").reset_index([0, 1]).rename({"T/T": "mult_tower"}, axis="columns")

    # Compute maximal track impact parameters per jet.
    max_IP = constits_info[["Event", "Jet", "D0", "DZ"]][constits_info["T/T"] == 1].groupby(["Event", "Jet"])\
        .agg(lambda x: np.max(np.abs(x))).reset_index([0, 1]).rename({"D0": "maxD0", "DZ": "maxDZ"}, axis="columns")

    # Compute jet transverse momentum components.
    sum_P = constits_info[["Event", "Jet", "Px", "Py"]][constits_info["T/T"] == 2].groupby(["Event", "Jet"])\
        .agg("sum").reset_index([0, 1]).rename({"Px": "sumPx", "Py": "sumPy"}, axis="columns")

    # Add multiplicities, max impact parameters and transvers momentum components to jets array.
    j_info = j_info.merge(mult_track).merge(mult_tower).merge(max_IP).merge(sum_P)

    # Add missing transverse energy to jets array.
    j_info["MET"] = (j_info.sumPx ** 2 + j_info.sumPy ** 2) ** 0.5

    # Add label to jets array.
    j_info["label"] = label

    # Add constituents info to jets array
    track_feats = ["PT", "Eta", "Phi", "DeltaR", "D0", "DZ"]
    tower_feats = ["PT", "Eta", "Phi", "DeltaR"]

    track_rename = [feat + "_track" for feat in track_feats]
    tower_rename = [feat + "_tower" for feat in tower_feats]

    def pad_trunc(x):
        x = list(x)
        return x[:n_constits] + [0] * (n_constits - len(x))

    track = constits_info[["Event", "Jet"] + track_feats][constits_info["T/T"] == 1].groupby(["Event", "Jet"]) \
        .agg(pad_trunc).reset_index().rename(dict(zip(track_feats, track_rename)), axis="columns")

    tower = constits_info[["Event", "Jet"] + tower_feats][constits_info["T/T"] == 2].groupby(["Event", "Jet"])\
        .agg(pad_trunc).reset_index().rename(dict(zip(tower_feats, tower_rename)), axis="columns")

    j_info = j_info.merge(track).merge(tower)

    return constits_info, j_info

