import os, select
import sys
import re
import select
import pyqtgraph as pg
from PyQt5 import QtWidgets

fd = os.open("/dev/simtemp", os.O_RDONLY | os.O_NONBLOCK)

ep = select.epoll()
ep.register(fd, select.EPOLLIN | select.EPOLLPRI)

while True:
    events = ep.poll(5)
    for fileno, event in events:
        if event & select.EPOLLPRI:
            try:
                data_b = os.read(fd, 1024)
                if data_b:
                    data_str = data_b.decode('utf-8', errors='ignore').translate(str.maketrans({'=': ' '}))
                    dataset = data_str.split('\n',7)
                    print(dataset)
                    sampl = dataset[len(dataset)-2].split(' ')
                    print(sampl)
                    temp_str = sampl[sampl.index("temp")+1].replace("C","")
                    temp = float(temp_str)
                    print(temp)
            except BlockingIOError:
                continue
            
