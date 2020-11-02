import os
import sys
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from Functions.Preprocess import tracks_to_df
from Functions.Models import train, lstm_mask, cnn
import tensorflow as tf
import sklearn.metrics
########################################################################################################################
### Flags and parameters
########################################################################################################################
#### Cluster flag
cluster = False
#### Parameters
N = 75000  # Total number of events in data
max_ev = 1e6 # Maximum number of events to load (before PT and 2jet cuts)
r = 0.2  # Signal proportion from total events
mult_thresh = 15  # Jet1 multiplicity threshold for initial cut
model_type = lstm_mask # Model for classification
model_name = "lstm" # Where to save weights
bkg_path = ["Data/for_semi_supervised_new/0.{}.5000.0.5.0.50.150.200.root.GetTracks.0.7.txt".format(i) for i in range(50)]
sig_path = ["Data/for_semi_supervised_new/1.{}.5000.0.5.0.50.150.500.root.GetTracks.0.7.txt".format(i) for i in range(50)]
n_constits = 40
mask = 0.0
sort = "D0"
feats = ["track_D0", "track_DZ"] + ["track_PT", "track_Eta", "track_Phi"]
jet_feats = ["jet_PT", "jet_Eta", "jet_Phi"]
PT_cut = (60, 180)
val_frac = 0.2
epochs = 20
analysis_dir = "Analysis/Semi-supervised/{}_{}_{}_{}_{}/".format(model_name, mult_thresh, r, n_constits, sort)
Path(analysis_dir).mkdir(parents=True, exist_ok=True)
#### Inputs
if cluster:
    os.chdir("/gpfs0/kats/users/wunch/dark_jets_repo/")
    analysis_dir = str(sys.argv[1])
    epochs = int(sys.argv[2])
    N = float(sys.argv[3])
    r = float(sys.argv[4])
    os.makedirs(analysis_dir, exist_ok=True)
    bkg_path = ["/gpfs0/kats/users/wunch/cluster_out/bb{}.root.GetTracks.txt".format(i) for i in range(1, 5)]
    sig_path = ["/gpfs0/kats/users/wunch/cluster_out/dark{}.root.GetTracks.txt".format(i) for i in range(1, 5)]

########################################################################################################################
### Preprocessing
########################################################################################################################
# Load signal and background
bkg_j = tracks_to_df(bkg_path, label=0, max_ev=max_ev, n_constits=n_constits, trunc=False, PT_cut=PT_cut, sort=sort)
sig_j = tracks_to_df(sig_path, label=1, max_ev=max_ev, n_constits=n_constits, trunc=False, PT_cut=PT_cut, sort=sort)
# Clean single jet events
bkg_evs_2jet = bkg_j.Event.value_counts()[bkg_j.Event.value_counts() > 1].index  # Events with two jets
sig_evs_2jet = sig_j.Event.value_counts()[sig_j.Event.value_counts() > 1].index
print("Number of Background events with two jets is ", len(bkg_evs_2jet), " out of ", len(bkg_j.Event.value_counts())) # Output number of events with two jets
print("Number of Signal events with two jets is ",len(sig_evs_2jet), " out of ", len(sig_j.Event.value_counts()))
print("----------")
bkg_j = bkg_j.loc[bkg_j.Event.map(lambda x: x in bkg_evs_2jet)].reset_index() # Clean single jet events
sig_j = sig_j.loc[sig_j.Event.map(lambda x: x in sig_evs_2jet)].reset_index()
# Group by jet
bkg_j1 = bkg_j[bkg_j.Jet == 1][['Event'] + feats + jet_feats + ['label']]  # Seperate
bkg_j2 = bkg_j[bkg_j.Jet == 2][['Event'] + feats + jet_feats]
bkg_j1 = bkg_j1.rename(columns={feat: feat+"1" for feat in (feats+jet_feats)})  # Rename
bkg_j2 = bkg_j2.rename(columns={feat: feat+"2" for feat in (feats+jet_feats)})
bkg_ev = pd.merge(bkg_j2, bkg_j1, on='Event')  # Merge
sig_j1 = sig_j[sig_j.Jet == 1][['Event'] + feats + jet_feats + ['label']] # Seperate
sig_j2 = sig_j[sig_j.Jet == 2][['Event'] + feats + jet_feats]
sig_j1 = sig_j1.rename(columns={feat : feat+"1" for feat in (feats+jet_feats)}) # Rename
sig_j2 = sig_j2.rename(columns={feat : feat+"2" for feat in (feats+jet_feats)})
sig_ev = pd.merge(sig_j2, sig_j1, on='Event') # Merge
# Create data sample with specified proportions
data = pd.concat([sig_ev.loc[:int(N*r)], bkg_ev.loc[:int(N*(1-r))]]).sample(frac=1).reset_index(drop=True)
print("N = ", len(data))
print("#B = ", len(data[(data.label == 0)]))
print("#S = ", len(data[(data.label == 1)]))
print("----------")
#### Seperate events into two groups S' (mult_thres=1) B'(mult_thresh=0) by cutting on jet1 multiplicity
data["j1_mult"] = data.track_PT1.map(lambda x: len(x))
data["j1_mult_cut"] = (data.j1_mult > mult_thresh).astype(np.int)

