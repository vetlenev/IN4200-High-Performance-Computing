import numpy as np
import os

random_temp = np.round(np.random.normal(loc=5, scale=5, size=100), 1)
random_sec = np.random.randint(low=0, high=86400, size=100)
random_sec = np.sort(random_sec)

hours = random_sec/3600
hours_remain = hours - np.floor(hours)

hours = np.round(hours - hours_remain).astype('int')
minutes = np.round(hours_remain*60).astype('int')

file = open('temps.txt', 'w+')

for i in range(len(random_temp)):
    file.write(f'{hours[i]}:{minutes[i]} {random_temp[i]}\n')

file.close()



