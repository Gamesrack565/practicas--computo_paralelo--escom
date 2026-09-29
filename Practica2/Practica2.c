#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

//DEfinidmos la cantidad de filas y columnas de la matriz
#define filas 3
#define columnas 9

int main() {
    //Creamos la matriz de 3x9 que se nos da en la practica
    int matriz[filas][columnas] = {
        {1, 2, 3, 4, 5, 6, 7, 8, 9},
        {1, 3, 5, 7, 9, 11, 13, 15, 17},
        {2, 4, 6, 8, 10, 12, 14, 16, 18}
    };

    //Se crea un archivo de texto llamado "matriz.txt" y se abre en modo escritura
    // Agregamos getpid() para saber el identificador del proceso Padre
    printf("---> [Padre PID: %d] Ejecución de proceso padre iniciada\n", getpid());
    
    FILE *archivo = fopen("matriz.txt", "w");
    //En caso de que el archivo tenga un error al abrirse, se imprime un mensaje de error y se retorna 1
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    //Se recorre la matriz y se escribe cada elemento en el archivo, 
    //separando los elementos de cada fila con un espacio y agregando un salto de línea al final de cada fila
    for (int i=0; i < filas; i++) {
        for (int j=0; j < columnas; j++) {
            fprintf(archivo, "%d ", matriz[i][j]);
        }
        fprintf(archivo, "\n");
    }
    fclose(archivo);
    
    //inicio de procesos hijo
    printf("---> [Padre PID: %d] Creación de procesos hijos...\n", getpid());
    pid_t pid;
    
    //Ciclo que recorre las columnas de la matriz y crea un proceso hijo para cada columna
    for(int colu = 0; colu < columnas; colu++) {
        pid = fork();

        //Se verifica si el proceso hijo se creo correctamente
        if (pid == -1){
            printf("---> Error al crear el proceso hijo\n");
            //Perror es una llamada al sistema que imprime un mensaje de error 
            //en caso de que la creación del proceso hijo falle

            //Lo que si deferencia con un print normal, es que perror agrega informacion del error
            perror("Error al crear el proceso hijo");
            exit(1);
        }

        if (pid == 0) {
            //Proceso hijo
            // Usamos \t (tabulación) para que visualmente en la consola los mensajes de los hijos se vean indentados
            printf("---> \t[Hijo PID: %d] Iniciando proceso hijo. Me toca la columna %d\n", getpid(), colu + 1);
            
            FILE *archivo_cargado = fopen("matriz.txt", "r");
            int valor, indice = 0;
            long multip = 1;
            //Se recorre el archivo "matriz.txt" y se multiplica cada elemento de la columna correspondiente
            while (fscanf(archivo_cargado, "%d", &valor) == 1) {
                if (indice % columnas == colu) {
                    multip *= valor;
                }
                indice++;
            }
            fclose(archivo_cargado);

            //Se abre el archivo "resultados.txt" en modo append para agregar los resultados de cada hijo
            FILE *archivo_resultado = fopen("resultados.txt", "a");
            fprintf(archivo_resultado, "El producto de la columna %d es: %ld\n", colu + 1, multip);
            fclose(archivo_resultado);

            printf("---> \t[Hijo PID: %d] Finalizando tarea de la columna %d\n", getpid(), colu + 1);
            exit(0);
        }
    }

    //El padre hace 9 pausas, una por cada hijo que creó
    printf("---> [Padre PID: %d] Esperando a que todos los hijos terminen...\n", getpid());
    for (int i = 0; i < columnas; i++) {
        wait(NULL);
    }

    printf("\n---> [Padre PID: %d] Resultados calculados por los procesos hijos:\n", getpid());
    printf("----------------------------------------------------------\n");
    //Se abre el archivo "resultados.txt" en modo lectura para mostrar los resultados de cada hijo
    FILE *f_res = fopen("resultados.txt", "r");
    if (f_res != NULL) {
        char c;
        while ((c = fgetc(f_res)) != EOF) {
            putchar(c);
        }
        fclose(f_res);
    } else {
        //En caso de que el archivo no se pueda leer por alguna razón
        perror("Error al intentar leer resultados.txt");
    }

    return 0;
}