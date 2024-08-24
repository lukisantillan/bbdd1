#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char nada[200];

typedef struct
{
    int codigoCliente;
    char nombre[20];
    char apellido[20];
    int estado; // 1 activo , 0 borrado;
    int siguiente;
} t_clientes;

void crearArchivo(char name[]);
void altaCliente(char name[], int posicion);
bool bajaFisicaCliente(char name[], int pos);
void limpiarBuffer();
int contarRegistros(char name[]);
void leerArchivoEntero(char name[]);
void modificacionDeRegistro(char name[], int pos);
void leerRegistro(char name[], int n);
int menuModificacion();
void convertirAMinusculas(char cadena[]);

int main()
{
    char nombreArchivo[30] = "";
    int flag = 1;
    while (flag == 1)
    {
        int operacion;
        printf("\n\t\t\t\t- MENU PARA EL PT1 - ");
        printf("\t\t\n ELEGIR UNA OPCION");
        printf("\t\t\n 1- CREAR ARCHIVO");
        printf("\t\t\n 2- LEER ARCHIVO");
        printf("\t\t\n 3- MODIFICAR ARCHIVO");
        printf("\n");
        scanf("%d", &operacion);
        while (operacion > 3 || operacion < 1)
        {
            printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
            printf("\t\t\n 1- CREAR ARCHIVO");
            printf("\t\t\n 2- LEER ARCHIVO");
            printf("\t\t\n 3- MODIFICAR ARCHIVO");
            printf("\n");
            scanf("%d", &operacion);
        }
        limpiarBuffer();
        FILE *arch;
        if (nombreArchivo[0] == '\0')
        {
            printf("\n------------------------------");
            printf("\nINGRESE EL NOMBRE DEL ARCHIVO, RECUERDE QUE DEBE SER .bin:  ");
            fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
            nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';
            convertirAMinusculas(nombreArchivo);
            const char *extension = ".bin";
            size_t lenArchivo = strlen(nombreArchivo);
            size_t lenExt = strlen(extension);
            // Verificar si la extensión está al final del nombre del archivo
            while (strcmp(nombreArchivo + lenArchivo - lenExt, extension) != 0)
            {
                printf("\n\t\t\tEL ARCHIVO NO TERMINA EN .bin, REEINGRESE EL NOMBRE: ");
                fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
                nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';
                lenArchivo = strlen(nombreArchivo);
            }
        }
        switch (operacion)
        {
        case 1:
            arch = fopen(nombreArchivo, "rb");
            if (arch != NULL)
            {
                printf("\nYA EXISTE UN ARCHIVO CON ESTE NOMBRE, INGRESE OTRO NOMBRE O SELECCIONE LA OPCIÓN DE LEER EL ARCHIVO\n");
                fclose(arch);
                break;
            }
            fclose(arch);
            crearArchivo(nombreArchivo);
            break;
        case 2:
            printf("\n------------------------------");
            leerArchivoEntero(nombreArchivo);
            break;
        case 3:
            printf("\n------------------------------");
            int opcion;
            printf("\n\t\t\t\t- MENU PARA MODIFICAR EL ARCHIVO - ");
            printf("\t\t\n ELEGIR UNA OPCION");
            printf("\t\t\n 1- ALTA DE CLIENTE");
            printf("\t\t\n 2- BAJA DE CLIENTE");
            printf("\t\t\n 3- MODIFICAR CLIENTE");
            printf("\n");
            scanf("%d", &opcion);
            while (opcion > 3 || opcion < 1)
            {
                printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
                printf("\t\t\n 1- ALTA DE CLIENTE");
                printf("\t\t\n 2- BAJA DE CLIENTE");
                printf("\t\t\n 3- MODIFICAR CLIENTE");
                printf("\n");
                scanf("%d", &opcion);
            }
            limpiarBuffer();
            int pos;
            switch (opcion)
            {
            case 1:
                printf("\nINGRESE LA POSICION FISICA EN DONDE QUIERE HACER EL ALTA DEL CLIENTE:   ");
                scanf("%d", &pos);
                limpiarBuffer();
                altaCliente(nombreArchivo, pos);
                break;
            case 2:
                printf("\nINGRESE LA POSICION FISICA EN DONDE QUIERE HACER LA BAJA DEL CLIENTE:   ");
                scanf("%d", &pos);
                limpiarBuffer();
                bajaFisicaCliente(nombreArchivo, pos);
                break;
            case 3:
                printf("\nINGRESE LA POSICION FISICA EN DONDE QUIERE HACER LA MODIFICACION DEL CLIENTE:   ");
                scanf("%d", &pos);
                limpiarBuffer();
                modificacionDeRegistro(nombreArchivo, pos);
            default:
                break;
            }
            break;
        default:
            break;
        }
        printf("\n------------------------------");
        printf("\n\t\t- DESEA REALIZAR OTRA OPERACION?(1(SI)-0(NO)): - ");
        scanf("%d", &flag);
        while (flag > 1 || flag < 0)
        {
            printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
            printf("\n\t\t- DESEA REALIZAR OTRA OPERACION?(1(SI)-0(NO)): - ");
            printf("\n");
            scanf("%d", &flag);
        }
        int flag2 = 1;
        printf("\n\t\t- DESEA QUE SEA SOBRE EL MISMO ARCHIVO(1(SI)-0(NO)): - ");
        scanf("%d", &flag2);
        while (flag2 > 1 || flag2 < 0)
        {
            printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
            printf("\n\t\t- DESEA QUE SEA SOBRE EL MISMO ARCHIVO(0(SI)-1(NO)): - ");
            printf("\n");
            scanf("%d", &flag2);
        }
        limpiarBuffer();
        if (flag2 == 0)
        {
            printf("\n------------------------------");
            printf("\nINGRESE EL NOMBRE DEL NUEVO ARCHIVO A REALIZAR OPERACIONES, RECUERDE QUE DEBE SER .bin:  ");
            fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
            nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';
            convertirAMinusculas(nombreArchivo);
            const char *extension = ".bin";
            size_t lenArchivo = strlen(nombreArchivo);
            size_t lenExt = strlen(extension);
            // Verificar si la extensión está al final del nombre del archivo
            while (strcmp(nombreArchivo + lenArchivo - lenExt, extension) != 0)
            {
                printf("\n\t\t\tEL ARCHIVO NO TERMINA EN .bin, REEINGRESE EL NOMBRE: ");
                fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
                nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';
                lenArchivo = strlen(nombreArchivo);
            }
        }
    }

    return 0;
}

void limpiarBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void crearArchivo(char name[])
{
    char *modo = "wb";
    FILE *file;
    file = fopen(name, modo);
    fclose(file);
}

void altaCliente(char name[], int pos)
{
    FILE *arch, *temp;
    t_clientes nuevoCliente;
    int contador = 0;
    int registrosTotales;
    // Solicitar información del nuevo cliente
    printf("\n Ingrese el Codigo del Client(Debe contener 4 digitos):   ");
    scanf("%i", &nuevoCliente.codigoCliente);
    while (nuevoCliente.codigoCliente > 9999 || nuevoCliente.codigoCliente < 1000)
    {
        printf("\n Ingrese un Codigo del Cliente valido, este debe contener 4 digitos:   ");
        scanf("%i", &nuevoCliente.codigoCliente);
    }
    limpiarBuffer(); // Limpiar el búfer después de scanf

    printf("\n Ingrese el nombre:   ");
    fgets(nuevoCliente.nombre, sizeof(nuevoCliente.nombre), stdin);
    nuevoCliente.nombre[strcspn(nuevoCliente.nombre, "\n")] = '\0'; // Sacar salto de línea

    printf("\n Ingrese el apellido:   ");
    fgets(nuevoCliente.apellido, sizeof(nuevoCliente.apellido), stdin);
    nuevoCliente.apellido[strcspn(nuevoCliente.apellido, "\n")] = '\0'; // Sacar salto de línea
    nuevoCliente.estado = 1;
    // Contar el número total de registros en el archivo
    registrosTotales = contarRegistros(name);

    // Verificar si la posición es válida
    if (pos < 0)
    {
        printf("Posición inválida.\n");
        return;
    }

    if (registrosTotales == 0 || pos > registrosTotales)
    {
        // Si el archivo no existe o está vacío, lo creamos y agregamos el primer registro
        arch = fopen(name, "ab");
        if (arch == NULL)
        {
            printf("Error al crear el archivo.\n");
            return;
        }
        fwrite(&nuevoCliente, sizeof(t_clientes), 1, arch);
        printf("\nEl archivo estaba vacio o la posicion era mayor a la cantidad de registros");
        printf("\nEl cliente se agrego al final del archivo");
        fclose(arch);
    }
    else
    {
        // Abrimos el archivo original en modo lectura y el archivo temporal en modo escritura
        arch = fopen(name, "rb");
        if (arch == NULL)
        {
            printf("Error al abrir el archivo original.\n");
            return;
        }

        temp = fopen("temp.bin", "ab");
        if (temp == NULL)
        {
            printf("Error al abrir el archivo temporal.\n");
            fclose(arch);
            return;
        }

        t_clientes clienteAux;
        // Copiamos los registros del archivo original al temporal
        while (fread(&clienteAux, sizeof(t_clientes), 1, arch))
        {
            if (contador == pos)
            {
                // Insertamos el nuevo cliente en la posición especificada
                fwrite(&nuevoCliente, sizeof(t_clientes), 1, temp);
            }
            // Copiamos el siguiente registro del archivo original al temporal
            fwrite(&clienteAux, sizeof(t_clientes), 1, temp);
            contador++;
        }

        // Si la posición es al final, agregamos el nuevo cliente al final
        if (pos == registrosTotales)
        {
            fwrite(&nuevoCliente, sizeof(t_clientes), 1, temp);
        }

        fclose(arch);
        fclose(temp);

        // Eliminamos el archivo original y renombramos el archivo temporal
        remove(name);
        rename("temp.bin", name);
        printf("\nCliente agregado en la posición %d.", pos);
    }
}

