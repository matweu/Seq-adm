import numpy as np
import sys
np.set_printoptions(threshold=np.inf)
import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
from keras.models import Sequential, load_model
from keras.layers import Dense, LSTM
from keras import optimizers

import time
import json
from matplotlib import pyplot as plt
from matplotlib.lines import Line2D

from Seq_ADM_utils import create_model, fit_rnn, forecast_rnn, VAD, dec2bin

def args_set(x):
    for i, arg in enumerate(sys.argv[1:]):
        x[i] = arg

if len(sys.argv) < 2:
    print("DataSet Not Delivered")
    sys.exit(1)

# Вызываем функцию для вывода аргументов
x = np.zeros(4, dtype=object)
args_set(x)
model_directory = x[0]
test_fname = x[1]

N = 13
match x[3]:
    case 0:
        print('Выбран пункт с обучением')

        train_fname = x[2]
        try:
            train_seq = np.array([int(line.strip('\n')) for line in open(train_fname, 'r')])
            train_X = np.empty((len(train_seq), N))
            for i in range(len(train_seq)):
                train_X[i] = np.array(dec2bin(train_seq[i], N))

            train_Y = train_X.copy()
            train_data = np.column_stack((train_X, train_Y)).astype(np.float32)
            print('Обучающие данные загружены.')
        except FileNotFoundError:
            print('Файл с обучающими данными не найден!')

        num_neurons = 100
        opt = optimizers.SGD()
        loss = 'mean_squared_error'
        num_epochs = 500 #500

        model, train_loss, val_loss, epochs, train_time = create_model(train_data=train_data, batch_size=1,
                                                                       neurons=num_neurons, opt=opt, loss_func=loss,
                                                                       out_dim=N)
        model.summary()
        model, train_loss, val_loss, epochs, train_time = fit_rnn(model=model, train_data=train_data, val_data=train_data,
                                                                  nb_epoch=num_epochs, batch_size=1,
                                                                  train_loss=train_loss, val_loss=val_loss,
                                                                  epochs=epochs, train_time=train_time, out_dim=N)

        predictions_train = np.zeros(shape=(len(train_data), N))
        for i in range(len(train_data)):
            X = train_data[i, 0:-N]
            yhat = forecast_rnn(model=model, batch_size=1, X=X)
            predictions_train[i] = yhat

        error_train = (1 / N) * np.sum((train_Y - predictions_train) ** 2, axis=1)

        model.save(model_directory + '\\S009')

        with open(model_directory + '\\S009\\train.predict', 'w') as f:
            json.dump(predictions_train.tolist(), f)
        with open(model_directory + '\\S009\\train.error', 'w') as f:
            json.dump(error_train.tolist(), f)

        model_input = 'S009'
        
        model = load_model(model_directory + '\\S009')
        with open(model_directory + '\\S009\\train.predict', 'r') as f:
            predictions_train = json.load(f)
        with open(model_directory + '\\S009\\train.error', 'r') as f:
            error_train = json.load(f)
        print('Модель и отклики загружены')
        print("Model is complete Training")
        time.sleep(10)

    case 'train.error':
        print('Не производится обучение')

        train_predict = x[2]
        train_error = x[3]
        try:
            model = load_model(model_directory)
            with open(model_directory + '\\train.predict', 'r') as f:
                predictions_train = json.load(f)
            with open(model_directory + '\\train.error', 'r') as f:
                error_train = json.load(f)
            print('Модель и отклики загружены')
            print("Model is complete Loading")
            time.sleep(10)
        except FileNotFoundError:
            print('Не удалось загрузить модель, файлы повреждены!')

anomal_threshold = np.max(np.abs(error_train[100:]), axis=0)
print("Выполняется поиск аномалий:")


time.sleep(5)

column1 = []
column2 = []
column3 = []
column4 = []

