from Xlib import display

def mouse_pos():
    data = display.Display().screen().root.query_pointer()._data
    return data["root_x"], data["root_y"]

