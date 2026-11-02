#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Bloque {
    int inicio;
    int tamano;
    bool libre;
    string proceso;
};

void limpiarEntrada() {
    cin.clear();
    string resto;
    getline(cin, resto);
}

int pedirNumero(const string& mensaje, int minimo) {
    int numero;

    while (true) {
        cout << mensaje;
        if (cin >> numero) {
            limpiarEntrada();
            if (numero >= minimo) {
                return numero;
            }
        } else {
            limpiarEntrada();
        }

        cout << "Entrada no valida. Intente nuevamente.\n";
    }
}

int pedirNumeroEntre(const string& mensaje, int minimo, int maximo) {
    int numero;

    while (true) {
        cout << mensaje;
        if (cin >> numero) {
            limpiarEntrada();
            if (numero >= minimo && numero <= maximo) {
                return numero;
            }
        } else {
            limpiarEntrada();
        }

        cout << "Entrada no valida. Intente nuevamente.\n";
    }
}

string pedirTexto(const string& mensaje) {
    string texto;

    while (true) {
        cout << mensaje;
        if (cin >> texto) {
            limpiarEntrada();
            return texto;
        }

        limpiarEntrada();
        cout << "Entrada no valida. Intente nuevamente.\n";
    }
}

void pausar() {
    cout << "\nPresione Enter para continuar...";
    string linea;
    getline(cin, linea);
}

void mostrarTitulo(const string& titulo) {
    cout << "\n========================================\n";
    cout << "          " << titulo << "\n";
    cout << "========================================\n";
}

bool existeProceso(const vector<Bloque>& memoria, const string& nombre) {
    int cantidadBloques = memoria.size();
    for (int i = 0; i < cantidadBloques; i++) {
        if (!memoria[i].libre && memoria[i].proceso == nombre) {
            return true;
        }
    }

    return false;
}

// Algoritmo First-Fit
bool asignarProceso(vector<Bloque>& memoria, const string& nombre, int tamano) {
    int cantidadBloques = memoria.size();
    for (int i = 0; i < cantidadBloques; i++) {
        if (memoria[i].libre && memoria[i].tamano >= tamano) {
            int espacioRestante = memoria[i].tamano - tamano;

            memoria[i].tamano = tamano;
            memoria[i].libre = false;
            memoria[i].proceso = nombre;

            if (espacioRestante > 0) {
                Bloque nuevoBloque;
                nuevoBloque.inicio = memoria[i].inicio + tamano;
                nuevoBloque.tamano = espacioRestante;
                nuevoBloque.libre = true;
                nuevoBloque.proceso = "";
                memoria.insert(memoria.begin() + i + 1, nuevoBloque);
            }

            return true;
        }
    }

    return false;
}

// Unir bloques libres
void unirBloques(vector<Bloque>& memoria) {
    int i = 0;
    int cantidadBloques = memoria.size();

    while (i + 1 < cantidadBloques) {
        if (memoria[i].libre && memoria[i + 1].libre) {
            memoria[i].tamano += memoria[i + 1].tamano;
            memoria.erase(memoria.begin() + i + 1);
            cantidadBloques--;
        } else {
            i++;
        }
    }
}

// Liberar memoria
bool liberarProceso(vector<Bloque>& memoria, const string& nombre) {
    int cantidadBloques = memoria.size();
    for (int i = 0; i < cantidadBloques; i++) {
        if (!memoria[i].libre && memoria[i].proceso == nombre) {
            memoria[i].libre = true;
            memoria[i].proceso = "";
            unirBloques(memoria);
            return true;
        }
    }

    return false;
}

int calcularEspacioLibre(const vector<Bloque>& memoria) {
    int espacioLibre = 0;
    int cantidadBloques = memoria.size();

    for (int i = 0; i < cantidadBloques; i++) {
        if (memoria[i].libre) {
            espacioLibre += memoria[i].tamano;
        }
    }

    return espacioLibre;
}