data['j2_mult'] = data.track_PT2.map(lambda x: len(x))
data["j2_mult_cut"] = (data.j1_mult > mult_thresh).astype(np.int)
# Output cut efficiencies
B_tag_SNR = len(data[(data.j1_mult_cut == 0) & (data.label == 1)]) / len(data[data.j1_mult_cut == 0])
S_tag_SNR = len(data[(data.j1_mult_cut == 1) & (data.label == 1)]) / len(data[data.j1_mult_cut == 1])
print("#B' = {} with S/(B+S) of {:.2f}%".format(len(data[(data.j1_mult_cut == 0)]), B_tag_SNR*100))
print("#S' = {} with S/(B+S) of {:.2f}%".format(len(data[(data.j1_mult_cut == 1)]), S_tag_SNR*100))
print("Background efficiencey of multiplicity cut is {:.2f}".format(len(data[(data.label == 0) & (data.j1_mult_cut == 1)])/len(data[data.label == 0])))
print("Signal efficiencey of multiplicity cut is {:.2f}".format(len(data[(data.label == 1) & (data.j1_mult_cut == 1)])/len(data[data.label == 1])))
print("----------")
# Preprocessing for NN
data.track_PT2 = data.track_PT2 / data.track_PT2.map(np.max)  # Scale and shift
data.track_Eta2 = (data.track_Eta2 - data.jet_Eta2) * 10
data.track_Phi2 = (data.track_Phi2 - data.jet_Phi2) * 10
data.track_DZ2 = data.track_DZ2 / np.cosh(data.jet_Eta2)

data.track_PT1 = data.track_PT1 / data.track_PT1.map(np.max)  # Scale and shift
data.track_Eta1 = (data.track_Eta1 - data.jet_Eta1) * 10
data.track_Phi1 = (data.track_Phi1 - data.jet_Phi1) * 10
data.track_DZ1 = data.track_DZ1 / np.cosh(data.jet_Eta1)
# Reshape for NN
min_counts_class = data.j1_mult_cut.value_counts().min()
balanced_idxs = data.groupby('j1_mult_cut', group_keys=False).apply(lambda x: x.sample(min_counts_class)).sample(frac=1).index
j2_feats_for_nn = np.array([np.vstack(data[feat+"2"].apply(lambda x: np.append(x[:n_constits], [mask] * (n_constits - len(x))))) for feat in feats]).swapaxes(0,1).swapaxes(1,2)
j1_feats_for_nn = np.array([np.vstack(data[feat+"1"].apply(lambda x: np.append(x[:n_constits], [mask] * (n_constits - len(x))))) for feat in feats]).swapaxes(0,1).swapaxes(1,2)
data['j2_mult'] = data.track_PT2.map(lambda x: len(x))
data['avg_jet_mult'] = (data.j1_mult + data.j2_mult)/2
print("N_for_nn = {}".format(min_counts_class))
print("NN input shape = {}".format(j2_feats_for_nn[balanced_idxs].shape))

########################################################################################################################
### Train classifier to distinguish between S' anb B' using jet2 features
########################################################################################################################
train_ind = np.arange(0, int(len(balanced_idxs) * (1 - val_frac)))
val_ind = np.arange(int(len(balanced_idxs) * (1 - val_frac)), len(balanced_idxs))
X_train = j2_feats_for_nn[balanced_idxs][train_ind]
X_val = j2_feats_for_nn[balanced_idxs][val_ind]
y_train = data.iloc[balanced_idxs].iloc[train_ind]["j1_mult_cut"]
y_val = data.iloc[balanced_idxs].iloc[val_ind]["j1_mult_cut"]
# Train
model = model_type(n_constits, feats)
model.save(analysis_dir)
train(model, X_train, y_train, X_val, y_val, model_name, epochs=epochs)

########################################################################################################################
### Evaluate classifier
########################################################################################################################
model = tf.keras.models.load_model(analysis_dir)
data["nn_out"] = model.predict(j2_feats_for_nn).flatten()
# We want one of these for each jet and for combined
# Second classifier ROC curve (takes predicition vectors and returns ROC curves)
bkg_eff = []
sig_eff = []
closest_half_sig_eff = 0
bkg_eff_closest_half_sig_eff = 0
for nn_thresh in np.arange(0, 1, 0.01):
    bkg_eff_temp = len(data[(data.label == 0) & (data.nn_out>nn_thresh)])/len(data[data.label == 0])
    sig_eff_temp = len(data[(data.label == 1) & (data.nn_out>nn_thresh)])/len(data[data.label == 1])
    bkg_eff.append(bkg_eff_temp)
    sig_eff.append(sig_eff_temp)
    if abs(sig_eff_temp - 0.5) < abs(closest_half_sig_eff - 0.5):
        closest_half_sig_eff = sig_eff_temp
        bkg_eff_closest_half_sig_eff = bkg_eff_temp
