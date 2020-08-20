import numpy as np
import pandas as pd

def events_to_df(events_path, max_ev=int(1e5), PT_cut=(140, 160), boost_and_scale=False, n_constits=30, label="0"):
    """Takes event list path (string)
     and returns two pandas DataFrames. One with constituent info and the other with jet info"""
    PT_min = PT_cut[0]
    PT_max = PT_cut[1]

    constits_df = pd.read_table(events_path, header=6, sep=" ", usecols=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11])
    constits_df = constits_df[(constits_df.entry.fillna(0) != 0) & (constits_df.entry != '--') &
                              (constits_df.entry != 'Done') & (constits_df.entry != 'entry')]
    constits_df = constits_df.rename({"entry": "Event"}, axis=1)
    constits_types = {"Event": np.int, "Jet": np.int, "T/T": np.int, "PT": np.float, "Eta": np.float,
                      "Phi": np.float, "DeltaR": np.float, "PID": "string", "D0/Ehad": np.float,
                      "DZ/Eem": np.float, "errD0": np.float, "errDZ": np.float}
    constits_df = constits_df.astype(constits_types)

    # Initialize
    jets = []  # Matrix for general jet info
    ev_num = 0          # Event number
    met = 0

    with open(events_path) as constit:
        # Loop over constituents
        for line in constit:
            data = line.split()

            if not data[0].isdigit():  # If the first element in line isn't a digit
                # New event
                if data[0] == "--":
                    ev_num += 1
                    if ev_num > max_ev:
                        break
                    continue
                # MET
                if data[0] == "MET:":
                    met = data[1]
                    continue
                # General jet info
                if data[0] == "Jet":
                    jets.append([ev_num, data[1], data[3], data[5], data[7], met])
                continue

    jets_df = pd.DataFrame(jets, columns=["Event", "Jet", "PT", "Eta", "Phi", 'MET'])
    jets_types = {"Event": np.int, "Jet": np.int, "PT": np.float, "Eta": np.float, "Phi": np.float, "MET": np.float}
    jets_df = jets_df.astype(jets_types)

    # Drop PID and sort by "PT"
    constits_df = constits_df.drop("PID", axis=1).sort_values(by=["Event", "Jet", "T/T", "PT"], ascending=[True, True, True, False])

    # Drop null tower measurements
    constits_df.drop(constits_df[(constits_df["DZ/Eem"] == 0) & (constits_df["D0/Ehad"] == 0)].index, inplace=True)

    # Add constituents info to jets array
    # Group by jet
    by_jet_track = constits_df[constits_df["T/T"] == 1].groupby(["Event", "Jet"])
    by_jet_tower_had = constits_df[(constits_df["T/T"] == 2) & (constits_df["DZ/Eem"] == 0)].groupby(["Event", "Jet"])
    by_jet_tower_em = constits_df[(constits_df["T/T"] == 2) & (constits_df["D0/Ehad"] == 0)].groupby(["Event", "Jet"])

    tr_feats = ["PT", "Eta", "Phi", "DeltaR", "D0/Ehad", "DZ/Eem", "errD0", "errDZ"]
    feats =  ["PT", "Eta", "Phi", "DeltaR", "D0/Ehad", "DZ/Eem"]
    track_feats = ["PT", "Eta", "Phi", "DeltaR", "D0", "DZ", "errD0", "errDZ"]
    tower_feats = ["PT", "Eta", "Phi", "DeltaR", "Ehad", "Eem"]
    track_rename = [feat + "_track" for feat in track_feats]
    tower_had_rename = [feat + "_tower_had" for feat in tower_feats]
    tower_em_rename = [feat + "_tower_em" for feat in tower_feats]

    def pad_trunc(x):
        x = list(x)
        return x[:n_constits] + [0] * (n_constits - len(x))

    track = by_jet_track[tr_feats].agg(pad_trunc).reset_index().rename(dict(zip(tr_feats, track_rename)), axis="columns")
    tower_had = by_jet_tower_had[feats].agg(pad_trunc).reset_index().rename(dict(zip(feats, tower_had_rename)), axis="columns")
    tower_em = by_jet_tower_em[feats].agg(pad_trunc).reset_index().rename(dict(zip(feats, tower_em_rename)), axis="columns")

    # Add multiplicities, max impact parameters, transverse momentum components and constits to jets array.
    jets_df = jets_df.merge(track).merge(tower_had).merge(tower_em)

    track_features = ["PT_track", "Eta_track", "Phi_track", "D0_track", "DZ_track", "errD0_track", "errDZ_track"]
    tower_features = ["PT_tower_had", "Eta_tower_had", "Phi_tower_had", "Ehad_tower_had", "Eem_tower_had"] + \
                     ["PT_tower_em", "Eta_tower_em", "Phi_tower_em", "Ehad_tower_em", "Eem_tower_em"]
    jets_df[track_features + tower_features] = jets_df[track_features + tower_features].applymap(np.array)

    jets_df = jets_df[(jets_df.PT > PT_min) & (jets_df.PT < PT_max)]

    # Add impact parameters D0/DZ of highest PT constituent of jet.
    jets_df["maxD0"] = jets_df["D0_track"].apply(lambda x: x[0])
    jets_df["maxDZ"] = jets_df["DZ_track"].apply(lambda x: x[0])
    jets_df["maxerrD0"] = jets_df["errD0_track"].apply(lambda x: x[0])
    jets_df["maxerrDZ"] = jets_df["errDZ_track"].apply(lambda x: x[0])
    jets_df["sigD0"] = jets_df["maxD0"]/jets_df["maxerrD0"]
    jets_df["sigDZ"] = jets_df["maxDZ"]/jets_df["maxerrDZ"]
    jets_df["avgDeltaR"] = jets_df.DeltaR_track.map(np.mean)
    # Add label to jets array.
    jets_df["label"] = label

    return constits_df, jets_df

