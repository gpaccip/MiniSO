#include <iostream>
#include <string>
#include "procesos.hpp"
#include "hilos_y_concurrencia.hpp"
#include "memoria.hpp"

using namespace std;

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    int opcion = -1;

    while (opcion != 0) {
        limpiarPantalla();
        cout << "======================================================\n";
        cout << "      SISTEMA OPERATIVO SIMULADO - MiniSO (2026)      \n";
        cout << "======================================================\n";
        cout << "1. Modulo I: Gestion de Procesos (Linux real)\n";
        cout << "2. Modulo II: Hilos y Concurrencia (pthread)\n";
        cout << "3. Modulo III: Gestion de Memoria (Simulacion)\n";
        cout << "0. Salir de MiniSO\n";
        cout << "======================================================\n";
        cout << "Seleccione una opcion [0-3]: ";

        if (!(cin >> opcion)) {
            cin.clear();
            string descarte;
            getline(cin, descarte);
            cout << "\nEntrada no valida. Presione Enter para reintentar...";
            cin.get();
            continue;
        }

        switch (opcion) {
            case 1:
                limpiarPantalla();
                menuProcesos();
                break;
            case 2:
                limpiarPantalla();
                menuHilosYConcurrencia();
                break;
            case 3:
                limpiarPantalla();
                menuMemoria();
                break;
            case 0:
                cout << "\nGracias por usar MiniSO. Cerrando aplicacion...\n";
                break;
            default:
                cout << "\nOpcion fuera de rango. Intente nuevamente.\n";
                break;
        }
    }

    return 0;
}
