# DynSysVis - Documentación de Desarrollo



# **Objeto** (Clase Generica Abstracta)
Es la base de todos los elementos que pueden estar dentro de un Panel. Al heredar de esta clase, cualquier componente nuevo se vuelve automáticamente compatible con el sistema de **Paneles**.

* **Gestión de Recursos:** La clase se encarga de la carga estática de fuentes para optimizar el uso de memoria en todas las instancias.
* **Compatibilidad:** Para crear un objeto compatible, solo se debe implementar el método virtual puro `draw`.

```cpp
class Objeto {
public:
    static sf::Font fuenteCompartida; 
    virtual void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize) = 0;
};

```

## **GraficaBase** (Clase hija de `Objeto`)
Clase intermedia que gestiona la lógica común para representaciones en el plano Cartesiano. De ella derivan las gráficas especializadas.

* **`GraficaTiempo`**: Visualización de variables que evolucionan cronológicamente.
* **`EspacioFase2D`**: Representación de trayectorias y estados del sistema.

### **Gestión de Series de Datos**
Permite manejar múltiples flujos de información (series) de forma independiente.
* **Declarar serie:** `void agregarSerie(std::string nombre, sf::Color color)`
* **Agregar puntos:** `void push_back(float x, float y, std::string clave ="")`

### **Estética y Visualización**
Cada serie cuenta con efectos visuales configurables:
* **Efectos de rastro:** `void ponerDesvanecido(bool desvanece, bool cabeza)`
* **Marcador frontal:** `void ponerCabeza(bool cabeza)`
* **Memoria de datos:** `void configurarMaxPoints(int mp)`

### **Control de Escala y Ventana**
- **Auto-escalado dinámico:** escalado dinamico (la escala se va actualizando) los límites se ajustan automáticamente según los puntos presentes.
- **Límites manuales:** la posibildid de elejir la escala, ya activar o desactivar  el auto escalado
        `` void configurarLimites( float mx, float MX, float my, float MY, bool autoEsc = false ) ``
       

## **EspacioFase2D** (clase Hija de `GraficaBase`)

### **Cosas utiles heredadas**
* Para cada "serie" se marca el punto inicial y la cola con desvanecido (ambos true por defecto)
                ``void ponerDesvanecido( bool desvanece, bool cabeza )``
                ``void ponerCabeza( bool cabeza )``
* Para cambiar el tamaño de la cola se usa
                ``void configurarMaxPoints(int mp) ``

    

### **Control de Escala y Ventana**
Al igual que la clase madre `GraficaBase` tiene **Límites manuales:** y **Auto-escalado dinámico:**

**Seguimiento de datos:** Se agrega la opcion que permite que la ventana se desplace con los nuevos datos: `void activarSeguimiento(bool s)` 

*Al estar **desactivado** la escala va aumentando simpre. Es decir se va ajustandos segun todo el recorrido
*Al estar **activado**. Es decir se reduce para tener la cola actual.

La diferencia esta en seguir el punto por ejemplo (t,t) con una cola de maximo 1000 digamos, si estamso en t =2000, 
* *activado** Se vera una ventana desde [1000,2000] (ancho y alto)
* **desactivado** Se vera una ventana desde [0,2000] (ancho y alto)
    
Segun el caso nos puede interesar uno y otro
* *activado** Vigilar que tan lejos se van variables en el espacio fase, siguiendo una **direccion** de equilibrio
* **desactivado** Vigilar el comportamineto a **detalle** de la curva en el espacio fase.