// Mostrar bloques
void mostrarMemoria(const vector<Bloque>& memoria) {
    int espacioLibre = 0;
    int mayorHueco = 0;
    int cantidadHuecos = 0;
    int cantidadBloques = memoria.size();

    cout << "\nBloque\tInicio\tFin\tTamano\tEstado\tProceso\n";
    cout << "--------------------------------------------------------\n";

    for (int i = 0; i < cantidadBloques; i++) {
        int fin = memoria[i].inicio + memoria[i].tamano - 1;
        string estado = "Ocupado";
        string proceso = memoria[i].proceso;

        if (memoria[i].libre) {
            estado = "Libre";
            proceso = "-";
            espacioLibre += memoria[i].tamano;
            cantidadHuecos++;

            if (memoria[i].tamano > mayorHueco) {
                mayorHueco = memoria[i].tamano;
            }
        }

        cout << i << '\t'
             << memoria[i].inicio << '\t'
             << fin << '\t'
             << memoria[i].tamano << '\t'
             << estado << '\t'
             << proceso << '\n';
    }

    cout << "\nMemoria libre: " << espacioLibre << " MiB\n";
    if (cantidadHuecos > 1) {
        cout << "Hay " << cantidadHuecos << " bloques libres separados.\n";
        cout << "Mayor bloque libre: " << mayorHueco << " MiB\n";
    }
}

void menuFirstFit() {
    mostrarTitulo("PARTICIONAMIENTO FIRST-FIT");
    int memoriaTotal = pedirNumero("Memoria total (MiB): ", 1);

    vector<Bloque> memoria;
    Bloque bloqueInicial = {0, memoriaTotal, true, ""};
    memoria.push_back(bloqueInicial);

    while (true) {
        mostrarTitulo("PARTICIONAMIENTO FIRST-FIT");
        mostrarMemoria(memoria);

        cout << "\n1. Asignar proceso\n";
        cout << "2. Liberar proceso\n";
        cout << "3. Mostrar memoria\n";
        cout << "4. Reiniciar memoria\n";
        cout << "0. Volver\n\n";

        int opcion = pedirNumeroEntre("Seleccione una opcion: ", 0, 4);

        if (opcion == 0) {
            return;
        }

        if (opcion == 1) {
            string nombre = pedirTexto("Nombre del proceso: ");

            if (existeProceso(memoria, nombre)) {
                cout << "Ya existe un proceso con ese nombre.\n";
            } else {
                int tamano = pedirNumero("Tamano del proceso (MiB): ", 1);

                if (asignarProceso(memoria, nombre, tamano)) {
                    cout << "Proceso asignado correctamente.\n";
                } else {
                    int espacioLibre = calcularEspacioLibre(memoria);

                    if (espacioLibre >= tamano) {
                        cout << "No hay un bloque contiguo suficiente.\n";
                        cout << "Fragmentacion externa: hay " << espacioLibre
                             << " MiB libres, pero estan separados.\n";
                    } else {
                        cout << "No hay memoria suficiente.\n";
                    }
                }
            }
            pausar();
        } else if (opcion == 2) {
            string nombre = pedirTexto("Nombre del proceso a liberar: ");

            if (liberarProceso(memoria, nombre)) {
                cout << "Proceso liberado correctamente.\n";
            } else {
                cout << "El proceso no existe.\n";
            }
            pausar();
        } else if (opcion == 3) {
            mostrarMemoria(memoria);
            pausar();
        } else if (opcion == 4) {
            int confirmar = pedirNumeroEntre(
                "Confirmar reinicio (1 = si, 0 = no): ", 0, 1);

            if (confirmar == 1) {
                memoria.clear();
                memoria.push_back({0, memoriaTotal, true, ""});
                cout << "Memoria reiniciada.\n";
            } else {
                cout << "Reinicio cancelado.\n";
            }
            pausar();
        }
    }
}

