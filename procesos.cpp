#include "procesos.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>

using namespace std;

// Demostración usando wait() genérico
void ejecucionConWaitGenerico() {
    cout << "\n========================================\n";
    cout << "     1. DEMOSTRACION CON wait()         \n";
    cout << "========================================\n";
    cout << "[PADRE] PID actual: " << getpid() << "\n";

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork()");
        return;
    } 
    else if (pid == 0) {
        // Código del Proceso Hijo
        cout << "[HIJO] PID: " << getpid() << " | PPID (Padre): " << getppid() << "\n";
        cout << "[HIJO] Ejecutando comando 'echo' via execvp()...\n";

        char arg0[] = "echo";
        char arg1[] = "-> Mensaje ejecutado desde el proceso hijo";
        char* args[] = {arg0, arg1, NULL};

        if (execvp(args[0], args) < 0) {
            perror("Error en execvp()");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // El Padre usa wait() para esperar a cualquier hijo que termine
        int status;
        pid_t hijo_finalizado = wait(&status);

        cout << "\n[PADRE] El hijo con PID " << hijo_finalizado << " ha finalizado.\n";
        if (WIFEXITED(status)) {
            cout << "[PADRE] Codigo de retorno del hijo: " << WEXITSTATUS(status) << "\n";
        }
    }
}

// Demostración usando waitpid() para hijos específicos
void ejecucionConWaitpidEspecifico() {
    cout << "\n========================================\n";
    cout << "   2. DEMOSTRACION CON waitpid()        \n";
    cout << "========================================\n";
    cout << "[PADRE] PID del proceso principal: " << getpid() << "\n";
    cout << "[PADRE] Creando 2 procesos hijos concurrentes...\n\n";

    char arg1_0[] = "ls";
    char arg1_1[] = "-l";
    char* args1[] = {arg1_0, arg1_1, NULL};

    char arg2_0[] = "uptime";
    char* args2[] = {arg2_0, NULL};

    // Creación del primer hijo
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("Error al ejecutar fork() para el Hijo 1");
        return;
    } 
    else if (pid1 == 0) {
        cout << "[HIJO 1] PID: " << getpid() << " | PPID (Padre): " << getppid() << "\n";
        cout << "[HIJO 1] Ejecutando comando 'ls -l' via execvp()...\n\n";
        if (execvp(args1[0], args1) < 0) {
            perror("Error en execvp (Hijo 1)");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // Creación del segundo hijo
        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("Error al ejecutar fork() para el Hijo 2");
            return;
        } 
        else if (pid2 == 0) {
            cout << "[HIJO 2] PID: " << getpid() << " | PPID (Padre): " << getppid() << "\n";
            cout << "[HIJO 2] Ejecutando comando 'uptime' via execvp()...\n\n";
            if (execvp(args2[0], args2) < 0) {
                perror("Error en execvp (Hijo 2)");
                exit(EXIT_FAILURE);
            }
        } 
        else {
            // El Padre espera explícitamente por PID con waitpid()
            int status1, status2;
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);

            cout << "\n----------------------------------------\n";
            cout << "[PADRE] Estado Hijo 1 (PID " << pid1 << "): ";
            if (WIFEXITED(status1)) {
                cout << "Finalizo con exito (Codigo: " << WEXITSTATUS(status1) << ")\n";
            }
            cout << "[PADRE] Estado Hijo 2 (PID " << pid2 << "): ";
            if (WIFEXITED(status2)) {
                cout << "Finalizo con exito (Codigo: " << WEXITSTATUS(status2) << ")\n";
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
        cout << "1. Proceso unico con wait()\n";
        cout << "2. Procesos concurrentes con waitpid()\n";
        cout << "0. Volver al menu principal\n\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> opcion)) {
            cin.clear();
            string resto;
            getline(cin, resto);
            continue;
        }

        if (opcion == 1) {
            ejecucionConWaitGenerico();
        } else if (opcion == 2) {
            ejecucionConWaitpidEspecifico();
        }
    } while (opcion != 0);
}