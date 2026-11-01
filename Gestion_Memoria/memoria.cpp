#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct Bloque {
    int tamano;
    bool libre;
    string proceso;
};

int leerEnteroPositivo(const string& mensaje) {
    int valor;

    while (true) {
        cout << mensaje;
        if (cin >> valor && valor > 0) {
            return valor;
        }

        cout << "Entrada invalida. Ingrese un numero entero mayor que cero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int leerOpcion() {
    int opcion;

    while (true) {
        cout << "Seleccione una opcion: ";
        if (cin >> opcion && opcion >= 1 && opcion <= 3) {
            return opcion;
        }

        cout << "Opcion invalida. Intente nuevamente.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void asignarProceso(vector<Bloque>& memoria) {
    string nombre;
    cout << "Nombre del proceso: ";
    cin >> nombre;

    int tamano = leerEnteroPositivo("Tamano del proceso: ");

    for (size_t i = 0; i < memoria.size(); ++i) {
        if (memoria[i].libre && memoria[i].tamano >= tamano) {
            int espacioRestante = memoria[i].tamano - tamano;

            memoria[i] = {tamano, false, nombre};
            if (espacioRestante > 0) {
                memoria.insert(memoria.begin() + i + 1,
                               {espacioRestante, true, ""});
            }

            cout << "Proceso " << nombre
                 << " asignado correctamente mediante First-Fit.\n";
            return;
        }
    }

    cout << "No existe espacio suficiente para asignar el proceso.\n";
}

void mostrarEstado(const vector<Bloque>& memoria) {
    int espacioLibre = 0;

    cout << "\nEstado de la memoria:\n";
    for (size_t i = 0; i < memoria.size(); ++i) {
        cout << "Bloque " << i + 1 << ": ";
        if (memoria[i].libre) {
            cout << "Libre - " << memoria[i].tamano << " unidades\n";
            espacioLibre += memoria[i].tamano;
        } else {
            cout << "Proceso " << memoria[i].proceso << " - "
                 << memoria[i].tamano << " unidades\n";
        }
    }

    cout << "Espacio libre total: " << espacioLibre << " unidades\n";
}

void mostrarMenu() {
    cout << "\nGestion de memoria - First-Fit\n";
    cout << "1. Asignar proceso\n";
    cout << "2. Ver estado de memoria\n";
    cout << "3. Salir\n";
}

int main() {
    int memoriaTotal = leerEnteroPositivo("Ingrese la memoria total: ");
    vector<Bloque> memoria = {{memoriaTotal, true, ""}};

    while (true) {
        mostrarMenu();
        int opcion = leerOpcion();

        if (opcion == 1) {
            asignarProceso(memoria);
        } else if (opcion == 2) {
            mostrarEstado(memoria);
        } else {
            cout << "Programa finalizado.\n";
            break;
        }
    }

    return 0;
}
