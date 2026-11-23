# Sistemas Dinámicos


Los modelos de sistemas dinamicos que abarca DynSysVis se caracterizan por 

1. **Dominio temporal $\mathcal{T}$:** continuo ($\mathcal{T} = [t_0, T] \subseteq \mathbb{R}_{\ge 0}$) o discreto ($\mathcal{T} = \{0, 1, 2, \dots, N\} \subseteq \mathbb{Z}_{\ge 0}$).
2. **Espacio de estados $\mathcal{X}$:** continuo ($\mathcal{X} \subseteq \mathbb{R}^d$) o discreto ($\mathcal{X} \subseteq \mathbb{Z}^d$, o un conjunto numerable).
3. **Regla de evolución:** determinista (un flujo o mapa determinista) o estocástica (un proceso de Markov o semimartingala).


| Dominio Temporal | Determinista | Estocástico |
| :--- | :--- | :--- |
| **Continuo** ($t \in \mathbb{R}_{\ge 0}$) | **Sistema de EDOs**<br>$\frac{dx(t)}{dt} = f(x(t), t)$ | **Sistema de SDEs (Itô)**<br>$dX_t = \mu(X_t, t)\,dt + \sigma(X_t, t)\,dW_t$ |
| **Discreto** ($n \in \mathbb{Z}_{\ge 0}$) | **Sistema de  Ec. en Dif.**<br>$x_{n+1} = f(x_n, n)$ | **Sistema de Ec. en Dif. Estocásticas**<br>$X_{n+1} = F(X_n, \xi_{n+1}, n)$ |

| Enfoque | Tiempo | Ruido / Estocasticidad | Estructura matemática típica |
| :--- | :--- | :--- | :--- |
| **Sistema de EDOs** | Continuo ($t \in \mathbb{R}$) | Determinista | $\frac{dz}{dt} = f(z, t)$ |
| **Sistema de SDEs** | Continuo ($t \in \mathbb{R}$) | Continuo (Wiener / Lévy) | $dZ_t = \mu(Z_t, t)dt + \sigma(Z_t, t)dW_t$ |
| **Sistema de ec. en dif.** | Discreto ($n \in \mathbb{Z}_{\ge 0}$) | Determinista | $z_{n+1} = f(z_n, n)$ |
| **Sistema de ec. en dif. estocásticas** | Discreto ($n \in \mathbb{Z}_{\ge 0}$) | Discreto o continuo ($\xi_{n+1}$) | $Z_{n+1} = F(Z_n, \xi_{n+1}, n)$ |

* EDOs/ODEs (Ecuaciones Diferenciales Ordinarias/ Ordinary differential equations)
* SDEs/SDEs (Ecuaciones Diferenciales Estocasticas/ Stochastic differential equations)
* Ec. en Dif. (Ecuaciones en Diferencias)



Nota: No abarca sistemas de ecuaciones diferenciales parciales ni sistemas de ecuaciones diferenciales parciales estocasticas. Nos limitamos unicamente a sistemas donde la unica variable es el tiempo.