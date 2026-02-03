# InsightRT - Libreria Grafica de datops para Simulaciones en tiempo Real

**InsightRT** 
es una herramienta (en desarrollo) para la visualización de datos en tiempo real y análisis de sistemas dinámicos desarrollada en C++ utilizando la biblioteca SFML. 

Está diseñada para integrarse fácilmente en simulaciones complejas, permitiendo monitorear mediante gráficas temporales y retratos de fase.

## Caracteristicas
* **Visualización en Tiempo Real**: Gráficas de evolución temporal con sombreado de degradado (gradient fill).
* **Análisis de Espacio Fase**: Gráficas de trayectoria (X vs Y) para estudio de sistemas dinámicos.
* **Interfaz Adaptativa**: Paneles con bordes redondeados y títulos dinámicos que ajustan el área de dibujo automáticamente.
* **Arquitectura Extensible**: Basada en herencia para facilitar la creación de nuevos tipos de visualizaciones.

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



