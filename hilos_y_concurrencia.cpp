#include "hilos_y_concurrencia.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>

using namespace std;

class BufferCompartido {
private:
    queue<int> buffer;
    size_t capacidad;
    mutex mtx;
    condition_variable cv_vacio; // Para que los productores esperen si esta lleno
    condition_variable cv_lleno; // Para que los consumidores esperen si esta vacio
    int producidos;
    int consumidos;
    bool productores_activos;

public:
    BufferCompartido(size_t cap) 
        : capacidad(cap), producidos(0), consumidos(0), productores_activos(true) {}

    void producir(int idProductor, int item) {
        unique_lock<mutex> lock(mtx);
        
        // Mensaje de espera si el buffer esta lleno (Requisito de la rubrica)
        if (buffer.size() >= capacidad) {
            cout << "[Productor " << idProductor << "] ESPERANDO: Buffer lleno (" << buffer.size() << "/" << capacidad << ").\n";
        }

        cv_vacio.wait(lock, [this]() { return buffer.size() < capacidad; });

        buffer.push(item);
        producidos++;
        cout << "[Productor " << idProductor << "] PRODUJO item " << item 
             << " | Buffer: " << buffer.size() << "/" << capacidad << "\n";

        // Notificar a los consumidores que hay un nuevo item
        cv_lleno.notify_one();
    }

    bool consumir(int idConsumidor, int& itemConsumido) {
        unique_lock<mutex> lock(mtx);
        
        // Mensaje de espera si el buffer esta vacio y aun hay productores
        if (buffer.empty() && productores_activos) {
            cout << "   [Consumidor " << idConsumidor << "] ESPERANDO: Buffer vacio.\n";
        }

        cv_lleno.wait(lock, [this]() { 
            return !buffer.empty() || !productores_activos; 
        });

        // Si el buffer esta vacio y los productores terminaron, salir
        if (buffer.empty() && !productores_activos) {
            return false;
        }

        itemConsumido = buffer.front();
        buffer.pop();
        consumidos++;
        cout << "   [Consumidor " << idConsumidor << "] CONSUMIO item " << itemConsumido 
             << " | Buffer: " << buffer.size() << "/" << capacidad << "\n";

        // Notificar a los productores que hay espacio
        cv_vacio.notify_one();
        return true;
    }

    void finalizarProductores() {
        lock_guard<mutex> lock(mtx);
        productores_activos = false;
        cv_lleno.notify_all(); // Despertar consumidores restantes para que terminen
    }

    int getProducidos() { return producidos; }
    int getConsumidos() { return consumidos; }
};

void ejecutarProductorConsumidor() {
    int capacidad, totalItems, numProductores, numConsumidores;
    
    cout << "\n--- CONFIGURACION DEL ESCENARIO ---\n";
    cout << "Capacidad del buffer (ej. 5): ";
    cin >> capacidad;
    cout << "Total de elementos a producir (ej. 20): ";
    cin >> totalItems;
    cout << "Cantidad de hilos productores (ej. 2): ";
    cin >> numProductores;
    cout << "Cantidad de hilos consumidores (ej. 2): ";
    cin >> numConsumidores;

    if (numProductores <= 0 || numConsumidores <= 0 || totalItems <= 0 || capacidad <= 0) {
        cout << "Error: Todos los valores deben ser mayores a 0.\n";
        return;
    }

    BufferCompartido buffer(capacidad);
    vector<thread> productores;
    vector<thread> consumidores;

    cout << "\n--- INICIANDO EJECUCION CONCURRENTE ---\n";

    // Crear Hilos Productores
    int itemsPorProductor = totalItems / numProductores;
    int itemsSobrantes = totalItems % numProductores;

    for (int i = 1; i <= numProductores; ++i) {
        productores.push_back(thread([&buffer, i, itemsPorProductor, itemsSobrantes, numProductores]() {
            int itemsAsignados = itemsPorProductor + (i == numProductores ? itemsSobrantes : 0);
            for (int val = 1; val <= itemsAsignados; ++val) {
                // El item tendra un ID unico basado en el productor y su iteracion
                int itemUnico = (i * 1000) + val; 
                buffer.producir(i, itemUnico);
                this_thread::sleep_for(chrono::milliseconds(50)); // Simular tiempo de produccion
            }
            cout << "[Productor " << i << "] FINALIZO su tarea.\n";
        }));
    }

    // Crear Hilos Consumidores
    for (int j = 1; j <= numConsumidores; ++j) {
        consumidores.push_back(thread([&buffer, j]() {
            int item;
            while (buffer.consumir(j, item)) {
                this_thread::sleep_for(chrono::milliseconds(80)); // Simular tiempo de consumo
            }
            cout << "   [Consumidor " << j << "] FINALIZO su tarea.\n";
        }));
    }

    // Sincronizacion: Esperar a que terminen los productores
    for (auto& p : productores) {
        if (p.joinable()) p.join();
    }

    // Indicar al buffer que ya no habra mas items
    buffer.finalizarProductores();

    // Sincronizacion: Esperar a que terminen los consumidores
    for (auto& c : consumidores) {
        if (c.joinable()) c.join();
    }

    cout << "\n--- RESUMEN DE EJECUCION ---\n";
    cout << "Total items producidos: " << buffer.getProducidos() << "\n";
    cout << "Total items consumidos: " << buffer.getConsumidos() << "\n";
    if (buffer.getProducidos() == buffer.getConsumidos() && buffer.getProducidos() == totalItems) {
        cout << "ESTADO: EXITO. Todos los elementos fueron consumidos sin perdidas.\n";
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
