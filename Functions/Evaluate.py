import numpy as np
from matplotlib import pyplot as plt

import tensorflow as tf

def test_model(model_name, X_test_B, X_test_S, analysis_dir="Analysis/"):
    #Load best weights
    model = tf.keras.models.load_model("Models/"+model_name)
    bkg_preds = model.predict(X_test_B).flatten()
    sig_preds = model.predict(X_test_S).flatten()
    sig_eff = []
    bkg_eff = []
    sig_eff_50 = 1.0
    bkg_eff_50 = 1.0
    for thresh in (1 - np.arange(0.00005, 0.8, 0.01)):
        bkg_eff_temp = np.sum(bkg_preds > thresh) / len(bkg_preds)
        sig_eff_temp = np.sum(sig_preds > thresh) / len(sig_preds)
        sig_eff.append(sig_eff_temp)
        if bkg_eff_temp == 0:
            bkg_eff_temp = 1e-5
        bkg_eff.append(1/bkg_eff_temp)
        if abs(sig_eff_temp - 0.5) < abs(sig_eff_50 - 0.5):
            sig_eff_50 = sig_eff_temp
            bkg_eff_50 = 1 / bkg_eff_temp
    plt.semilogy(sig_eff, bkg_eff)
    plt.annotate(model_name + ' Background rejection @0.5 Signal efficiency = {:.2e}'.format(bkg_eff_50),
                 xy=(0.05, 0.95), xycoords='axes fraction')
    print(sig_eff_50)
    plt.xlabel("Signal efficiency")
    plt.ylabel("Background rejection")
    plt.gcf().set_size_inches(8.3, 5.85)
    plt.savefig(analysis_dir + "ROC_" + model_name + ".pdf", format="pdf")
    plt.savefig(analysis_dir + "ROC_" + model_name + ".svg", format="svg")
    plt.show()

def compareAB(model1_name, model2_name, X_test_B, X_test_S, analysis_dir="Analysis/"):
    """" This function compares two models """
    #Load best weights
    model = tf.keras.models.load_model("Models/"+model1_name)
    bkg_preds1 = model.predict(X_test_B).flatten()
    sig_preds1 = model.predict(X_test_S).flatten()

    model = tf.keras.models.load_model("Models/"+model2_name)
    bkg_preds2 = model.predict(X_test_B).flatten()
    sig_preds2 = model.predict(X_test_S).flatten()

    sig_eff = []
    bkg_eff = []
    sig_eff_50 = 1.0
    bkg_eff_50 = 1.0
    for thresh in (1-np.arange(0.00005, 0.8, 0.01)):
        bkg_eff_temp = np.sum(bkg_preds1 > thresh)/len(bkg_preds1)
        sig_eff_temp = np.sum(sig_preds1 > thresh)/len(sig_preds1)
        sig_eff.append(sig_eff_temp)
        if bkg_eff_temp == 0:
            bkg_eff_temp = 1e-5
        bkg_eff.append(1/bkg_eff_temp)
        if abs(sig_eff_temp-0.5) < abs(sig_eff_50-0.5):
            sig_eff_50 = sig_eff_temp
            bkg_eff_50 = 1/bkg_eff_temp
    plt.semilogy(sig_eff, bkg_eff)
    plt.annotate(model1_name + ' Background rejection @0.5 Signal efficiency = {:.2e}'.format(bkg_eff_50), xy=(0.05, 0.95), xycoords='axes fraction')
    print(sig_eff_50)

    sig_eff = []
    bkg_eff = []
    sig_eff_50 = 1.0
    bkg_eff_50 = 1.0
    for thresh in (1-np.arange(0.00005, 0.8, 0.01)):
        bkg_eff_temp = np.sum(bkg_preds2 > thresh)/len(bkg_preds2)
        sig_eff_temp = np.sum(sig_preds2 > thresh)/len(sig_preds2)
        sig_eff.append(sig_eff_temp)
        if bkg_eff_temp == 0:
            bkg_eff_temp = 1e-5
        bkg_eff.append(1/bkg_eff_temp)
        if abs(sig_eff_temp-0.5) < abs(sig_eff_50-0.5):
            sig_eff_50 = sig_eff_temp
            bkg_eff_50 = 1/bkg_eff_temp
    plt.semilogy(sig_eff, bkg_eff)
    plt.annotate(model2_name + ' Background rejection @0.5 Signal efficiency = {:.3e}'.format(bkg_eff_50), xy=(0.05, 0.88), xycoords='axes fraction')
    print(sig_eff_50)

    plt.legend([model1_name, model2_name])
    plt.xlabel("Signal efficiency")
    plt.ylabel("Background rejection")
    plt.gcf().set_size_inches(8.3, 5.85)
    plt.savefig(analysis_dir+"ROC" + model1_name + "VS" + model2_name + ".pdf", format="pdf")
    plt.show()

