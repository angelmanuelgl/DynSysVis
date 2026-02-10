import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib import cm

# 1. Cargar los datos
try:
    data = pd.read_csv('Apps/GaltonWatson/Datos/experimento_incesto.txt', sep=' ')
except:
    print("No se encontro el archivo datos_tesis.txt")
    exit()

# 2. Preparar los ejes
x = data['Poblacion_Inicial']
y = data['Lambda']
z = data['Gen_Media_Incesto']

# 3. Crear la figura
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')

# Para crear una superficie, necesitamos interpolar o "gridderar" los datos
# si no están en una malla perfecta. 
surf = ax.plot_trisurf(x, y, z, cmap=cm.viridis, linewidth=0.1, antialiased=True)

# 4. Etiquetas y títulos
ax.set_xlabel(r'Población Inicial ($N_0$)')
ax.set_ylabel(r'Tasa de Nacimiento ($\lambda$)')
ax.set_zlabel('Gen. Media de Incesto')
plt.title('Análisis de Consanguinidad: Barrido de Parámetros')

# Añadir barra de color
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()