bool bajaFisicaCliente(char name[], int pos)
{
    FILE *arch, *temp;
    t_clientes cliente;
    int contador = 0;

    // Abrimos el archivo original en modo lectura y el archivo temporal en modo escritura
    arch = fopen(name, "rb");
    if (arch == NULL)
    {
        printf("Error al abrir el archivo original.\n");
        return false;
    }

    temp = fopen("temp.bin", "wb");
    if (temp == NULL)
    {
        printf("Error al abrir el archivo temporal.\n");
        fclose(arch);
        return false;
    }
    if (pos > contarRegistros(name))
    {
        printf("En la posicion indicada el archivo no contiene datos");
        return false;
    }

    // Copiamos todos los registros al archivo temporal, excepto el que está en la posición dada
    while (fread(&cliente, sizeof(t_clientes), 1, arch))
    {
        if (contador != pos)
        {
            fwrite(&cliente, sizeof(t_clientes), 1, temp);
        }
        contador++;
    }

    fclose(arch);
    fclose(temp);

    // Eliminamos el archivo original y renombramos el archivo temporal
    remove(name);
    rename("temp.bin", name);

    return true;
}

int contarRegistros(char name[])
{
    FILE *arch = fopen(name, "rb");
    if (arch == NULL)
    {
        return 0; // Archivo no existe o no puede abrirse
    }
    fseek(arch, 0, SEEK_END);
    int registrosTotales = ftell(arch) / sizeof(t_clientes);
    fclose(arch);
    return registrosTotales;
}

void leerArchivoEntero(char name[])
{
    FILE *arch = fopen(name, "rb");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Obtener el tamaño total del archivo
    fseek(arch, 0, SEEK_END);
    long longitudArchivo = ftell(arch);
    fseek(arch, 0, SEEK_SET);

    // Calcular el número de registros
    size_t longitudRegistro = sizeof(t_clientes);
    size_t numRegistros = longitudArchivo / longitudRegistro;

    // Verificar si el tamaño es múltiplo del tamaño del registro
    if (longitudArchivo % longitudRegistro != 0)
    {
        printf("El tamaño del archivo no es múltiplo del tamaño del registro.\n");
        fclose(arch);
        return;
    }

    // Leer y procesar cada registro
    t_clientes cliente;
    printf("------------------------------\n");
    for (size_t i = 0; i < numRegistros; i++)
    {
        // Leer un registro
        if (fread(&cliente, sizeof(t_clientes), 1, arch) == 1)
        {
            printf("| Registro #%zu |", i);
            printf(" ID: %d |", cliente.codigoCliente);
            printf(" Nombre: %s |", cliente.nombre);
            printf(" Apellido: %s\n", cliente.apellido);
        }
        else
        {
            printf("Error al leer el registro #%zu.\n", i);
            break;
        }
    }

    fclose(arch);
}

void leerRegistro(char name[], int n)
{
    FILE *arch = fopen(name, "rb");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    // Obtener el tamaño total del archivo
    fseek(arch, 0, SEEK_END);
    long longitudArchivo = ftell(arch);
    fseek(arch, 0, SEEK_SET);

    // Verificar que la posición deseada está dentro del archivo
    long longitudRegistro = sizeof(t_clientes);
    if (n * longitudRegistro >= longitudArchivo)
    {
        printf("Registro #%d no existe. El archivo solo tiene %ld bytes.\n", n, longitudArchivo);
        fclose(arch);
        return;
    }

    // Mover el puntero del archivo al inicio del registro n
    if (fseek(arch, n * longitudRegistro, SEEK_SET) != 0)
    {
        perror("Error al mover el puntero del archivo");
        fclose(arch);
        return;
    }

    // Leer el registro
    t_clientes cliente;
    if (fread(&cliente, sizeof(t_clientes), 1, arch) == 1)
    {
        printf("Registro #%d\n", n);
        printf("ID: %d\n", cliente.codigoCliente);
        printf("Nombre: %s\n", cliente.nombre);
        printf("Apellido: %s\n", cliente.apellido);
    }
    else
    {
        printf("No se pudo leer el registro #%d.\n", n);
    }

    fclose(arch);
}

