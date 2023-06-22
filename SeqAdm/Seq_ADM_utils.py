from tensorflow.keras.models import Sequential, load_model
from keras.layers import Dense, LSTM
from keras import optimizers
import numpy as np
import time
import math
from matplotlib import pyplot as plt
from IPython.display import display, clear_output

def sec2time(t):
    h = int(t//3600)
    t = t%3600
    m = int(t//60)
    s = int(t%60)
    return h, m, s


#creating and compiling model
def create_model(train_data, batch_size, neurons, opt, loss_func, out_dim):
    X = train_data[:, :-out_dim]
    X = X.reshape(X.shape[0], 1, X.shape[1])
    model = Sequential()
    model.add(LSTM(neurons, batch_input_shape=(batch_size, X.shape[1], X.shape[2]), stateful=True, return_sequences=False)) #!!!
    #model.add(LSTM(5, stateful=True))
    model.add(Dense(20, activation = 'tanh'))
    model.add(Dense(out_dim, activation = 'tanh'))
    model.compile(loss=loss_func, optimizer=opt)
    train_loss = [np.nan]
    val_loss = [np.nan]
    epochs = [0]
    train_time = 0
    return model, train_loss, val_loss, epochs, train_time


# fit an network to training data
def fit_rnn(model, train_data, val_data, nb_epoch, batch_size, train_loss, val_loss, epochs, train_time, out_dim):
    X, y = train_data[:, :-out_dim], train_data[:, -out_dim:]
    X = X.reshape(X.shape[0], 1, X.shape[1])
    X_val, y_val = val_data[:, :-out_dim], val_data[:, -out_dim:]
    X_val = X_val.reshape(X_val.shape[0], 1, X_val.shape[1])
    
    fig, ax = plt.subplots()
    last_epoch = epochs[-1]
    t1 = time.time()
    for i in range(nb_epoch):
        H = model.fit(X, y, epochs=1, batch_size=batch_size, verbose=0, shuffle=False, validation_data=(X_val, y_val))
        train_loss += H.history['loss']
        val_loss += H.history['val_loss']
        epochs += [last_epoch+i+1]
        model.reset_states()
        ax.cla()
        ax.plot(epochs, train_loss)
        ax.plot(epochs, val_loss)
        ax.set_ylim(bottom = 0)
        ax.set_xlabel('epochs')
        ax.set_ylabel('loss')
        ax.set_title('Model loss by epochs')
        ax.legend(('train_loss','val_loss'))
        ax.grid(True)
        display(fig)
        clear_output(wait = True)
    t2 = time.time() - t1
    train_time += t2

    h,m,s = sec2time(train_time)
    print('The training was completed in {:d}h {:d}m {:d}s ({:.2f} sec/epoch).'.format(h,m,s,train_time/epochs[-1]))
    
    return model, train_loss, val_loss, epochs, train_time

# make a one-step forecast
def forecast_rnn(model, batch_size, X):
    X = X.reshape(1, 1, len(X))
    yhat = model.predict(X, batch_size=batch_size)
    return yhat



# Voice Activity Detection algorithm
def VAD(signal, norma, frame_len, k_th = 1):
    
    def preprocess(signal, frame_len):    
        N_frames = math.floor(len(signal)/frame_len)
        signal = signal[:N_frames*frame_len]
        A = np.zeros((N_frames, frame_len))
        E = np.zeros(N_frames)
        
        for i in range(N_frames):
            A[i,:] = signal[i*frame_len : (i+1)*frame_len]
            s = sum(A[i,j]**2 for j in range(frame_len))
            E[i] = (1/frame_len) * s
        return signal, E, N_frames

    signal, E_s, N_fr_s = preprocess(signal, frame_len)
    norma, E_n, N_fr_n = preprocess(norma, frame_len)

    detection = np.zeros(len(signal))
    
    E_th = k_th * max(E_n)
    
    for i in range(N_fr_s):
        if E_s[i] > E_th:
            detection[i*frame_len : (i+1)*frame_len] = max(signal)*1.1
    
    return signal, detection

def dec2bin(x,n=7):
    # creating a binary list view of x
    u = 2**n - 1
    x = int(x)
    if abs(x) > u:
        print('Входное значение должно быть в пределах [-',u,' ; ',u,']')
    else:
        y = []
        if x == 0:
            for i in range(n): #+1
                y.append(0)
        else:

            while x != 1:
                y.append(x % 2)
                x = x // 2
            y.append(x)
            delta = n - len(y)
            for i in range(delta):
                y.append(0)

            y.reverse()
        return y