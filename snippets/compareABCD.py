%%time
n = 100
name = "constits_D0_IP"
name1 = "cnn 15"
name2 = "lstm 15"
name3 = "cnn 30"
name4 = "lstm 30"

model_name_cnn = ["cnn_track_D0_15_IP1","cnn_track_D0_30_IP1"]
model_name_lstm = ["lstm_track_D0_15_IP1","lstm_track_D0_30_IP1"]

model = tf.keras.models.load_model(model_name_cnn[0])
bkg_preds1_cnn = model.predict(X_test_B1).flatten()
sig_preds1_cnn = model.predict(X_test_S1).flatten()
model = tf.keras.models.load_model(model_name_lstm[0])
bkg_preds1_lstm = model.predict(X_test_B1).flatten()
sig_preds1_lstm = model.predict(X_test_S1).flatten()

model = tf.keras.models.load_model(model_name_cnn[1])
bkg_preds2_cnn = model.predict(X_test_B2).flatten()
sig_preds2_cnn = model.predict(X_test_S2).flatten()
model = tf.keras.models.load_model(model_name_lstm[1])
bkg_preds2_lstm = model.predict(X_test_B2).flatten()
sig_preds2_lstm = model.predict(X_test_S2).flatten()

sig_eff = []
bkg_eff = []
frac = []
for thresh in (1-np.linspace(0.00001,0.8,n)):
    bkg_eff_temp = sum(bkg_preds1_cnn>thresh)/len(bkg_preds1_cnn)
    sig_eff_temp = sum(sig_preds1_cnn>thresh)/len(sig_preds1_cnn)
    sig_eff.append(sig_eff_temp)
    bkg_eff.append(1/bkg_eff_temp)
plt.semilogy(sig_eff,bkg_eff,"-k")
sig_eff = []
bkg_eff = []
frac = []
for thresh in (1-np.linspace(0.00001,0.8,n)):
    bkg_eff_temp = sum(bkg_preds1_lstm>thresh)/len(bkg_preds1_lstm)
    sig_eff_temp = sum(sig_preds1_lstm>thresh)/len(sig_preds1_lstm)
    sig_eff.append(sig_eff_temp)
    bkg_eff.append(1/bkg_eff_temp)
plt.semilogy(sig_eff,bkg_eff,"--k")

sig_eff = []
bkg_eff = []
frac = []
for thresh in (1-np.linspace(0.0001,0.8,n)):
    bkg_eff_temp = sum(bkg_preds2_cnn>thresh)/len(bkg_preds2_cnn)
    sig_eff_temp = sum(sig_preds2_cnn>thresh)/len(sig_preds2_cnn)
    sig_eff.append(sig_eff_temp)
    bkg_eff.append(1/bkg_eff_temp)
plt.semilogy(sig_eff,bkg_eff,"-r")
sig_eff = []
bkg_eff = []
frac = []
for thresh in (1-np.linspace(0.0001,0.8,n)):
    bkg_eff_temp = sum(bkg_preds2_lstm>thresh)/len(bkg_preds2_lstm)
    sig_eff_temp = sum(sig_preds2_lstm>thresh)/len(sig_preds2_lstm)
    sig_eff.append(sig_eff_temp)
    bkg_eff.append(1/bkg_eff_temp)
plt.semilogy(sig_eff,bkg_eff,"--r")

plt.legend([name1, name2, name3, name4])
plt.xlabel("Signal efficiency")
plt.ylabel("Background rejection")
plt.gcf().set_size_inches(8.3,5.85)
plt.savefig("ROC" + name + ".pdf",format="pdf")