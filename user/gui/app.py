import numpy as np
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout
import pyqtgraph as pg
from PyQt5 import QtWidgets
import os

device_path = "/dev/simtemp"

try:
    # Open the device file
    fd = os.open(device_path, os.O_RDWR)

    # Read data from the device (if supported by the driver)
    # Read up to 128 bytes from the file descriptor (fd)
    data = os.read(fd, 128)

    # Decode the byte data into a string and then process it
    read_data = data.splitlines()[0]
    print(f"{read_data}")

except OSError as e:
    print(f"Error interacting with device driver: {e}")

finally:
    if 'fd' in locals() and fd:
        os.close(fd)


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        # Temperature vs time plot
        self.plot_graph = pg.PlotWidget()
        self.setCentralWidget(self.plot_graph)
        time = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        temperature = [30, 32, 34, 32, 33, 31, 29, 32, 35, 30]
        self.plot_graph.setLabel(
                "left",
                '<span style="color: red; font-size: 18px">Temperature (°C)</span>'
        )
        self.plot_graph.setLabel(
                "bottom",
                '<span style="color: red; font-size: 18px">Time (s)</span>'
        )

        self.plot_graph.plot(time, temperature)

app = QtWidgets.QApplication([])
main = MainWindow()
main.show()
app.exec()
