"""
This demo demonstrates how to embed a matplotlib (mpl) plot 
into a PyQt4 GUI application, including:

* Using the navigation toolbar
* Adding data to the plot
* Dynamically modifying the plot's properties
* Processing mpl events
* Saving the plot to a file from a menu

The main goal is to serve as a basis for developing rich PyQt GUI
applications featuring mpl plots (using the mpl OO API).

Eli Bendersky (eliben@gmail.com)
License: this code is in the public domain
Last modified: 19.01.2009
"""

from PyQt4.QtCore import SIGNAL
from PyQt4.QtGui import QMainWindow, QFileDialog, QMessageBox, QHBoxLayout, QVBoxLayout, QLabel, QAction, QIcon, QWidget
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure


class AppForm(QMainWindow):
    def __init__(self, parent=None, hor=1, ver=1):
        QMainWindow.__init__(self, parent)
        self.setWindowTitle('Curious iCub')

        self.create_menu()
        self.create_main_frame(hor, ver)
        self.create_status_bar()

        self.on_draw()

    def save_plot(self):
        #parse filters
        filetypes = self.canvas.get_supported_filetypes()
        default_filetype = self.canvas.get_default_filetype()
        sorted_filetypes = filetypes.items()
        sorted_filetypes.sort()
        
        selected_filter = None
        filters = []
        for i, (ext, name) in enumerate(sorted_filetypes):
            filter = '%s (*.%s)' % (name, ext)
            filters.append(filter)
            if ext == default_filetype:
                selected_filter = filter
                
        filters = '\n'.join(filters)
        
        path = unicode(QFileDialog.getSaveFileName(self, 'Save file', '', filters, selectedFilter=selected_filter))
        if path:
            self.canvas.print_figure(path, dpi=self.dpi)
            self.statusBar().showMessage('Saved to %s' % path, 2000)

    def on_about(self):
        msg = """ Plotting is done in PyQT.
        """
        QMessageBox.about(self, "About the curious iCub", msg.strip())
        
    def on_draw(self):
        self.canvas.draw()
                                 
    def create_main_frame(self, hor, ver):
        self.main_frame = QWidget()

        # Create the mpl Figure and FigCanvas objects. 
        # 5x4 inches, 100 dots-per-inch
        self.dpi = 100
        self.fig = Figure((5.0, 4.0), dpi=self.dpi)
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self.main_frame)

        self.axes=[None]*hor*ver
        for iax in xrange(hor*ver):
            self.axes[iax] = self.fig.add_subplot(ver, hor, iax + 1)

        # Create the navigation toolbar, tied to the canvas
        self.mpl_toolbar = NavigationToolbar(self.canvas, self.main_frame)

        hbox = QHBoxLayout()
        vbox = QVBoxLayout()
        vbox.addWidget(self.canvas)
        vbox.addWidget(self.mpl_toolbar)
        vbox.addLayout(hbox)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)

    def set_status_text(self, text):
        ""
        #self.statusBar().showMessage(text)
        #self.repaint()
        
    def create_status_bar(self):
        self.status_text = ""
        self.statusBar().addWidget(QLabel("Status message"), 1)


    def create_menu(self):        
        self.file_menu = self.menuBar().addMenu("&File")

        load_file_action = self.create_action("&Save plot", shortcut="Ctrl+S", slot=self.save_plot, tip="Save the plot")
        quit_action = self.create_action("&Quit", slot=self.close, shortcut="Ctrl+Q", tip="Close the application")

        self.add_actions(self.file_menu, (load_file_action, None, quit_action))

        self.help_menu = self.menuBar().addMenu("&Help")
        about_action = self.create_action("&About", shortcut='F1', slot=self.on_about, tip='About the demo')

        self.add_actions(self.help_menu, (about_action,))

    def add_actions(self, target, actions):
        for action in actions:
            if action is None:
                target.addSeparator()
            else:
                target.addAction(action)

    def create_action(self, text, slot=None, shortcut=None,
                        icon=None, tip=None, checkable=False,
                        signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

