import numpy as np
import pandas as pd

def events_to_pd(events_path, max_ev=int(1e5), PT_cut=(140, 160), boost_and_scale=False):
    """Takes event list path (string)
     and returns two pandas DataFrames. One with constituent info and the other with jet info"""

    # Initialize
    PT_min = PT_cut[0]
    PT_max = PT_cut[1]
    jet1_accept = True
    jet2_accept = True
    ev_num = 0          # Event number
    jets_mat = []        # Matrix for general jet info
    constits_mat = []     # Matrix for constituent info
    met = 0

    with open(events_path) as constit:
        # Loop over constituents
        for line in constit:
            data = line.split()

            if not data[0].isdigit(): # If the first element in line isn't a digit
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
                    if data[1] == "T/T":
                        continue
                    if int(data[1]) == 1:
                        jet1_accept = (float(data[3])>PT_min) and (float(data[3])<PT_max)
                        if jet1_accept: jets_mat.append([ev_num, data[1], data[3], data[5], data[7], met])
                        continue
                    if int(data[1]) == 2:
                        jet2_accept = (float(data[3])>PT_min) and (float(data[3])<PT_max)
                        if jet2_accept: jets_mat.append([ev_num, data[1], data[3], data[5], data[7], met])
                        continue
                continue

            data = [ev_num] + data      # Concatenate event number to constituent features
            constits_mat.append(data)   # Append constituemt features to constituents matrix

    constits_mat = pd.DataFrame(constits_mat,
                                columns=["Event", "Jet", "T/T", "PT", "Eta", "Phi", "DeltaR", "PID", "D0/Ehad", "DZ/Eem"])
    constits_types = {"Event": np.int, "Jet": np.int, "T/T": np.int, "PT": np.float, "Eta": np.float, "Phi": np.float,
                      "DeltaR": np.float, "PID": "string", "D0/Ehad": np.float, "DZ/Eem": np.float}
    constits_mat = constits_mat.astype(constits_types)

    jets_mat = pd.DataFrame(jets_mat, columns=["Event", "Jet", "PT", "Eta", "Phi", 'MET'])
    jets_types = {"Event": np.int, "Jet": np.int, "PT": np.float, "Eta": np.float, "Phi": np.float, "MET":np.float}
    jets_mat = jets_mat.astype(jets_types)

    return constits_mat, jets_mat