def compareAB1(model1_name, model2_name, X_test_B1, X_test_S1, X_test_B2, X_test_S2, analysis_dir="Analysis/"):
    """" This function compares two models """
    #Load best weights
    model = tf.keras.models.load_model("Models/"+model1_name)
    bkg_preds1 = model.predict(X_test_B1).flatten()
    sig_preds1 = model.predict(X_test_S1).flatten()

    model = tf.keras.models.load_model("Models/"+model2_name)
    bkg_preds2 = model.predict(X_test_B2).flatten()
    sig_preds2 = model.predict(X_test_S2).flatten()

    sig_eff = []
    bkg_eff = []
    sig_eff_50 = 1.0
    bkg_eff_50 = 1.0
    for thresh in (1-np.arange(0.00005, 0.8, 0.01)):
        bkg_eff_temp = np.sum(bkg_preds1 > thresh)/len(bkg_preds1)
        sig_eff_temp = np.sum(sig_preds1 > thresh)/len(sig_preds1)
        sig_eff.append(sig_eff_temp)
        if bkg_eff_temp == 0:
            bkg_eff_temp = 1e-5
        bkg_eff.append(1/bkg_eff_temp)
        if abs(sig_eff_temp-0.5) < abs(sig_eff_50-0.5):
            sig_eff_50 = sig_eff_temp
            bkg_eff_50 = 1/bkg_eff_temp
    plt.semilogy(sig_eff, bkg_eff)
    plt.annotate(model1_name + ' Background rejection @0.5 Signal efficiency = {:.2e}'.format(bkg_eff_50), xy=(0.05, 0.95), xycoords='axes fraction')
    print(sig_eff_50)

    sig_eff = []
    bkg_eff = []
    sig_eff_50 = 1.0
    bkg_eff_50 = 1.0
    for thresh in (1-np.arange(0.00005, 0.8, 0.01)):
        bkg_eff_temp = np.sum(bkg_preds2 > thresh)/len(bkg_preds2)
        sig_eff_temp = np.sum(sig_preds2 > thresh)/len(sig_preds2)
        sig_eff.append(sig_eff_temp)
        if bkg_eff_temp == 0:
            bkg_eff_temp = 1e-5
        bkg_eff.append(1/bkg_eff_temp)
        if abs(sig_eff_temp-0.5) < abs(sig_eff_50-0.5):
            sig_eff_50 = sig_eff_temp
            bkg_eff_50 = 1/bkg_eff_temp
    plt.semilogy(sig_eff, bkg_eff)
    plt.annotate(model2_name + ' Background rejection @0.5 Signal efficiency = {:.3e}'.format(bkg_eff_50), xy=(0.05, 0.88), xycoords='axes fraction')
    print(sig_eff_50)

    plt.legend([model1_name, model2_name])
    plt.xlabel("Signal efficiency")
    plt.ylabel("Background rejection")
    plt.gcf().set_size_inches(8.3, 5.85)
    plt.savefig(analysis_dir+"ROC" + model1_name + "VS" + model2_name + ".pdf", format="pdf")
    plt.show()

