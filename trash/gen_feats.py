import numpy as np


import pandas as pd

def gen_feats(constits_info_inp, j_info_inp, n_constits=50, label=0, boost_and_shift=False, sort="PT"):
    """Takes two pandas DataFrames (created by events_to_pd) and returns two pandas DataFrames with features."""

    # Initialize
    j_info = j_info_inp.copy()

    # Drop PID and sort by "PT"
    constits_info = constits_info_inp.copy().drop("PID", axis=1)
    constits_info["absD0"] = np.abs(constits_info["D0/Ehad"])
    if sort == "PT":
        constits_info = constits_info.sort_values(by=["Event", "Jet", "T/T", "PT"], ascending=[True,True,True,False])
    elif sort == "absD0":
        constits_info = constits_info.sort_values(by=["Event", "Jet", "T/T", "absD0"], ascending=[True, True, True, False])

    # Drop null tower measurements
    constits_info.drop(constits_info[(constits_info["DZ/Eem"] == 0) & (constits_info["D0/Ehad"] == 0)].index, inplace=True)

    # Group by jet
    #by_jet = constits_info.groupby(["Event", "Jet"])
    by_jet_track = constits_info[constits_info["T/T"] == 1].groupby(["Event", "Jet"])
    by_jet_tower_had = constits_info[(constits_info["T/T"] == 2) & (constits_info["DZ/Eem"] == 0)].groupby(["Event", "Jet"])
    by_jet_tower_em = constits_info[(constits_info["T/T"] == 2) & (constits_info["D0/Ehad"] == 0)].groupby(["Event", "Jet"])

    # Compute tower/track multiplicities per jet.
    mult_track = by_jet_track["T/T"].agg("count") \
        .reset_index().rename({"T/T": "mult_track"}, axis="columns")
    mult_tower_had = by_jet_tower_had["T/T"].agg("count") \
        .reset_index().rename({"T/T": "mult_tower_had"}, axis="columns")
    mult_tower_em = by_jet_tower_em["T/T"].agg("count") \
        .reset_index().rename({"T/T": "mult_tower_em"}, axis="columns")

    avgDeltaR = by_jet_track["DeltaR"].agg("mean") \
        .reset_index().rename({"DeltaR": "avgDeltaR"}, axis="columns")

    # Add constituents info to jets array
    feats = ["PT", "Eta", "Phi", "DeltaR", "D0/Ehad", "DZ/Eem"]
    track_feats = ["PT", "Eta", "Phi", "DeltaR", "D0", "DZ"]
    tower_feats = ["PT", "Eta", "Phi", "DeltaR", "Ehad", "Eem"]
    track_rename = [feat + "_track" for feat in track_feats]
    tower_had_rename = [feat + "_tower_had" for feat in tower_feats]
    tower_em_rename = [feat + "_tower_em" for feat in tower_feats]
    def pad_trunc(x):
        x = list(x)
        return x  # x[:n_constits] + [0] * (n_constits - len(x))
    track = by_jet_track[feats].agg(pad_trunc)\
        .reset_index().rename(dict(zip(feats, track_rename)), axis="columns")
    #tower_had = by_jet_tower_had[feats].agg(pad_trunc)\
        #.reset_index().rename(dict(zip(feats, tower_had_rename)), axis="columns")
    #tower_em = by_jet_tower_em[feats].agg(pad_trunc)\
        #.reset_index().rename(dict(zip(feats, tower_em_rename)), axis="columns")

    # Add multiplicities, max impact parameters, transverse momentum components and constits to jets array.
    j_info = j_info.merge(mult_track).merge(mult_tower_had).merge(mult_tower_em).merge(avgDeltaR)\
        .merge(track) # .merge(tower_had).merge(tower_em)

    # Add impact parameters D0/DZ of highest PT constituent of jet.
    j_info["maxD0"] = j_info["D0_track"].apply(lambda x: x[0])
    j_info["maxDZ"] = j_info["DZ_track"].apply(lambda x: x[0])
    # j_info["errmaxD0"] = j_info["errD0_track"].apply(lambda x: x[0])
    # j_info["errmaxDZ"] = j_info["errDZ_track"].apply00(lambda x: x[0])

    # Add label to jets array.
    j_info["label"] = label

    track_features = ["PT_track", "Eta_track", "Phi_track", "D0_track", "DZ_track"]
    tower_features = [] #["PT_tower_had", "Eta_tower_had", "Phi_tower_had", "Ehad_tower_had", "Eem_tower_had"] +\
                    # ["PT_tower_em", "Eta_tower_em", "Phi_tower_em", "Ehad_tower_em", "Eem_tower_em"]
    j_info[track_features + tower_features] = j_info[track_features + tower_features].applymap(np.array)

    def boost(df):
        df.Phi_track = df.Phi_track - df.Phi
        df.Eta_track = df.Eta_track - df.Eta
        df.D0_track = df.D0_track / np.cosh(df.Eta)
        df.DZ_track = df.DZ_track

        #df.Phi_tower_em = df.Phi_tower_em - df.Phi
        #df.Eta_tower_em = df.Eta_tower_em - df.Eta
        #df.Phi_tower_had = df.Phi_tower_had - df.Phi
        #df.Eta_tower_had = df.Eta_tower_had - df.Eta

    def scale_PT(df):
        df.PT_track = df.PT_track/df.PT
        #df.PT_tower_em = df.PT_tower_em / df.PT
        #df.PT_tower_had = df.PT_tower_had / df.PT

    if boost_and_shift:
        boost(j_info)
        scale_PT(j_info)

    def tosize(x):
        padded = np.zeros(n_constits)
        m = min(n_constits, len(x))
        padded[:m] = x[:m]
        return padded

    j_info[track_features] = j_info[track_features].applymap(tosize)

    return constits_info, j_info

''' Trashcan:


    



# Add transverse momentum components to constituents array.
constits_info["Px"] = constits_info.PT * np.cos(constits_info["Phi"])
constits_info["Py"] = constits_info.PT * np.sin(constits_info["Phi"])
    
# Compute maximal track impact parameters per jet.
#max_IP = by_jet_track[["D0", "DZ"]].agg(lambda x: np.max(np.abs(x)))\s
        .reset_index().rename({"D0": "maxD0", "DZ": "maxDZ"}, axis="columns")    
    
# Add missing transverse energy to jets array.
j_info["MET"] = (j_info.sumPx ** 2 + j_info.sumPy ** 2) ** 0.5
j_info.drop(["sumPx", "sumPy"], axis=1, inplace=True)    

# Compute tower transverse momentum components.
sum_P = by_jet_tower[["Px", "Py"]].agg("sum")\
        .reset_index().rename({"Px": "sumPx", "Py": "sumPy"}, axis="columns")
'''