void asignarPaginasAutomaticamente(vector<int>& tablaPaginas,
                                    int cantidadMarcos) {
    int paginasAsignadas = 0;
    int cantidadPaginas = tablaPaginas.size();

    for (int pagina = 0; pagina < cantidadPaginas; pagina++) {
        if (pagina < cantidadMarcos) {
            tablaPaginas[pagina] = pagina;
            paginasAsignadas++;
        }
    }

    if (paginasAsignadas == cantidadPaginas) {
        cout << "Todas las paginas fueron asignadas.\n";
    } else {
        cout << "No hay suficientes marcos. Algunas paginas no fueron asignadas.\n";
    }
}

void asignarPaginasManualmente(vector<int>& tablaPaginas,
                               int cantidadMarcos) {
    vector<bool> marcoOcupado(cantidadMarcos, false);
    int cantidadPaginas = tablaPaginas.size();

    for (int pagina = 0; pagina < cantidadPaginas; pagina++) {
        while (true) {
            string mensaje = "Marco para pagina " + to_string(pagina)
                             + " (-1 = sin marco): ";
            int marco = pedirNumeroEntre(mensaje, -1, cantidadMarcos - 1);

            if (marco == -1) {
                break;
            }

            if (marcoOcupado[marco]) {
                cout << "Ese marco ya esta ocupado.\n";
            } else {
                tablaPaginas[pagina] = marco;
                marcoOcupado[marco] = true;
                break;
            }
        }
    }
}

int calcularFragmentacion(int tamanoProceso, int tamanoPagina) {
    int usados = tamanoProceso % tamanoPagina;

    if (usados == 0) {
        return 0;
    }

    return tamanoPagina - usados;
}

// Tabla de paginas
void mostrarPaginacion(const vector<int>& tablaPaginas,
                       int cantidadMarcos,
                       int tamanoPagina,
                       int tamanoProceso) {
    int marcosAsignados = 0;
    int fragmentacion = calcularFragmentacion(tamanoProceso, tamanoPagina);
    int cantidadPaginas = tablaPaginas.size();

    for (int pagina = 0; pagina < cantidadPaginas; pagina++) {
        if (tablaPaginas[pagina] != -1) {
            marcosAsignados++;
        }
    }

    long long capacidadPaginas = tamanoProceso;
    capacidadPaginas += fragmentacion;
    long long memoriaReservada = marcosAsignados;
    memoriaReservada *= tamanoPagina;

    cout << "\nRESUMEN DE PAGINACION\n";
    cout << "Tamano del proceso: " << tamanoProceso << " bytes\n";
    cout << "Tamano de pagina: " << tamanoPagina << " bytes\n";
    cout << "Paginas necesarias: " << cantidadPaginas << '\n';
    cout << "Capacidad de las paginas: " << capacidadPaginas << " bytes\n";
    cout << "Fragmentacion interna: " << fragmentacion << " bytes\n";
    cout << "Memoria fisica reservada: " << memoriaReservada << " bytes\n";

    cout << "\nPagina\tMarco\tEstado\n";
    cout << "----------------------------\n";
    for (int pagina = 0; pagina < cantidadPaginas; pagina++) {
        cout << pagina << '\t';
        if (tablaPaginas[pagina] == -1) {
            cout << "-\tNo asignada\n";
        } else {
            cout << tablaPaginas[pagina] << "\tAsignada\n";
        }
    }

    cout << "\nMarco\tContenido\tEstado\n";
    cout << "--------------------------------\n";
    for (int marco = 0; marco < cantidadMarcos; marco++) {
        int paginaEncontrada = -1;

        for (int pagina = 0; pagina < cantidadPaginas; pagina++) {
            if (tablaPaginas[pagina] == marco) {
                paginaEncontrada = pagina;
                break;
            }
        }

        cout << marco << '\t';
        if (paginaEncontrada == -1) {
            cout << "-\t\tLibre\n";
        } else {
            cout << "Pagina " << paginaEncontrada << "\tOcupado\n";
        }
    }
}

