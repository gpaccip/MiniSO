#include "hilos_y_concurrencia.hpp"
#include <iostream>
#include <pthread.h>
#include <queue>
#include <vector>
#include <unistd.h>

using namespace std;

class BufferCompartido {
private:
    queue<int> buffer;
    size_t capacidad;
    pthread_mutex_t mtx;
    pthread_cond_t cv_espacio_disponible;
    pthread_cond_t cv_elemento_disponible;
    int producidos;
    int consumidos;
    int productores_activos;

public:
    BufferCompartido(size_t cap, int numProductores)
        : capacidad(cap), producidos(0), consumidos(0),
          productores_activos(numProductores) {
        pthread_mutex_init(&mtx, NULL);
        pthread_cond_init(&cv_espacio_disponible, NULL);
        pthread_cond_init(&cv_elemento_disponible, NULL);
    }

    ~BufferCompartido() {
        pthread_cond_destroy(&cv_espacio_disponible);
        pthread_cond_destroy(&cv_elemento_disponible);
        pthread_mutex_destroy(&mtx);
    }

    void producir(int idProductor, int item) {
        pthread_mutex_lock(&mtx);

        while (buffer.size() >= capacidad) {
            cout << "[Productor " << idProductor
                 << "] ESPERANDO: Buffer lleno ("
                 << buffer.size() << "/" << capacidad << ").\n";

            pthread_cond_wait(&cv_espacio_disponible, &mtx);
        }

        buffer.push(item);
        producidos++;

        cout << "[Productor " << idProductor
             << "] PRODUJO item " << item
             << " | Buffer: " << buffer.size()
             << "/" << capacidad << "\n";

        pthread_cond_signal(&cv_elemento_disponible);
        pthread_mutex_unlock(&mtx);
    }

    bool consumir(int idConsumidor, int& itemConsumido) {
        pthread_mutex_lock(&mtx);

        while (buffer.empty() && productores_activos > 0) {
            cout << "   [Consumidor " << idConsumidor
                 << "] ESPERANDO: Buffer vacio.\n";

            pthread_cond_wait(&cv_elemento_disponible, &mtx);
        }

        if (buffer.empty() && productores_activos == 0) {
            pthread_mutex_unlock(&mtx);
            return false;
        }

        itemConsumido = buffer.front();
        buffer.pop();
        consumidos++;

        cout << "   [Consumidor " << idConsumidor
             << "] CONSUMIO item " << itemConsumido
             << " | Buffer: " << buffer.size()
             << "/" << capacidad << "\n";

        pthread_cond_signal(&cv_espacio_disponible);
        pthread_mutex_unlock(&mtx);

        return true;
    }

    void finalizarProductor(int idProductor) {
        pthread_mutex_lock(&mtx);

        cout << "[Productor " << idProductor
             << "] FINALIZO su tarea.\n";

        productores_activos--;

        if (productores_activos == 0) {
            pthread_cond_broadcast(&cv_elemento_disponible);
        }

        pthread_mutex_unlock(&mtx);
    }

    int getProducidos() {
        pthread_mutex_lock(&mtx);
        int resultado = producidos;
        pthread_mutex_unlock(&mtx);
        return resultado;
    }

    int getConsumidos() {
        pthread_mutex_lock(&mtx);
        int resultado = consumidos;
        pthread_mutex_unlock(&mtx);
        return resultado;
    }
};

struct DatosProductor {
    BufferCompartido* buffer;
    int id;
    int cantidad;
    int primerItem;
};

struct DatosConsumidor {
    BufferCompartido* buffer;
    int id;
};

void* ejecutarProductor(void* arg) {
    DatosProductor* datos = (DatosProductor*)arg;

    for (int i = 0; i < datos->cantidad; i++) {
        int itemUnico = datos->primerItem + i;

        datos->buffer->producir(datos->id, itemUnico);

        usleep(50000);
    }

    datos->buffer->finalizarProductor(datos->id);

    return NULL;
}

void* ejecutarConsumidor(void* arg) {
    DatosConsumidor* datos = (DatosConsumidor*)arg;

    int item;

    while (datos->buffer->consumir(datos->id, item)) {
        usleep(80000);
    }

    cout << "   [Consumidor " << datos->id
         << "] FINALIZO su tarea.\n";

    return NULL;
}

void ejecutarProductorConsumidor() {
    int capacidad, totalItems, numProductores, numConsumidores;

    cout << "\n--- CONFIGURACION DEL ESCENARIO ---\n";
    cout << "Capacidad del buffer: ";
    cin >> capacidad;

    cout << "Total de elementos a producir: ";
    cin >> totalItems;

    cout << "Cantidad de hilos productores: ";
    cin >> numProductores;

    cout << "Cantidad de hilos consumidores: ";
    cin >> numConsumidores;

    if (numProductores < 2 || numConsumidores < 2 ||
        totalItems <= 0 || capacidad <= 0) {
        cout << "Error: Se requieren minimo 2 productores, "
             << "2 consumidores y valores positivos.\n";
        return;
    }

    BufferCompartido buffer(capacidad, numProductores);

    vector<pthread_t> productores(numProductores);
    vector<pthread_t> consumidores(numConsumidores);

    vector<DatosProductor> datosProductores(numProductores);
    vector<DatosConsumidor> datosConsumidores(numConsumidores);

    int itemsPorProductor = totalItems / numProductores;
    int itemsSobrantes = totalItems % numProductores;
    int siguienteItem = 1;

    cout << "\n--- INICIANDO EJECUCION CONCURRENTE ---\n";

    for (int j = 0; j < numConsumidores; j++) {
        datosConsumidores[j].buffer = &buffer;
        datosConsumidores[j].id = j + 1;

        pthread_create(&consumidores[j], NULL,
                       ejecutarConsumidor, &datosConsumidores[j]);
    }

    for (int i = 0; i < numProductores; i++) {
        datosProductores[i].buffer = &buffer;
        datosProductores[i].id = i + 1;
        datosProductores[i].cantidad = itemsPorProductor +
                                       (i < itemsSobrantes ? 1 : 0);
        datosProductores[i].primerItem = siguienteItem;

        siguienteItem += datosProductores[i].cantidad;

        pthread_create(&productores[i], NULL,
                       ejecutarProductor, &datosProductores[i]);
    }

    for (int i = 0; i < numProductores; i++) {
        pthread_join(productores[i], NULL);
    }

    for (int j = 0; j < numConsumidores; j++) {
        pthread_join(consumidores[j], NULL);
    }

    cout << "\n--- RESUMEN DE EJECUCION ---\n";
    cout << "Total items producidos: " << buffer.getProducidos() << "\n";
    cout << "Total items consumidos: " << buffer.getConsumidos() << "\n";

    if (buffer.getProducidos() == totalItems &&
        buffer.getConsumidos() == totalItems &&
        buffer.getProducidos() == buffer.getConsumidos()) {
        cout << "ESTADO: EXITO. Todos los elementos fueron "
             << "consumidos sin perdidas ni duplicaciones.\n";
    } else {
        cout << "ESTADO: ERROR en la sincronizacion.\n";
    }
}

void menuHilosYConcurrencia() {
    int opcion;

    do {
        cout << "\n========================================\n";
        cout << "     GESTION DE HILOS Y CONCURRENCIA    \n";
        cout << "========================================\n";
        cout << "1. Ejecutar Productor-Consumidor (Buffer Acotado)\n";
        cout << "0. Volver al menu principal\n\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            string resto;
            getline(cin, resto);
            continue;
        }

        if (opcion == 1) {
            ejecutarProductorConsumidor();
        }

    } while (opcion != 0);
}
