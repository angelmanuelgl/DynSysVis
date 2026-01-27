# InsightRT - Framework de Telemetría para Simulaciones

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

## ompilación
Para compilar el proyecto manualmente, utiliza el siguiente comando:

```bash
g++ main.cpp src/Geometria.cpp src/Graficas.cpp -I include -o hormigas.exe -lsfml-graphics -lsfml-window -lsfml-system
g++ pendulo.cpp src/Geometria.cpp src/Graficas.cpp -I include -o pendulo.exe -lsfml-graphics -lsfml-window -lsfml-system
```