# Uso de `Instance<Model>` y `ModelRef`

> **Contexto:** `Instance<Model>` es la clase genérica que reemplaza a las antiguas
> `SIR_Instance`, `Lorenz_Instance`, etc. Combina el estado `(state, t)` con un
> `ModelRef<Model>` que puede apuntar a un modelo **propio** (en stack, sin heap)
> o a un modelo **compartido** (puntero raw, cero overhead).

---

## Includes necesarios

```cpp
#include "DynSysVis.hpp"
```

---

## Caso 1 — Uso básico: instancia con modelo propio

```cpp
dsv::mod::Instance<dsv::mod::SIR_Model> sir;

// Sobreescribir el estado inicial
sir.state = { 0.95f, 0.05f, 0.0f };

// Correr 100 pasos con el dispatcher automático
for (int i = 0; i < 100; ++i)
    dsv::sim::step(sir, 0.01f);

std::cout << "SIR t=" << sir.t
          << "  S=" << sir.state[0]
          << "  I=" << sir.state[1]
          << "  R=" << sir.state[2] << "\n";
```

`getModel()` devuelve una referencia directa al modelo interno, sin overhead.

---

## Caso 2 — Modificar parámetros del modelo propio

```cpp
dsv::mod::Instance<dsv::mod::SIR_Model> sir2;
sir2.getModel().beta  = 0.5f;   // más contagioso
sir2.getModel().gamma = 0.02f;

// También puedes llamar integradores directamente
dsv::sim::RK4_step(sir2, 0.01f);
```

---

## Caso 3 — Modelo compartido entre dos instancias

Útil para comparar trayectorias con **condiciones iniciales distintas** pero la
misma física. `vincular()` solo guarda un puntero; no hay copia del modelo.

```cpp
dsv::mod::SIR_Model modelo_comun;
modelo_comun.beta = 0.4f;

dsv::mod::Instance<dsv::mod::SIR_Model> poblacion_A;
dsv::mod::Instance<dsv::mod::SIR_Model> poblacion_B;

poblacion_A.model_ref.vincular(modelo_comun);
poblacion_B.model_ref.vincular(modelo_comun);

poblacion_A.state = { 0.99f, 0.01f, 0.0f };
poblacion_B.state = { 0.80f, 0.20f, 0.0f }; // brote más avanzado

for (int i = 0; i < 50; ++i) {
    dsv::sim::step(poblacion_A, 0.01f);
    dsv::sim::step(poblacion_B, 0.01f);
}

// Cambiar beta afecta a AMBAS instancias en el siguiente step()
modelo_comun.beta = 0.1f; // intervención: cuarentena

for (int i = 0; i < 50; ++i) {
    dsv::sim::step(poblacion_A, 0.01f);
    dsv::sim::step(poblacion_B, 0.01f);
}
```

---

## Caso 4 — Volver al modelo propio

`usar_propio()` inicializa una copia con los **valores default** del modelo,
no con los parámetros actuales del modelo compartido.

```cpp
poblacion_A.model_ref.usar_propio();
poblacion_A.getModel().beta = 0.9f; // solo afecta a poblacion_A

std::cout << poblacion_A.model_ref.es_propio() << "\n"; // true
std::cout << poblacion_B.model_ref.es_propio() << "\n"; // false
```

> **Nota:** Si necesitas desconectarte conservando los parámetros actuales del
> modelo compartido, puedes agregar este método a `ModelRef`:
>
> ```cpp
> void usar_propio_con_copia_actual() {
>     storage = get(); // copia Model actual antes de soltar el puntero
> }
> ```

---

## Caso 5 — Lorenz: misma interfaz, diferente modelo

El dispatcher `step()` detecta `noise_dim == 0` en tiempo de compilación y
elige `E_step` automáticamente. No hay cambios en los integradores.

```cpp
dsv::mod::Instance<dsv::mod::Lorenz_Model> lorenz;
lorenz.state = { 0.1f, 0.0f, 0.0f };

for (int i = 0; i < 1000; ++i)
    dsv::sim::step(lorenz, 0.001f);

std::cout << "Lorenz t=" << lorenz.t
          << "  x=" << lorenz.state[0] << "\n";
```

---

## Referencia rápida de `ModelRef<Model>`

| Método | Descripción |
|---|---|
| `get()` | Devuelve `Model&` (local o compartido, sin overhead) |
| `vincular(Model& m)` | Apunta al modelo externo `m` |
| `usar_propio()` | Desconecta y crea una copia default del modelo |
| `es_propio()` | `true` si el modelo es local (no compartido) |

## Selección automática de integrador (`step()`)

| `noise_dim` | Integrador elegido |
|---|---|
| `0` | `E_step` — Euler, ODE pura |
| `1` | `EM_step_simple` — Euler-Maruyama, un ruido |
| `> 1` | `EM_step` — Euler-Maruyama, matriz de difusión `d × m` |