void modificacionDeRegistro(char name[], int pos)
{
    FILE *arch = fopen(name, "r+b");
    if (arch == NULL)
    {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    t_clientes cliente;
    int encontrado = 0;
    int acumulador = 0;
    while (fread(&cliente, sizeof(cliente), 1, arch))
    {
        if (pos == acumulador)
        {
            fseek(arch, -sizeof(cliente), SEEK_CUR);
            t_clientes clienteModificado = cliente;
            printf("\nEl registro a modificar es : ");
            leerRegistro(name, acumulador);
            int flag = 1;
            while (flag == 1)
            {
                int respuesta = menuModificacion();
                switch (respuesta)
                {
                case 1:
                    printf("\n - Ingrese el nuevo nombre: ");
                    fgets(clienteModificado.nombre, sizeof(clienteModificado.nombre), stdin);
                    clienteModificado.nombre[strcspn(clienteModificado.nombre, "\n")] = '\0';
                    if (clienteModificado.codigoCliente == cliente.codigoCliente)
                    {
                        clienteModificado.codigoCliente = cliente.codigoCliente;
                    }
                    if (clienteModificado.apellido == cliente.apellido)
                    {
                        strcpy(clienteModificado.apellido, cliente.apellido);
                    }
                    break;
                case 2:
                    printf("\n - Ingrese el nuevo Apellido: ");
                    fgets(clienteModificado.apellido, sizeof(clienteModificado.apellido), stdin);
                    clienteModificado.apellido[strcspn(clienteModificado.apellido, "\n")] = '\0';
                    if (clienteModificado.codigoCliente == cliente.codigoCliente)
                    {
                        clienteModificado.codigoCliente = cliente.codigoCliente;
                    }
                    if (clienteModificado.nombre == cliente.nombre)
                    {
                        strcpy(clienteModificado.nombre, cliente.nombre);
                    }
                    break;
                case 3:
                    printf("\n - Ingrese el nuevo Codigo de cliente: ");
                    scanf("%d", &clienteModificado.codigoCliente);
                    limpiarBuffer(); // Limpiar el búfer después de scan
                    if (clienteModificado.apellido == cliente.apellido)
                    {
                        strcpy(clienteModificado.apellido, cliente.apellido);
                    }
                    if (clienteModificado.nombre == cliente.nombre)
                    {
                        strcpy(clienteModificado.nombre, cliente.nombre);
                    }
                    break;
                default:
                    break;
                }
                printf("\n Desea hacer otra modificacion sobre el mismo cliente (1(SI) - 0(NO)): ");
                scanf("%d", &flag);
                while (flag > 1 || flag < 0)
                {
                    printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
                    printf("\n - 1 - SI");
                    printf("\n - 0 - NO");
                    scanf("%d", &respuesta);
                }
                limpiarBuffer(); // Limpiar el búfer después de scanf
            }
            fwrite(&clienteModificado, sizeof(cliente), 1, arch);
            encontrado = 1;
            break;
        }
        acumulador++;
    }
    fclose(arch);
    if (encontrado)
    {
        printf("Empleado modificado con éxito.\n");
    }
    else
    {
        printf("ERRORN\n");
    }
}

int menuModificacion()
{
    int respuesta;
    printf("\n - Que desea modificar? : ");
    printf("\n - 1 - Nombre");
    printf("\n - 2 - Apellido");
    printf("\n - 3 - CodigoCliente");
    printf("\n");
    scanf("%d", &respuesta);
    while (respuesta > 3 || respuesta < 1)
    {
        printf("\n - RESPUESTA INCORRECTA REEINGRESE UNA OPCION; ");
        printf("\n - 1 - Nombre");
        printf("\n - 2 - Apellido");
        printf("\n - 3 - CodigoCliente");
        printf("\n");
        scanf("%d", &respuesta);
    }
    limpiarBuffer(); // Limpiar el búfer después de scanf
    return respuesta;
}

void convertirAMinusculas(char cadena[])
{
    int i = 0;
    while (cadena[i])
    {
        cadena[i] = tolower(cadena[i]); // Convierte cada carácter a minúscula
        i++;
    }
}