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
    condition_variable cv_vacio;
    condition_variable cv_lleno;
    int totalProducir;
    int producidos;
    int consumidos;

public:
    BufferCompartido(size_t cap, int total) 
        : capacidad(cap), totalProducir(total), producidos(0), consumidos(0) {}

    void producir(int idProductor, int item) {
        unique_lock<mutex> lock(mtx);
        cv_vacio.wait(lock, [this]() { return buffer.size() < capacidad; });

        buffer.push(item);
        producidos++;
        cout << "[Productor " << idProductor << "] Produjo item " << item 
             << " | Ocupacion Buffer: " << buffer.size() << "/" << capacidad << "\n";

        cv_lleno.notify_one();
    }

    bool consumir(int idConsumidor, int& itemConsumido) {
        unique_lock<mutex> lock(mtx);
        cv_lleno.wait(lock, [this]() { 
            return !buffer.empty() || consumidos >= totalProducir; 
        });

        if (buffer.empty() && consumidos >= totalProducir) {
            return false;
        }

        itemConsumido = buffer.front();
        buffer.pop();
        consumidos++;
        cout << "   [Consumidor " << idConsumidor << "] Consumio item " << itemConsumido 
             << " | Ocupacion Buffer: " << buffer.size() << "/" << capacidad << "\n";

        cv_vacio.notify_one();
        return true;
    }

    int getConsumidos() {
        lock_guard<mutex> lock(mtx);
        return consumidos;
    }

    void notificarTodos() {
        lock_guard<mutex> lock(mtx);
        cv_lleno.notify_all();
    }
};

void ejecutarProductorConsumidor() {
    int capacidad, total;
    cout << "\nCapacidad del buffer (ej. 3): ";
    cin >> capacidad;
    cout << "Total de elementos a producir (ej. 10): ";
    cin >> total;

    BufferCompartido buffer(capacidad, total);

    vector<thread> productores;
    vector<thread> consumidores;

    int itemsPorProductor = total / 2;

    for (int i = 1; i <= 2; ++i) {
        productores.push_back(thread([&buffer, i, itemsPorProductor, total]() {
            int inicio = (i - 1) * itemsPorProductor + 1;
            int fin = (i == 2) ? total : i * itemsPorProductor;
            for (int val = inicio; val <= fin; ++val) {
                buffer.producir(i, val);
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }));
    }

    for (int j = 1; j <= 2; ++j) {
        consumidores.push_back(thread([&buffer, j]() {
            int item;
            while (buffer.consumir(j, item)) {
                this_thread::sleep_for(chrono::milliseconds(200));
            }
        }));
    }

    for (auto& p : productores) {
        if (p.joinable()) p.join();
    }

    buffer.notificarTodos();

    for (auto& c : consumidores) {
        if (c.joinable()) c.join();
    }

    cout << "\n----------------------------------------\n";
    cout << "Total items procesados: " << buffer.getConsumidos() << "/" << total << "\n";
    cout << "----------------------------------------\n";
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
