import tkinter as tk
import matplotlib
import matplotlib.pyplot as plt
from Menu import *

matplotlib.use('TkAgg')

# Consts
# PATH = r"C:\Users\AppTa\Desktop\code 2.7\Python"
PATH = r"C:\Users\AppTa\Desktop\code\PC_side"
COPYRIGHT = "© Copyright: Ofir Tanami & Ariel Moshe | Digital Computer Structure Course | 2020-21"
TITLE = r'$\mathtt{Radar\ Detector\ System\ Project}$'
np.set_printoptions(precision=0)


def main():
    try:
        img = plt.imread(PATH + r"\Pics\radar.jpg")
        # for dialog input
        ROOT = tk.Tk()
        ROOT.withdraw()
        menu = Menu(state='0', title='Radar Detector System')
        while True:
            # Menu State Machine:
            if menu == '0':
                menu.main_page(TITLE, COPYRIGHT, img)
                while menu == '0':
                    plt.pause(0.0001)
                    continue

            # 1. Radar Detector System:
            elif menu == '1':
                menu.sub_menu(title=r'$\mathtt{1. Radar\ Detector\ System}$', copyright=COPYRIGHT)
                r_max = int(menu.get_input(title="Radar Detector System", text="Insert masking distance:"))
                # send start bit
                menu.send('1')
                while menu == '1':
                    menu.radar_detector_system(r_max)
                    plt.pause(0.0001)


            # 2. Telemeter:
            elif menu == '2':
                menu.sub_menu(title=r'$\mathtt{2. Telemeter}$', img=img, copyright=COPYRIGHT)
                # the input dialog
                degree = menu.get_input(title="Degree", text="Please insert degree (0-180):")
                menu.send(degree + '\0')
                while menu == '2':
                    menu.telemeter(degree)
                    plt.pause(0.0001)

            # 3. Script Mode:
            elif menu == '3':
                script_img = plt.imread(PATH + r"\Pics\opcodes.jpg")
                menu.sub_menu(title=r'$\mathtt{3. Script\ Mode}$', img=script_img, box=(.67, .65, .33, .33),
                              copyright=COPYRIGHT)
                while menu == '3':
                    menu.script_mode()
                    plt.pause(0.0001)

            # Exit the program:
            elif menu == '4':
                goodbye = plt.imread(PATH + r"\Pics\goodbye.jpg")
                menu.sub_menu(title="We're sad to see you go :(", img=goodbye, copyright=COPYRIGHT)
                plt.pause(2)
                plt.close()
                menu.serial_comm.close()
                exit("Goodbye!")
    finally:
        menu.serial_comm.close()
        print("Closed Serial COMM")


if __name__ == "__main__":
    main()