// Traducir direccion
void traducirDireccion(const vector<int>& tablaPaginas,
                       int tamanoPagina,
                       int tamanoProceso) {
    int direccionLogica = pedirNumero(
        "Direccion logica (-1 = cancelar): ", -1);

    if (direccionLogica == -1) {
        cout << "Traduccion cancelada.\n";
        return;
    }

    if (direccionLogica >= tamanoProceso) {
        cout << "Direccion invalida. El rango valido es 0 a "
             << tamanoProceso - 1 << ".\n";
        return;
    }

    int pagina = direccionLogica / tamanoPagina;
    int desplazamiento = direccionLogica % tamanoPagina;
    int marco = tablaPaginas[pagina];

    if (marco == -1) {
        cout << "La pagina " << pagina << " no tiene un marco asignado.\n";
        return;
    }

    long long direccionFisica = marco;
    direccionFisica = direccionFisica * tamanoPagina + desplazamiento;

    cout << "Pagina: " << pagina << '\n';
    cout << "Desplazamiento: " << desplazamiento << '\n';
    cout << "Marco: " << marco << '\n';
    cout << "Direccion fisica: " << direccionFisica << '\n';
    cout << "Traduccion: " << direccionLogica << " -> "
         << direccionFisica << '\n';
}

void menuPaginacion() {
    while (true) {
        mostrarTitulo("PAGINACION");

        int tamanoPagina = pedirNumero("Tamano de pagina (bytes): ", 1);
        int cantidadMarcos = pedirNumero("Cantidad de marcos fisicos: ", 1);
        int tamanoProceso = pedirNumero("Tamano del proceso (bytes): ", 1);
        int cantidadPaginas = (tamanoProceso - 1) / tamanoPagina + 1;
        vector<int> tablaPaginas(cantidadPaginas, -1);

        cout << "El proceso necesita " << cantidadPaginas << " paginas.\n\n";
        cout << "1. Asignacion automatica\n";
        cout << "2. Asignacion manual\n\n";

        int metodo = pedirNumeroEntre("Seleccione una opcion: ", 1, 2);

        if (metodo == 1) {
            asignarPaginasAutomaticamente(tablaPaginas, cantidadMarcos);
        } else {
            asignarPaginasManualmente(tablaPaginas, cantidadMarcos);
        }

        mostrarPaginacion(tablaPaginas, cantidadMarcos,
                          tamanoPagina, tamanoProceso);

        while (true) {
            cout << "\n1. Traducir direccion logica\n";
            cout << "2. Mostrar tablas\n";
            cout << "3. Nueva simulacion\n";
            cout << "0. Volver\n\n";

            int opcion = pedirNumeroEntre("Seleccione una opcion: ", 0, 3);

            if (opcion == 0) {
                return;
            }

            if (opcion == 1) {
                traducirDireccion(tablaPaginas, tamanoPagina, tamanoProceso);
            } else if (opcion == 2) {
                mostrarPaginacion(tablaPaginas, cantidadMarcos,
                                  tamanoPagina, tamanoProceso);
            } else if (opcion == 3) {
                break;
            }
        }
    }
}

void menuMemoria() {
    while (true) {
        mostrarTitulo("GESTION DE MEMORIA");
        cout << "\n1. Particionamiento dinamico (First-Fit)\n";
        cout << "2. Paginacion\n";
        cout << "0. Salir\n\n";

        int opcion = pedirNumeroEntre("Seleccione una opcion: ", 0, 2);

        if (opcion == 0) {
            return;
        }

        if (opcion == 1) {
            menuFirstFit();
        } else if (opcion == 2) {
            menuPaginacion();
        }
    }
}

int main() {
    menuMemoria();
    return 0;
}
