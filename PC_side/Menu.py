import numpy as np
import serial as ser
import time
from matplotlib.widgets import Button
from tkinter import simpledialog
from main import *
from Script import Script


def init_serial_comm():
    serial_comm = ser.Serial('COM6', baudrate=9600, bytesize=ser.EIGHTBITS,
                             parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE)
    if not serial_comm.isOpen():
        serial_comm.open()
    print('com4 is open', serial_comm.isOpen())
    serial_comm.reset_input_buffer()
    serial_comm.reset_output_buffer()
    return serial_comm


class Menu:
    def __init__(self, state='0', title='Radar Detector System', dpi=150):
        self.state = state
        self.buttons = []
        self.fig = plt.figure(facecolor='#46fd3d')
        self.init_figure(title, dpi)
        self.clear = False
        self.s = Script()
        self.serial_comm = init_serial_comm()

    def __eq__(self, char):
        return self.state == char

    def init_figure(self, title, dpi):
        win = self.fig.canvas.manager.window  # figure window
        screen_res = win.wm_maxsize()  # used for window formatting later
        self.fig.set_dpi(dpi)  # set figure resolution
        percent = .96
        self.fig.set_size_inches(percent * (screen_res[0] / dpi), percent * (screen_res[1] / dpi))
        plot_res = self.fig.get_window_extent().bounds  # window extent for centering
        win.wm_geometry('+{0:1.0f}+{1:1.0f}'. \
                        format((screen_res[0] / 2.0) - (plot_res[2] / 2.0),
                               (screen_res[1] / 2.0) - (plot_res[3] / 2.0)))  # centering plot
        self.fig.canvas.toolbar.pack_forget()  # remove toolbar for clean presentation
        self.fig.canvas.set_window_title(title)
        self.fig.canvas.draw()  # draw before loop
        self.fig.show()

    def main_page(self, title, copyright, img):
        plt.clf()
        self.buttons = []  # clear buttons
        self.put_image(img)
        # Create menu and buttons:
        self.insert_title(title)
        self.insert_text(0.2, .02, copyright, size=10)
        self.add_button([0.41, 0.63, 0.2, 0.05], '1. Radar Detector System', '#00e145', '#fdff56', self.menu_event)
        self.add_button([0.41, 0.53, 0.2, 0.05], '2. Telemeter', '#1dff72', '#fdff56', self.menu_event)
        self.add_button([0.41, 0.43, 0.2, 0.05], '3. Script Mode', '#00e145', '#fdff56', self.menu_event)
        self.add_button([0.41, 0.33, 0.2, 0.05], '4. Exit', '#ff0000', '#fb91ff', self.menu_event)

    def insert_text(self, x, y, title, size=32):
        temp = self.fig.text(x, y, title, fontsize=size)
        plt.pause(0.000001)
        return temp

    def insert_title(self, title):
        self.fig.text(0.5, 0.9, title, size=30,
                      ha="center", va="center",
                      bbox=dict(boxstyle="square",
                                ec=(0., 0., 0.),  # RGB of frame
                                fc=(0.12, .87, .52),  # RGB of background
                                )
                      )
        plt.pause(0.000001)

    def add_button(self, location, name, color, font_color, event):
        button_ax = self.fig.add_axes(location)
        self.buttons.append(Button(button_ax, name, color=color, hovercolor=font_color))
        self.buttons[-1].on_clicked(event)

    def draw_radar(self, r_max=450):
        ax = self.fig.add_subplot(111, polar=True, facecolor='#08752a')
        ax.set_position([-0.05, -0.05, 1.1, 1.05])
        ax.set_ylim([0.0, r_max])  # range of distances to show
        ax.set_xlim([0.0, np.pi])  # limited by the servo span (0-180 deg)
        ax.tick_params(axis='both', colors='w')

        ax.set_rticks(np.linspace(0.0, r_max, 7).round())  # show 5 different distances
        ax.set_thetagrids(np.linspace(0.0, 180.0, 9))  # show 9 angles
        axbackground = self.fig.canvas.copy_from_bbox(ax.bbox)  # background to keep during loop
        return ax

    def radar_detector_system(self, r_max=450):
        self.add_button([0.657, 0.08, 0.125, 0.05], 'Exit', '#ff9000', '#aec9ff', self.radar_event)
        self.add_button([0.212, 0.08, 0.125, 0.05], 'Clear', '#ff0000', '#aec9ff', self.radar_event)
        self.add_button([0.44, 0.08, 0.125, 0.05], 'Menu', '#1d67ff', '#aec9ff', self.menu_event)

        ax = self.draw_radar(r_max)
        degrees = np.arange(0, 181, 1)  # 0 - 180 degrees
        theta = degrees * (np.pi / 180.0)  # to radians
        dists = np.ones((len(degrees),))  # dummy distances until real data comes in
        pols, = ax.plot([], linestyle='', marker='o', markerfacecolor='w',
                        markeredgecolor='#EFEFEF', markeredgewidth=1.0,
                        markersize=5.0, alpha=0.9)  # dots for radar points
        line, = ax.plot([], color='r',
                        linewidth=3.0)  # sweeping arm plot

        # main loop:
        while self.state == '1':
            plt.pause(0.000001)
            if self.state == '1' and self.serial_comm.in_waiting:  # while the input buffer isn't empty
                data = self.serial_comm.read_until(b'\r').decode("ascii")
                try:
                    degree, dist = data.split('\n')
                    dist = dist.replace('\r', '')
                    print(f"Degree: {degree} Distance: {dist} [cm]")
                except:
                    print("Error 1")
                    continue

                degree = int(degree)
                dist = int(dist)
                if dist > r_max:
                    continue  # measuring more than r_max, it's likely inaccurate
                dists[int(degree)] = dist
                pols.set_data(theta, dists)
                ax.draw_artist(pols)
                line.set_data(np.repeat((degree * (np.pi / 180.0)), 2),
                              np.linspace(0.0, r_max, 2))
                ax.draw_artist(line)
                self.fig.canvas.blit(ax.bbox)  # replot only data

                if self.clear:
                    ax.clear()
                    ax = draw_radar()

                    # self.fig.canvas.restore_region(axbackground)
                    self.fig.canvas.flush_events()  # flush for next plot
                    dists = np.ones((len(degrees),))  # dummy distances until real data comes in
                    pols, = ax.plot([], linestyle='', marker='o', markerfacecolor='w',
                                    markeredgecolor='#EFEFEF', markeredgewidth=1.0,
                                    markersize=5.0, alpha=0.9)  # dots f
                    self.clear = False
            time.sleep(0.01)

    def telemeter(self, degree):
        self.add_button([0.44, 0.08, 0.125, 0.05], 'Menu', '#1d67ff', '#aec9ff', self.menu_event)
        self.fig.text(.5, .3, f"Degree: {degree} [degrees]", size=20,
                                   ha="center", va="center",
                                   bbox=dict(boxstyle="round",
                                             ec=(0., 0., 0.),  # RGB of frame
                                             fc=(0.6, .8, .9),  # RGB of background
                                             )
                                   )
        temp = None
        # main loop:
        while self.state == '2':
            plt.pause(0.000001)
            if self.state == '2' and self.serial_comm.in_waiting:  # while the input buffer isn't empty
                data = self.serial_comm.read_until(b'\r').decode("ascii")
                try:
                    degree, dist = data.split('\n')
                    dist = dist.replace('\r', '')
                    print(f"Degree: {degree} Distance: {dist} [cm]")
                except:
                    print("Error 2")
                    continue
                if temp is not None:
                    temp.remove()
                temp = self.fig.text(.5, .5, f"Distance: {dist} [cm]", size=50,
                                     ha="center", va="center",
                                     bbox=dict(boxstyle="round",
                                               ec=(0., 0., 0.),  # RGB of frame
                                               fc=(1., 1, 1),  # RGB of background
                                               )
                                     )
                plt.pause(.0001)
            time.sleep(0.01)

    def script_mode(self, r_max = 50):
        ax = self.draw_radar(r_max)
        degrees = np.arange(0, 181, 1)  # 0 - 180 degrees
        theta = degrees * (np.pi / 180.0)  # to radians
        dists = np.ones((len(degrees),))  # dummy distances until real data comes in
        pols, = ax.plot([], linestyle='', marker='o', markerfacecolor='w',
                        markeredgecolor='#EFEFEF', markeredgewidth=1.0,
                        markersize=5.0, alpha=0.9)  # dots for radar points
        line, = ax.plot([], color='r',
                        linewidth=3.0)  # sweeping arm plot
        plt.pause(.25)
        x = p = l = r = '0'
        d = '50'
        opcode = {'1': 'blink_rgb(', '2': 'rlc_leds(', '3': 'rrc_leds(', '4': 'set_delay(',
                  '5': 'clear_all_leds', '6': 'servo_deg(', '7': 'servo_scan(', '8': 'sleep'}

        self.s.script_num_char = self.get_input("Script Mode", "Choose Script number from 1-3: ")
        while self.s.script_num_char not in '123':
            self.s.script_num_char = self.get_input("Script Mode", "Wrong input! Choose Script number from 1-3: ")

        self.send(self.s.script_num_char)
        self.s.script_num = int(self.s.script_num_char) - 1

        if not self.s.written[self.s.script_num]:
            text = self.insert_text(.2, .1, "Writing Script Number " + self.s.script_num_char + " to Flash...",  size=25)
            self.s.names[self.s.script_num] = PATH + '\Scripts\Script' + self.s.script_num_char + '.txt'
            with open(self.s.names[self.s.script_num], 'r') as scriptFile:
                # Send script size
                self.s.sizes[self.s.script_num] = str(len(scriptFile.read()))
                scriptFile.seek(0)  # back to start of the file
                self.send(self.s.sizes[self.s.script_num] + '\0')

                # Send script lines
                self.s.lines[self.s.script_num] = str(sum(1 for line in scriptFile))
                scriptFile.seek(0)  # back to start of the file
                self.send(self.s.lines[self.s.script_num] + '\0')

                # Send chars of script
                for line_file in scriptFile:
                    for char in line_file:
                        plt.pause(0.000001)
                        if char != '\n':
                            self.s.script_opc[self.s.script_num] += char
                        if char == line_file[-1]:
                            self.s.script_opc[self.s.script_num] += " " + opcode[line_file[1]]
                            if line_file[1] in "123":
                                x = str(int(line_file[2:-1], 16))
                                self.s.script_opc[self.s.script_num] += x + ', ' + d + ')'
                            elif line_file[1] == "4":
                                d = str(int(line_file[2:-1], 16))
                                self.s.script_opc[self.s.script_num] += d + ')'
                            elif line_file[1] == "6":
                                p = str(int(line_file[2:-1], 16))
                                self.s.script_opc[self.s.script_num] += p + ')'
                            elif line_file[1] == "7":
                                l = str(int(line_file[2:4], 16))
                                r = str(int(line_file[4:-1], 16))
                                self.s.script_opc[self.s.script_num] += l + ', ' + r + ')'
                            if line_file[-1] == '\n':
                                self.s.script_opc[self.s.script_num] += '\n'

                        if char == '\n':
                            self.send('\0')
                        else:
                            self.send(char)

                        acknowledge = 0
                        while not acknowledge:
                            if self.serial_comm.in_waiting:  # while the input buffer isn't empty
                                acknowledge = self.serial_comm.readline().decode("ascii").replace('\n\r', '')
                            time.sleep(0.01)

                scriptFile.close()
                self.s.written[self.s.script_num] = 1
        else:
            text = self.insert_text(.3, .1, "Script " + self.s.script_num_char + " Already Written",  size=25)
            plt.pause(0.5)

        self.fig.text(0.01, 0.80, self.s.script_opc[self.s.script_num], size=10,
                                     va="center",
                                    bbox=dict(boxstyle="round",
                                              ec=(0., 0., 0.),  # RGB of frame
                                              fc=(0.6, .8, .9),  # RGB of background
                                              )
                                    )
        text.remove()
        self.insert_text(.3, .1, "Executing script number " + self.s.script_num_char, size=25)

        # Get distance and degree for servo_deg and servo_scan:
        acknowledge = ''
        self.serial_comm.reset_input_buffer()
        while acknowledge != '500\n\r':  # Wait for the script to finish
            plt.pause(0.000001)
            if self.serial_comm.in_waiting:  # while the input buffer isn't empty
                acknowledge = data = self.serial_comm.read_until(b'\r').decode("ascii")
                if acknowledge != '500\n\r':
                    try:
                        degree, dist = data.split('\n')
                        dist = dist.replace('\r', '')
                        print(f"Degree: {degree} Distance: {dist} [cm]")
                    except:
                        print("Error 3")
                        continue

                    degree = int(degree)
                    dist = int(dist)
                    if dist > r_max:
                        continue  # measuring more than r_max, it's likely inaccurate
                    dists[int(degree)] = dist
                    pols.set_data(theta, dists)
                    ax.draw_artist(pols)
                    line.set_data(np.repeat((degree * (np.pi / 180.0)), 2),
                                  np.linspace(0.0, r_max, 2))
                    ax.draw_artist(line)
                    self.fig.canvas.blit(ax.bbox)  # replot only data
            time.sleep(0.01)
        self.state = '0'

    def put_image(self, img, box=(0.1, 0.1, .8, .8)):
        newax = self.fig.add_axes(box)
        newax.imshow(img)
        newax.axis('off')

    def get_input(self, title, text):
        return simpledialog.askstring(title=title, prompt=text)

    def send(self, data, delay=.25):
        self.serial_comm.write(bytes(data, 'ascii'))
        time.sleep(delay)  # delay for accurate read/write operations on both
        print(f"Send: {data}")

    def sub_menu(self, title, copyright, img=None, box=(0.1, 0.1, .8, .8)):
        plt.clf()
        if img is not None:
            self.put_image(img, box)
        self.insert_title(title)
        self.insert_text(0.2, .02, copyright, size=10)

    def menu_event(self, event):
        choice = str(event.inaxes)[12]
        d = {'6': '1', '5': '2', '4': '3', '3': '4', '0': '0'}
        self.state = d[choice]  # d[choice]
        self.send(self.state)

    def radar_event(self, event):
        choice = str(event.inaxes)
        if choice == "Axes(0.212,0.08;0.125x0.05)":
            self.clear = True
        else:
            self.state = '4'
            self.send(self.state)