f1 = plt.figure()
plt.plot(bkg_eff, sig_eff, '--k')
plt.text(0.35, 0.35, 'NN bkg rejection @{:.2} sig efficiency = {:.2e}'.format(closest_half_sig_eff, 1/bkg_eff_closest_half_sig_eff), transform=plt.gca().transAxes)
plt.text(0.35, 0.30, 'NN AUC = {:.2f}'.format(sklearn.metrics.auc(bkg_eff, sig_eff)), transform=plt.gca().transAxes)
f2 = plt.figure()
plt.semilogy(bkg_eff, sig_eff, '--k')
plt.text(0.35, 0.35, 'NN bkg rejection @{:.2} sig efficiency = {:.2e}'.format(closest_half_sig_eff, 1/bkg_eff_closest_half_sig_eff), transform=plt.gca().transAxes)
plt.text(0.35, 0.30, 'NN AUC = {:.2f}'.format(sklearn.metrics.auc(bkg_eff, sig_eff)), transform=plt.gca().transAxes)
# Multiplicity cut ROC curve
bkg_eff = []
sig_eff = []
closest_half_sig_eff = 0
bkg_eff_closest_half_sig_eff = 0
for mult_threshold in np.arange(1, 50):
    bkg_eff_temp = len(data[(data.label == 0) & (data.avg_jet_mult > mult_threshold)])/len(data[data.label == 0])
    sig_eff_temp = len(data[(data.label == 1) & (data.avg_jet_mult > mult_threshold)])/len(data[data.label == 1])
    if bkg_eff_temp < 1e-6:
        bkg_eff_temp = 1e-6
    bkg_eff.append(bkg_eff_temp)
    sig_eff.append(sig_eff_temp)
    if abs(sig_eff_temp - 0.5) < abs(closest_half_sig_eff - 0.5):
        closest_half_sig_eff = sig_eff_temp
        bkg_eff_closest_half_sig_eff = bkg_eff_temp
plt.figure(f1.number)
plt.plot(bkg_eff, sig_eff, '-r')
plt.text(0.35, 0.5, 'Mult cut bkg rejection @{:.2} sig efficiency = {:.2e}'.format(closest_half_sig_eff, 1/bkg_eff_closest_half_sig_eff), transform=plt.gca().transAxes)
plt.text(0.35, 0.45, 'Mult cut AUC = {:.2f}'.format(sklearn.metrics.auc(bkg_eff, sig_eff)), transform=plt.gca().transAxes)
plt.xlim([0, 1])
plt.ylim([0, 1])
plt.ylabel("Signal efficiency")
plt.xlabel("Background efficiency")
plt.legend(["NN", "Multiplicity cut"], loc='upper left')
plt.gcf().set_size_inches(8.3, 5.85)
plt.savefig(analysis_dir + "both_ROC_" + model_name + ".pdf", format="pdf")
plt.figure(f2.number)
plt.semilogy(sig_eff, 1/np.array(bkg_eff), '-r')
plt.text(0.35, 0.5,'Mult cut bkg rejection @{:.2} sig efficiency = {:.2e}'.format(closest_half_sig_eff, 1/bkg_eff_closest_half_sig_eff), transform=plt.gca().transAxes)
plt.text(0.35, 0.45,'Mult cut AUC = {:.2f}'.format(sklearn.metrics.auc(bkg_eff, sig_eff)), transform=plt.gca().transAxes)
plt.ylabel("Background rejection")
plt.xlabel("Signal efficiency")
plt.legend(["NN", "Multiplicity cut"], loc='upper left')
plt.gcf().set_size_inches(8.3, 5.85)
plt.savefig(analysis_dir + "both_ROC_log" + model_name + ".pdf", format="pdf")

# NN output and multiplicity cut histograms
plt.figure()
data[(data.label == 1)].nn_out.hist(bins=np.arange(-0.005,1,0.01), color='black', histtype="step", hatch="x", density=True)
data[(data.label == 0)].nn_out.hist(bins=np.arange(-0.005,1,0.01), color='red', histtype="step", hatch="o", density=True)
plt.xticks(np.arange(0, 1, 0.1))
plt.yticks([])
plt.legend(["Signal - NN output", "Background - NN output"])
plt.xlabel('NN output')
plt.gcf().set_size_inches(8.3, 5.85)
plt.savefig(analysis_dir + "NN_hist_" + model_name + ".pdf", format="pdf")

plt.figure()
data[(data.label == 1)].avg_jet_mult.hist(bins=np.arange(-0.5,50,1), color='green', histtype="step", hatch="x", density=True)
data[(data.label == 0)].avg_jet_mult.hist(bins=np.arange(-0.5,50,1), color='blue', histtype="step", hatch="o", density=True)
plt.xticks(np.arange(0,50,2))
plt.yticks([])
plt.legend(["Signal - Multiplicity", "Background - Multiplicity"])
plt.gcf().set_size_inches(8.3, 5.85)
plt.xlabel('Average Multiplicity (mult_j1+multj2)/2')
plt.savefig(analysis_dir + "Multiplicity_hist_" + model_name + ".pdf", format="pdf")

print(data[['nn_out', 'j1_mult']].corr())
print(data[['nn_out', 'j2_mult']].corr())
print(data[['j1_mult', 'j2_mult']].corr())
