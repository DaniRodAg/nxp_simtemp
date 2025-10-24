import os
import select
import sys
import select
import time
from datetime import datetime
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QWidget, QHBoxLayout
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import pyqtgraph as pg

DEVICE_PATH = "/dev/simtemp"

class DeviceReader(QtCore.QThread):
    """Reads temperature updates from /dev/simtemp in real time using epoll"""
    new_temperature = QtCore.pyqtSignal(float, float)
    #new_time = QtCore.pyqtSignal(float)

    def __init__(self, device_path):
        super().__init__()
        self.device_path = device_path
        self.running = True

    def run(self):
        try:
            ti = None
            # Open device file in non-blocking mode
            fd = os.open(DEVICE_PATH, os.O_RDONLY | os.O_NONBLOCK)
            ep = select.epoll()
            ep.register(fd, select.EPOLLIN | select.EPOLLPRI)
            while self.running:
                events = ep.poll(1)  # wait up to 1s
                for fileno, event in events:
                    if event & select.EPOLLPRI:
                        data_b = os.read(fd, 1024)
                        if data_b:
                            data_str = data_b.decode('utf-8', errors='ignore').translate(str.maketrans({'=': ' '}))
                            dataset = data_str.split('\n',7)
                            for i in range(len(dataset)):
                                sampl = dataset[i].split(' ')
                                #print(sampl)
                                temp = float(sampl[sampl.index("temp")+1].replace("C",""))

                                # Parse datetime part
                                timestamp_str = " ".join(sampl[:2])
                                t = datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S.%f")

                                # To keep a monotonic delta:
                                if not hasattr(self, "t0"):
                                    self.t0 = t
                                    delta = 0.0
                                else:
                                    delta = (t - self.t0).total_seconds()
                                #temp = float(temp_str)
                                print(temp, delta)
                                self.new_temperature.emit(temp,delta)
                                #self.new_time.emit(time)
                            
        except Exception as e:
            os.close(DEVICE_PATH)
            print(f"[DeviceReader] Error: {e}")

    def stop(self):
        self.running = False
        self.wait()

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Real-Time Temperature Plot")
        self.resize(800, 500)

        # Main container widget
        central_widget = QWidget()
        layout = QHBoxLayout(central_widget)
        self.setCentralWidget(central_widget)

        # Temperature Label
        self.temp_label = QLabel("Temperature: -- C") 
        self.temp_label.setFixedHeight(200)   # <-- keeps it visible
        self.temp_label.setStyleSheet("font-size: 14px; padding: 4px;")
        layout.addWidget(self.temp_label)

        

        # PyQtGraph setup
        self.plot_graph = pg.PlotWidget(title="Temperature vs Time")
        self.plot_graph.showGrid(x=True, y=True)
        self.plot_graph.setLabel("left", "Temperature (C)")
        self.plot_graph.setLabel("bottom", "Time (s)")
#        self.setCentralWidget(self.plot_graph)
        layout.addWidget(self.plot_graph, stretch=1)
        
        self.xdata = []
        self.ydata = []
        #self.start_time = time.time()

        self.curve = self.plot_graph.plot(pen=pg.mkPen('r', width=2))

        # Background thread to read from device
        self.reader = DeviceReader(DEVICE_PATH)
        self.reader.new_temperature.connect(self.update_plot)
        self.reader.start()

    def update_plot(self, temp, time):
        """Append new temperature and update the plot"""
        self.xdata.append(time)
        self.ydata.append(temp)

        if not hasattr(self, 'temp_label') or self.temp_label is None:
            return
        if not self.isVisible():
            return

#        self.temp_label.setText(f"Temperature: {temp:.2f}")

        # Show all buffered temps
        buffer_str = "\n".join(f"{t:.2f}" for t in self.ydata[-8:])
        self.temp_label.setText(f"Temperature: \n{buffer_str}")

        # Keep last 100 samples
        if len(self.xdata) > 100:
            self.xdata.pop(0)
            self.ydata.pop(0)


        self.curve.setData(self.xdata, self.ydata)

    def closeEvent(self, event):
        """Cleanly stop the reader thread when window closes"""
        if hasattr(self, 'reader'):
            self.reader.running = False
            self.reader.wait()  # wait for thread to stop
        event.accept()


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    main = MainWindow()
    main.show()
    sys.exit(app.exec())














