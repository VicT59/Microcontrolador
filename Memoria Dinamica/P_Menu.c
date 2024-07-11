#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de la estructura para almacenar datos de alumno
typedef struct {
    char nombre[50];
    int edad;
    int nota;
} ESTRUCTURA;

// Prototipos de funciones
void grabarDato(void);
void leerDisco(void);
void salir(void);
void mostrarMensajeFinal(void);

int main() {
    system("color 70"); // Cambiar color de la consola en Windows

    int opcion;
    do {
        // Menú principal
        printf("\t*********Menu:**********\n");
        printf("1-Grabar Dato\n");
        printf("2-Leer Disco\n");
        printf("3-Salir\n");
        printf("~~OPCION~~: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                grabarDato();
                system("cls"); // Limpiar pantalla en Windows
                break;
            case 2:
                leerDisco();
                system("pause"); // Pausar ejecución en Windows
                system("cls");
                break;
            case 3:
                salir();
                break;
            default:
                printf("Opcion no valida.\n");
                system("pause");
                system("cls");
        }
    } while(opcion != 3);

    mostrarMensajeFinal(); // Mensaje de despedida y confirmación
    return 0;
}

void grabarDato(void) {
    FILE *archivo = fopen("alumno.dat", "ab"); // Abrir archivo para escritura binaria

    if (!archivo) {
        perror("Error al abrir el archivo");
        return;
    }

    ESTRUCTURA *alumno;
    alumno = (ESTRUCTURA*) malloc(sizeof(ESTRUCTURA)); // Asignar memoria dinámica para estructura

    if (!alumno) {
        perror("Error al asignar memoria");
        return;
    }

    char otro = 's';
    while (otro == 's') {
        // Capturar datos del usuario
        printf("Ingrese el nombre del alumno: ");
        scanf("%s", alumno->nombre);
        fflush(stdin);
        printf("Ingrese la edad del alumno: ");
        scanf("%d", &alumno->edad);
        fflush(stdin);
        printf("Ingrese la nota del alumno: ");
        scanf("%d", &alumno->nota);
        fflush(stdin);

        // Escribir estructura en el archivo
        fwrite(alumno, sizeof(ESTRUCTURA), 1, archivo);

        printf("Desea ingresar otro registro? (s/n): ");
        scanf(" %c", &otro);
    }

    free(alumno); // Liberar memoria asignada dinámicamente
    fclose(archivo); // Cerrar archivo
}

void leerDisco(void) {
    FILE *archivo = fopen("alumno.dat", "rb"); // Abrir archivo para lectura binaria

    if (!archivo) {
        perror("Error al abrir el archivo");
        return;
    }

    ESTRUCTURA *alumno;
    alumno = (ESTRUCTURA*) malloc(sizeof(ESTRUCTURA)); // Asignar memoria dinámica para estructura

    if (!alumno) {
        perror("Error al asignar memoria");
        return;
    }

    while (fread(alumno, sizeof(ESTRUCTURA), 1, archivo)) {
        // Mostrar datos leídos del archivo
        printf("Nombre: %s, Edad: %d, Nota: %d\n", alumno->nombre, alumno->edad, alumno->nota);
    }

    free(alumno); // Liberar memoria
    fclose(archivo); // Cerrar archivo
}

void salir(void) {
    system("cls"); // Limpiar pantalla en Windows
    printf("\n\nSALIENDO DEL PROGRAMA.\n\n");
}

void mostrarMensajeFinal(void) {
    printf("Gracias por usar el programa.\n");
    printf("Presione cualquier tecla para salir...");
    getchar(); // Esperar a que el usuario presione una tecla
}