from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas

class QtAxes(FigureCanvas):
    def __init__(self):
        FigureCanvas.__init__(self, Figure())
        self.ax = self.figure.add_subplot(111)
        self.draw()