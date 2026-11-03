#include "procesos.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>

using namespace std;

void ejecutarProcesosConcurrentes() {
    cout << "\n========================================\n";
    cout << "  CREACION DE PROCESOS HIJOS (LINUX)   \n";
    cout << "========================================\n";
    cout << "[PADRE] PID del proceso principal: " << getpid() << "\n";
    cout << "[PADRE] Creando 2 procesos hijos concurrentes...\n\n";

    // Argumentos para programas externos
    char* args1[] = {(char*)"ls", (char*)"-l", NULL};
    char* args2[] = {(char*)"uptime", NULL};

    // Creacion del primer hijo
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("Error al ejecutar fork() para el Hijo 1");
        return;
    } 
    else if (pid1 == 0) {
        // Codigo del Hijo 1
        cout << "[HIJO 1] PID: " << getpid() << " | PPID (Padre): " << getppid() << "\n";
        cout << "[HIJO 1] Ejecutando comando 'ls -l' via execvp()...\n\n";
        if (execvp(args1[0], args1) < 0) {
            perror("Error en execvp (Hijo 1)");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // Creacion del segundo hijo desde el Padre
        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("Error al ejecutar fork() para el Hijo 2");
            return;
        } 
        else if (pid2 == 0) {
            // Codigo del Hijo 2
            cout << "[HIJO 2] PID: " << getpid() << " | PPID (Padre): " << getppid() << "\n";
            cout << "[HIJO 2] Ejecutando comando 'uptime' via execvp()...\n\n";
            if (execvp(args2[0], args2) < 0) {
                perror("Error en execvp (Hijo 2)");
                exit(EXIT_FAILURE);
            }
        } 
        else {
            // El Padre espera la finalizacion de ambos hijos
            int status1, status2;
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);

            cout << "\n----------------------------------------\n";
            cout << "[PADRE] Estado Hijo 1 (PID " << pid1 << "): ";
            if (WIFEXITED(status1)) {
                cout << "Finalizo con exito (Codigo: " << WEXITSTATUS(status1) << ")\n";
            } else {
                cout << "Finalizo con error.\n";
            }

            cout << "[PADRE] Estado Hijo 2 (PID " << pid2 << "): ";
            if (WIFEXITED(status2)) {
                cout << "Finalizo con exito (Codigo: " << WEXITSTATUS(status2) << ")\n";
            } else {
                cout << "Finalizo con error.\n";
            }
            cout << "----------------------------------------\n";
        }
    }
}

void menuProcesos() {
    int opcion;
    do {
        cout << "\n========================================\n";
        cout << "          GESTION DE PROCESOS           \n";
        cout << "========================================\n";
        cout << "1. Ejecutar demostracion de fork() y execvp()\n";
        cout << "0. Volver al menu principal\n\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> opcion)) {
            cin.clear();
            string resto;
            getline(cin, resto);
            continue;
        }

        if (opcion == 1) {
            ejecutarProcesosConcurrentes();
        }
    } while (opcion != 0);
}
