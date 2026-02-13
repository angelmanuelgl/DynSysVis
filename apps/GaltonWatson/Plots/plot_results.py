import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys
from matplotlib import cm


import os

def aplicar_mi_estilo(ax, fig, titulo="ANÁLISIS DE DATOS", etiqueta_z="Eje Z"):
    # 1. Colores base del estilo (Paleta Dark/Neon)
    color_fondo = '#1b1e26'      # Azul muy oscuro (como el de tu imagen)
    color_grid = '#2d333f'       # Gris azulado para la rejilla
    color_texto = '#d1d5db'      # Gris claro para etiquetas
    color_resalte = '#00f2ff'    # Cyan neón para el título

    # 2. Configurar el fondo de la figura y del área de dibujo
    fig.patch.set_facecolor(color_fondo)
    ax.set_facecolor(color_fondo)

    # 3. Título personalizado (usando fuentes monoespaciadas si es posible)
    ax.set_title(titulo.upper(), fontsize=14, fontweight='bold', 
                 color=color_resalte, pad=20, family='monospace')

    # 4. Configurar etiquetas de los ejes
    etiquetas_estilo = {'color': color_texto, 'fontsize': 10, 'family': 'monospace'}
    ax.set_xlabel(r'Población ($N_0$)', **etiquetas_estilo, labelpad=10)
    ax.set_ylabel(r'Lambda ($\lambda$)', **etiquetas_estilo, labelpad=10)
    ax.set_zlabel(etiqueta_z, **etiquetas_estilo, labelpad=10)

    # 5. Estilo de los "Panes" (las paredes del cubo 3D)
    ax.xaxis.set_pane_color(color_fondo)
    ax.yaxis.set_pane_color(color_fondo)
    ax.zaxis.set_pane_color(color_fondo)

    # 6. Rejilla (Grid) sutil
    ax.xaxis._axinfo["grid"]['color'] = color_grid
    ax.yaxis._axinfo["grid"]['color'] = color_grid
    ax.zaxis._axinfo["grid"]['color'] = color_grid
    ax.xaxis._axinfo["grid"]['linestyle'] = '--'
    ax.xaxis._axinfo["grid"]['linewidth'] = 0.5

    # 7. Cambiar color de los números en los ejes (ticks)
    ax.tick_params(axis='x', colors=color_texto, labelsize=8)
    ax.tick_params(axis='y', colors=color_texto, labelsize=8)
    ax.tick_params(axis='z', colors=color_texto, labelsize=8)

    # 8. Quitar líneas de los bordes para un look más limpio
    ax.set_axis_on()

# INPUT
if len(sys.argv) < 2:
    print("Uso: python script.py archivo1.txt archivo2.txt ...")
    sys.exit(1)

# PARA CADA ARCHIVO
for i, file_path in enumerate(sys.argv[1:], start=1):
    try:
        with open(file_path, 'r') as f:
            titulo_archivo = f.readline().strip()
        
        data = pd.read_csv(file_path, sep=' ', skiprows=1)
        
        # Crear una nueva figura con vetana unica
        fig = plt.figure(i, figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        
        x, y, z = data['Poblacion_Inicial'], data['Lambda'], data['Gen_Media_Incesto']
        surf = ax.plot_trisurf(x, y, z, 
                       cmap='inferno',     # 'viridis' o 'magma' quedan muy bien
                       linewidth=0.5,      # Líneas delgaditas entre triángulos
                       edgecolor='#2d333f',# Color de la malla
                       antialiased=True,
                       alpha=0.8 )
        

        # ESTILO
        aplicar_mi_estilo(ax, fig,  titulo=titulo_archivo, etiqueta_z="Gen Incesto Media")    

        # fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)
        #  colorbar 
        color_texto = '#d1d5db'
        cbar = fig.colorbar(surf, ax=ax, shrink=0.5, aspect=10)
        cbar.ax.yaxis.set_tick_params(color=color_texto)
        plt.setp(plt.getp(cbar.ax.axes, 'yticklabels'), color=color_texto)

    except Exception as e:
        print(f"Error al procesar {file_path}: {e}")

# MOSTRAR TODO
plt.show()