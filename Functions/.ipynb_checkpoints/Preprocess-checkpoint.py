import numpy as np
import pandas as pd

def events_to_df(events_paths, label, max_ev=int(1e5), n_constits=15, PT_cut=(140, 160), sort="PT"):
    """Takes event list path (string) and returns a pandas Dataframe with jet info"""

    # PT cut
    PT_min = PT_cut[0]
    PT_max = PT_cut[1]

    ev_num = 0  # Event number
    met = 0     # Missing transverse energy

    jets_list = []  # List of jet features
    jet1_info = []
    jet2_info = []
    jet1_accept = False
    jet2_accept = False

    jet1_track = []
    jet2_track = []

    if sort == "PT":
        sort = 0
    elif sort == "D0":
        sort = 11
    if type(events_paths) != list:
        events_paths = [events_paths]
    for events_path in events_paths:
        with open(events_path) as events:
            for line in events:
                row = line.split()

                # New event
                if row[0] == "--":
                    if ev_num > 0:
                        if jet1_accept and jet1_track:
                            jet1_track = np.array(jet1_track, dtype="float")
                            jet1_track = jet1_track[jet1_track[:, sort].argsort()[::-1]]
                            jets_list.append(jet1_info + list(jet1_track.T))
                        if jet2_accept and jet2_track:
                            jet2_track = np.array(jet2_track, dtype="float")
                            jet2_track = jet2_track[jet2_track[:, sort].argsort()[::-1]]
                            jets_list.append(jet2_info + list(jet2_track.T))
                        jet1_accept = False
                        jet2_accept = False
                        jet1_track = []
                        jet2_track = []
                    ev_num += 1
                    if ev_num > max_ev:
                        break
                    continue

                # MET
                if row[0] == "MET:":
                    met = row[1]
                    continue

                # General jet info
                if row[0] == "Jet":
                    if int(row[1]) == 1:
                        jet1_accept = (float(row[3]) > PT_min) and (float(row[3]) < PT_max)
                        if jet1_accept:
                            jet1_info = [ev_num, row[1], met,  row[3], row[5], row[7]]
                        continue
                    if int(row[1]) == 2:
                        jet2_accept = (float(row[3]) > PT_min) and (float(row[3]) < PT_max)
                        if jet2_accept:
                            jet2_info = [ev_num, row[1],  met, row[3], row[5], row[7]]
                        continue

                # Constituents info
                if row[0].isdigit():
                    if (int(row[1]) == 1) and jet1_accept:
                        if int(row[2]) == 1:
                            jet1_track.append(row[3:] + [abs(float(row[8]))])
                            continue
                        continue
                    if (int(row[1]) == 2) and jet2_accept:
                        if int(row[2]) == 1:
                            jet2_track.append(row[3:] + [abs(float(row[8]))])
                            continue
                        continue

    jets_df = pd.DataFrame(jets_list,
                           columns=["Event", "Jet", "MET", "jet_PT", "jet_Eta", "jet_Phi",
                                    "track_PT", "track_Eta", "track_Phi", "track_DeltaR", "track_PID",
                                    "track_D0", "track_DZ", "track_errD0", "track_errDZ", "track_Xd", "track_Yd", "absD0"])
    jets_df.drop("track_PID", axis=1, inplace=True)
    dtypes = {"Event": np.int, "Jet": np.int, "MET": np.float, "jet_PT": np.float, "jet_Eta": np.float,
              "jet_Phi": np.float}
    jets_df = jets_df.astype(dtypes)
    track_feats = ["track_PT", "track_Eta", "track_Phi", "track_DeltaR",
                   "track_D0", "track_DZ", "track_errD0", "track_errDZ", "track_Xd", "track_Yd"]

    jets_df[track_feats] = jets_df[track_feats].applymap(lambda x: np.append(x[:n_constits], [0] * (n_constits - len(x))))

    jets_df["label"] = label

    return jets_df

def scale_shift_feats(df):
    df.track_PT = df.track_PT / df.track_PT.map(np.max)
    df.track_Eta = (df.track_Eta - df.jet_Eta) * 10
    df.track_Phi = (df.track_Phi - df.jet_Phi) * 10
    df.track_DZ = df.track_DZ / np.cosh(df.jet_Eta)
    df.track_D0 = (df.jet_PT * np.cos(df.jet_Phi) * df.track_Xd + df.jet_PT * np.sin(df.jet_Phi) * df.track_Yd)\
                      .map(np.sign) * df.track_D0.map(np.abs)

def for_nn(train_val, bkg_test, sig_test, feats, n_constits, val_frac=0.2):

    # Split and reshape data
    train_ind = np.arange(0, int(len(train_val) * (1 - val_frac)))
    val_ind = np.arange(int(len(train_val) * (1 - val_frac)), len(train_val))

    X_train = np.concatenate(np.array(train_val.copy().iloc[train_ind][feats]).flatten()).reshape((len(train_ind), n_constits, len(feats)))
    y_train = train_val.iloc[train_ind]["label"]

    X_val = np.concatenate(np.array(train_val.copy().iloc[val_ind][feats]).flatten()).reshape((len(val_ind), n_constits, len(feats)))
    y_val = train_val.loc[val_ind]["label"]

    X_test_B = np.concatenate(np.array(bkg_test.copy()[feats]).flatten()).reshape((len(bkg_test), n_constits, len(feats)))
    X_test_S = np.concatenate(np.array(sig_test.copy()[feats]).flatten()).reshape((len(sig_test), n_constits, len(feats)))

    print("num total examples = {}".format(len(train_val)))
    print("num train examples = {}".format(len(train_ind)))
    print("num validation examples = {}".format(len(val_ind)))
    print("num Background examples = {}".format(len(train_val[train_val.label == 0])))
    print("num Signal examples = {}".format(len(train_val[train_val.label == 1])))
    print("X_train shape = {} \n".format(X_train.shape))

    return X_train, y_train, X_val, y_val, X_test_B, X_test_S
