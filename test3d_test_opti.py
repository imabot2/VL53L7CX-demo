# pip install pyserial
# pip install numpy
# pip install matplotlib


import matplotlib
matplotlib.use("TkAgg")

import serial
import numpy as np
import matplotlib.pyplot as plt


ser = serial.Serial("/dev/ttyACM0", 115200)

image_width = 8

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

x = np.arange(image_width)
y = np.arange(image_width)
X, Y = np.meshgrid(x, y)

baryX = baryY = baryZ = None  # initialisation

while True:
    line = ser.readline().decode().strip()

    # --- Matrice ---
    if line.startswith("DATA:"):
        values = line.replace("DATA:", "").split(",")

        if len(values) == 64:
            Z = np.array(values, dtype=float).reshape(image_width, image_width)

            ax.clear()
            ax.plot_surface(X, Y, Z, cmap='cool')

            # afficher le barycentre si il est existant
            if baryX is not None:
                ax.scatter(baryX, baryY, baryZ, color='red', s=50)

            ax.set_xlabel("X")
            ax.set_ylabel("Y")
            ax.set_zlabel("Distance (mm)")
            #plt.pause(0.01)

    # --- Barycentre ---
    elif line.startswith("BARY:"):
        parts = line.replace("BARY:", "").split(",")
        if len(parts) == 3:
            baryX = float(parts[0])
            baryY = float(parts[1])
            baryZ = float(parts[2])
            print("Barycentre updated:", baryX, baryY, baryZ)