while True:
    file = open(test_fname, 'r')
    lines = file.readlines()
    file.close()

    if len(column1) < N:
        if len(lines) < N:
            #print("Недостаточно элементов в файле. Повторная попытка через 5 секунд...")
            column1.clear()
            column2.clear()
            column3.clear()
            column4.clear()
            time.sleep(5)
            continue

        for line in lines:
            data = line.strip().split('\t')
            if len(data) >= 2:
                column1.append(data[0])
                column2.append(data[1])
                column3.append(data[2])
                column4.append(data[3])
            else:
                print(f"Некорректный формат строки: {line}")

        print("Первый столбец:")
        for value in column1:
            print(value)

        print("Второй столбец:")
        for value in column2:
            print(value)
        time.sleep(5)
    else:
        try:
            test_seq = np.array([int(value) for value in column1])
            test_X = np.empty((len(test_seq), N))
            for i in range(len(test_seq)):
                test_X[i] = np.array(dec2bin(test_seq[i], N))

            test_Y = test_X.copy()
            test_data = np.column_stack((test_X, test_Y)).astype(np.float32)
            print('Данные загружены.')

            predictions_test = np.zeros(shape=(len(test_data), N))
            for i in range(len(test_data)):
                X = test_data[i, 0:-N]
                yhat = forecast_rnn(model=model, batch_size=1, X=X)
                predictions_test[i] = yhat

            error_test = (1 / N) * np.sum((test_Y - predictions_test) ** 2, axis=1)
            vad_detection = VAD(signal=error_test, norma=error_train, frame_len=10, k_th=1.1)

            detected = np.where(vad_detection != 0)[0]
            detected_events = []

            # Обнаружение аномалий
            with open(model_directory + '\\detected_anomalies.txt', 'a') as f:
                for i in range(detected[0], detected[-1] + 1):
                    if error_test[i] > 1e-01:
                        f.write('Обнаружение аномалий\n')
                        f.write('event_id  | error\n')
                        f.write('%d      | %.5e\n' % (test_seq[i], error_test[i]))
                        f.write('%s\t%s\t%s\t%s\n' % (column1[i], column2[i], column3[i], column4[i]))
                        for event_id in detected_events:
                            f.write(f'{event_id}\n')

            column1.clear()
            column2.clear()
            column3.clear()
            column4.clear()
            print("Model is Complete session")
            time.sleep(5)
        except FileNotFoundError:
            print('Ошибка в открытии файла testevent', train_fname)






'''
    plt.figure(figsize=(17, 3))
    plt.plot(range(len(train_data))[100:], error_train[100:])
    plt.hlines(anomal_threshold, 0,
        len(train_data), colors = 'm')
    plt.title('Error on train data')
    plt.ylabel('log(E)')
    plt.xlabel('time')
    plt.grid()
    plt.yscale('log')
    legend_elements = [Line2D([0], [0], color='tab:blue', label='Error'),
                    Line2D([0], [0], color='m', label='Threshold')]
    plt.legend(handles=legend_elements, loc=1)
    plt.savefig(model_directory+'\\'+model_input+'\\train error', dpi=None, facecolor='w', edgecolor='w',
    orientation='portrait', pad_inches=0.1)
    plt.show()
 
plt.figure(figsize=(17,3))
plt.plot(range(len(test_data)), error_test)
plt.hlines(anomal_threshold, 0,
           len(test_data), colors = 'm', label='threshhold')
plt.title('Error on test data')
plt.ylabel('log(E)')
plt.xlabel('time')
plt.grid()
plt.yscale('log')
legend_elements = [Line2D([0], [0], color='tab:blue', label='Error'),
                   Line2D([0], [0], color='m', label='Threshold')]
plt.legend(handles=legend_elements, loc=1)
plt.savefig(model_directory+'\\'+model_input+'\\test error', dpi=None, facecolor='w', edgecolor='w',
        orientation='portrait', pad_inches=0.1)
plt.show()









 
vad_error, vad_detection = VAD(signal=error_test, norma=error_train, frame_len = 10, k_th = 1.1)
 
plt.figure(figsize=(17,3))
plt.plot(range(len(vad_error)), vad_error)
plt.plot(range(len(vad_error)), vad_detection)
plt.yscale("log")
plt.title('Summary error on  test data + VAD detection')
plt.ylabel('log(E)')
plt.xlabel('time')
plt.grid()
legend_elements = [Line2D([0], [0], color='tab:blue', label='Error'),
                   Line2D([0], [0], color='tab:orange', label='Anomalies detection')]
plt.legend(handles=legend_elements, loc=1)
 
plt.savefig(model_directory+'\\'+model_input+'\\summary test error VAD', dpi=None, facecolor='w', edgecolor='w',
        orientation='portrait', pad_inches=0.1)
plt.show()
 
detected = np.where(vad_detection!=0)[0]
detected
 
print('Обнаружение аномалий')
print('№ события  | event_id  | error')
for i in range(detected[0], detected[-1]+1):
    print('%d       | %d      | %.5e' % (i,  test_seq[i], error_test[i]))
if sys.argv[4] =="1":
    np.where(train_seq == 5145)
'''