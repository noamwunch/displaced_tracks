import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Conv1D, Flatten, Dropout, MaxPool1D, LSTM, Masking
from tensorflow.keras.optimizers import Adam

def cnn(n_constits, feats):
    model = Sequential()
    model.add(Conv1D(128, 3, padding='same', activation='relu', input_shape=(n_constits,len(feats))))
    model.add(Conv1D(64, 3, padding='same', activation='relu'))
    model.add(Dropout(0.1))
    model.add(MaxPool1D())
    model.add(Conv1D(32, 3, padding='same', activation='relu'))
    model.add(Conv1D(32, 3, padding='same', activation='relu'))
    model.add(Dropout(0.1))
    model.add(MaxPool1D())
    model.add(Flatten())
    model.add(Dense(16, activation='relu'))
    model.add(Dense(16, activation='relu'))
    model.add(Dense(1, activation='sigmoid'))

    #Compile
    sgd = Adam(learning_rate=0.001)
    model.compile(optimizer=sgd, loss='binary_crossentropy')
    model.summary()
    print("")
    return model

def lstm(n_constits, feats):
    # Define model
    model = Sequential()
    model.add(LSTM(50, activation='relu', input_shape=(n_constits, len(feats)), return_sequences=False))
    model.add(Dense(16, activation='relu'))
    model.add(Dense(1, activation='sigmoid'))
    # Compile
    sgd = keras.optimizers.Adam(learning_rate=0.001)
    model.compile(optimizer=sgd, loss='binary_crossentropy')
    model.summary()
    print("")
    return model

def lstm_big(n_constits, feats):
    # Define model
    model = Sequential()
    model.add(LSTM(100, activation='relu', input_shape=(n_constits, len(feats)), return_sequences=True))
    model.add(LSTM(50, activation='relu', return_sequences=False))
    #model.add(Dense(32, activation='relu'))
    model.add(Dense(100, activation='relu'))
    model.add(Dense(1, activation='sigmoid'))

    # Compile
    sgd = keras.optimizers.Adam(learning_rate=0.001)
    model.compile(optimizer=sgd, loss='binary_crossentropy')
    model.summary()
    print("")
    return model

def lstm_mask(n_constits, feats):
    # Define model
    model = Sequential()
    model.add(Masking(mask_value=-10.0, input_shape=(n_constits, len(feats))))
    model.add(LSTM(50, return_sequences=False))
    model.add(Dense(16, activation='relu'))
    model.add(Dense(1, activation='sigmoid'))
    # Compile
    sgd = keras.optimizers.Adam(learning_rate=0.001)
    model.compile(optimizer=sgd, loss='binary_crossentropy')
    model.summary()
    print("")
    return model

def train(model, X_train, y_train, X_val, y_val, name, epochs=100):
    # Callbacks
    checkpoint = tf.keras.callbacks.ModelCheckpoint("Models/"+name, verbose=1, monitor='val_loss', save_best_only=True, mode='auto')
    es = tf.keras.callbacks.EarlyStopping(monitor="val_loss", min_delta=0.001, patience=5, verbose=2, mode="auto", baseline=None, restore_best_weights=False)
    # Train model
    model.fit(X_train, y_train, batch_size=1000, epochs=epochs, validation_data=(X_val, y_val), callbacks=[checkpoint, es])



