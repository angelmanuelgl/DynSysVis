# InsightRT - Libreria Grafica de datops para Simulaciones en tiempo Real

**InsightRT** 
es una herramienta (en desarrollo) para la visualización de datos en tiempo real y análisis de sistemas dinámicos desarrollada en C++ utilizando la biblioteca SFML. 

Está diseñada para integrarse fácilmente en simulaciones complejas, permitiendo monitorear mediante gráficas temporales y retratos de fase.
**Ajustando tamaños** y **posiciones** muy facilmente usando un **sistema de panales** implementados por mi mismo


## Caracteristicas
* **Visualización en Tiempo Real**: Gráficas de evolución temporal con sombreado de degradado (gradient fill).
* **Análisis de Espacio Fase**: Gráficas de trayectoria (X vs Y) para estudio de sistemas dinámicos.
* **Interfaz Adaptativa**: Paneles con bordes redondeados y títulos dinámicos que ajustan el área de dibujo automáticamente.
* **Arquitectura Extensible**: Basada en herencia para facilitar la creación de nuevos tipos de visualizaciones.


## Ejemplos de Uso: 


**Sistema de ecuaciones Diferenciales (Modelo De Colonia de Hormigas)**

![Image](https://github.com/user-attachments/assets/6257ad01-88e9-4828-8d25-a0eb1c262ee9)



**Ecuaciones Diferenciales De Segundo Orde (Pendulo Simple con resistencia al aire)**

![Image](https://github.com/user-attachments/assets/f2ebe176-b8e2-4021-9a7c-a3fe4edd0fc4)


**Procesos Estocasticos (SpatialBranchingProcesses)**

![Image](https://github.com/user-attachments/assets/63f04406-5a85-4423-8a49-52073ea1c5de)





## MAS Ejemplos de Uso: 


**ECUACIONES DIFERENCIALES ORDINARIAS (EDOS)** 
* Dinámica de Poblaciones: Visualización en tiempo real de modelos de crecimiento (Malthus, Logístico).
* Cinética Química: Monitoreo de la concentración de reactivos y productos en una reacción.
* Modelos Epidemiológicos: Seguimiento de curvas SIR (Susceptibles, Infectados, Recuperados) durante una simulación.

**SISTEMAS DE EDOS (SISTEMAS ACOPLADOS)**  | Gracias al sistema de Paneles Adaptativos, puedes monitorear múltiples variables interdependientes simultáneamente.
* Ecología Competitiva: Modelos de Depredador-Presa (Lotka-Volterra)
* Circuitos Eléctricos: Análisis de redes complejas (Leyes de Kirchhoff) donde varias corrientes y voltajes varían a la vez.
* Sistemas de Control: Visualización de la respuesta de un controlador ante perturbaciones externas.

**ECUACIONES DIFERENCIALES DE SEGUNDO ORDEN** |  Permite generar Retratos de Fase (Posición vs. Velocidad).
* Mecánica Clásica: Simulación de osciladores armónicos, péndulos simples y dobles (con o sin resistencia del aire).
* Vibraciones Mecánicas: Análisis de amortiguamiento en estructuras o sistemas de suspensión.
* Circuitos RLC: Comportamiento de la carga y corriente en circuitos con inductores y capacitores.

**PROCESOS ESTOCÁSTICOS Y PROBABILÍSTICOS** 

Biología Matemática: Procesos de ramificación como el de Dalton-Watson
Cadenas de Markov: Visualización de la evolución de estados en sistemas probabilísticos.
Finanzas Cuantitativas: Modelado de caminatas aleatorias o movimiento browniano para simular fluctuaciones de mercado.




## Requisitos
* Compilador de C++ (GCC/MinGW recomendado).
* [SFML](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library) instalada y configurada.

## 📂 Estructura del Proyecto

* `lib_grafica/`: La libreria en si, aqui esta todo el funcionamiento
* `apps/`: Experimentos y simulaciones que utilizan la librería.
* `assets/`: Recursos compartidos (Fuentes como Roboto, archivos de configuración).
* `build/`: Directorio para ejecutables y archivos objeto.



## 🚀 Instrucciones de Compilación

### PASO A PASO
Para no recompilar la librería entera cada vez que hagas un cambio en tus simulaciones, primero genera los archivos objeto:

```bash
g++ -c lib_grafica/src/Geometria.cpp -I lib_grafica/include -o build/Geometria.o
g++ -c lib_grafica/src/Graficas.cpp -I lib_grafica/include -o build/Graficas.o
g++ -c lib_grafica/src/Panel.cpp -I lib_grafica/include -o build/Panel.o
```
luego compilar cada proyecto proyecto

**Sistema de ecuacines diferenciales para poblaciones (HORMIGAS)**
```bash
g++ apps/hormigas/main.cpp build/Geometria.o build/Graficas.o -I lib_grafica/include -o build/hormigas.exe -lsfml-graphics -lsfml-window -lsfml-system
```
**Ecuacion diferencial de segundo Orden (PENDULO)**
```bash
g++ apps/pendulo/pendulo.cpp build/Geometria.o build/Graficas.o build/Panel.o -I lib_grafica/include -o build/pendulo.exe -lsfml-graphics -lsfml-window -lsfml-system
```
**Proceso Bisexual de Dalton Watson (PULPOS)**
```bash
g++ apps/GaltonWatson/simulacionconArbol.cpp build/Geometria.o build/Graficas.o -I lib_grafica/include -o build/pulpos.exe -lsfml-graphics -lsfml-window -lsfml-system
```

### Usando MakeFIle

Para faciliatar esto puedes usar makefile, solo asegurate de tene risntalado ``pacman -S mingw-w64-ucrt-x86_64-make``

Una vez que ejecutas ``mingw32-make`` dentro de lib_grafica, se genera el archivo lib/libinsightrt.a.

leugo compialr proyectos con
```
mingw32-make run APP=apps/pendulo/pendulo.cpp
mingw32-make run APP=apps/hormigas/main.cpp
mingw32-make run APP=apps/GaltonWatson/simulacionconArbol.cpp
```


## algunas visualizaciones externas

Para representar grafos rapdiamente usar
https://edotor.net/

```
python .\apps\GaltonWatson\Plots\plot_results.py
```