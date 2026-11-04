# MiniSO - Sistema Operativo Simulado en C++

Descripción general

MiniSO es una aplicación de consola desarrollada en C++ para entornos
Linux. Su objetivo es integrar y demostrar conceptos teóricos y
prácticos de los Sistemas Operativos mediante tres módulos:

-   Gestión de procesos: creación, ejecución y finalización de procesos
    mediante llamadas al sistema de Linux.
-   Gestión de hilos y concurrencia: implementación del problema
    Productor-Consumidor con un búfer compartido de capacidad limitada,
    utilizando POSIX Threads (pthread), mutex y variables de condición.
-   Gestión de memoria: simulación de técnicas de administración de
    memoria, como particionamiento dinámico mediante First-Fit y
    paginación con traducción de direcciones lógicas a físicas.

1. Requisitos del sistema

Para compilar y ejecutar la aplicación se requiere:

-   Sistema operativo: distribución basada en Linux. Se recomienda
    Ubuntu 20.04 LTS o superior.
-   Compilador: g++ (GCC) con soporte para C++17.
-   Bibliotecas: biblioteca POSIX Threads, utilizada mediante pthread.h.
    En la mayoría de distribuciones Linux está disponible junto con las
    herramientas de desarrollo.
-   Herramientas opcionales: git para control de versiones y utilidades
    de terminal como ps o pstree para observar procesos.

Nota: el módulo de procesos utiliza llamadas al sistema POSIX/Linux, por
lo que la aplicación está diseñada para ejecutarse en Linux. El módulo
de concurrencia utiliza las funciones POSIX Threads directamente, en
lugar de las clases std::thread, std::mutex y std::condition_variable.

2. Estructura del proyecto

El proyecto está organizado en módulos para separar las
responsabilidades de cada componente:

    MiniSO/
    ├── main.cpp
    ├── procesos.hpp
    ├── procesos.cpp
    ├── hilos_y_concurrencia.hpp
    ├── hilos_y_concurrencia.cpp
    ├── memoria.hpp
    └── memoria.cpp

Descripción de los archivos:

-   main.cpp: punto de entrada de la aplicación. Presenta el menú
    principal y permite acceder a los módulos.
-   procesos.hpp / procesos.cpp: contienen la implementación del módulo
    de gestión de procesos. Según la implementación del proyecto, se
    utilizan llamadas al sistema como fork(), execvp() y waitpid() para
    crear y coordinar procesos.
-   hilos_y_concurrencia.hpp / hilos_y_concurrencia.cpp: contienen el
    módulo de gestión de hilos y concurrencia. Implementan el problema
    Productor-Consumidor mediante la clase BufferCompartido, una cola
    queue<int>, hilos pthread_t, mutex pthread_mutex_t y variables de
    condición pthread_cond_t.
-   memoria.hpp / memoria.cpp: contienen las simulaciones de
    administración de memoria, incluyendo asignación contigua mediante
    First-Fit y paginación, de acuerdo con las funcionalidades
    implementadas en el proyecto.

3. Instrucciones de compilación

1.  Abre una terminal de Linux.
2.  Ingresa al directorio raíz del proyecto, donde se encuentran los
    archivos .cpp y .hpp.
3.  Ejecuta el siguiente comando:

    g++ -std=c++17 -Wall main.cpp procesos.cpp hilos_y_concurrencia.cpp memoria.cpp -o miniSO -pthread

Explicación de las opciones:

-   -std=c++17: establece el estándar C++17.
-   -Wall: activa un conjunto amplio de advertencias del compilador.
-   -o miniSO: establece miniSO como nombre del ejecutable generado.
-   -pthread: habilita las opciones de compilación y enlazado necesarias
    para utilizar POSIX Threads.

Si los archivos fuente tienen nombres o ubicaciones diferentes, adapta
el comando para que coincida con la estructura real del proyecto.

4. Utilización de la aplicación

Una vez compilado el proyecto correctamente, ejecuta:

    ./miniSO

Se mostrará el menú principal. Selecciona la opción correspondiente al
módulo que deseas utilizar o la opción de salida para cerrar la
aplicación.

Módulo I: Gestión de procesos

Ingresa al módulo y ejecuta las opciones disponibles para observar la
creación y finalización de procesos. Si la demostración mantiene los
procesos activos el tiempo suficiente, puedes abrir otra terminal y
utilizar herramientas como pstree -p o ps para observarlos. Para
consultar un proceso específico, reemplaza <PID> por su identificador
real.

Módulo II: Gestión de hilos y concurrencia

Selecciona la opción de Productor-Consumidor. El programa solicitará los
siguientes datos:

-   Capacidad del búfer: cantidad máxima de elementos que puede
    almacenar la cola.
-   Total de elementos: cantidad de elementos que se distribuirá entre
    los productores.
-   Cantidad de productores: número de hilos encargados de generar
    elementos.
-   Cantidad de consumidores: número de hilos encargados de retirar
    elementos.

Se requiere un mínimo de dos productores y dos consumidores, y todos los
valores deben ser positivos.

Durante la ejecución se mostrarán mensajes de producción, consumo,
espera y finalización. Si el búfer está lleno, los productores esperan
hasta que un consumidor retire un elemento. Si está vacío, los
consumidores esperan hasta que un productor agregue uno.

La sincronización se realiza mediante:

-   pthread_create(): crea los hilos productores y consumidores.
-   pthread_join(): espera a que los hilos terminen.
-   pthread_mutex_init(), pthread_mutex_lock() y pthread_mutex_unlock():
    inicializan y controlan el mutex que protege el búfer y sus
    contadores.
-   pthread_cond_init(), pthread_cond_wait() y pthread_cond_signal():
    coordinan las esperas y notificaciones entre productores y
    consumidores.
-   pthread_cond_broadcast(): despierta a todos los consumidores que
    esperan cuando finaliza el último productor.
-   pthread_cond_destroy() y pthread_mutex_destroy(): liberan los
    recursos de sincronización al finalizar el objeto BufferCompartido.

Al terminar, el programa presenta el total de elementos producidos y
consumidos. Para que la prueba sea satisfactoria, ambas cantidades deben
coincidir con el total solicitado.

Se recomienda realizar pruebas cambiando la capacidad del búfer, el
número de elementos y la cantidad de productores y consumidores. Una
capacidad pequeña permite observar con mayor facilidad las esperas de
los productores cuando el búfer se llena.

Módulo III: Gestión de memoria

Ingresa al módulo y utiliza las opciones disponibles:

-   Particionamiento dinámico: asigna memoria a procesos simulados,
    libera bloques y observa el comportamiento del algoritmo First-Fit y
    la fragmentación externa, si estas operaciones están contempladas en
    la implementación.
-   Paginación: configura los marcos y las páginas e introduce
    direcciones lógicas para observar su descomposición y la traducción
    a direcciones físicas.

5. Consideraciones

-   Ejecuta la aplicación desde una terminal Linux.
-   Compila todos los archivos fuente necesarios y conserva sus nombres
    correctos en el comando de compilación.
-   En el módulo de concurrencia, utiliza al menos dos productores y dos
    consumidores.
-   Los mensajes de consola permiten observar la actividad concurrente,
    pero el orden de aparición puede variar entre ejecuciones debido a
    la planificación de los hilos.
-   La opción -pthread debe incluirse al compilar y enlazar el proyecto.
