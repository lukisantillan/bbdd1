#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CAMPOS 10
#define MAX_NOMBRE_CAMPO 50
#define MAX_REGISTROS 1000
#define FILENAME "datos.bin"
#define METADATA_FILENAME "metadata.bin"
const int NULO = -1;

typedef struct
{
    char nombre[MAX_NOMBRE_CAMPO];
    int longitud;
} Campo;

typedef struct
{
    int cantidadCampos;
    Campo campos[MAX_CAMPOS];
    int cantidadRegistros;
    int longitudRegistro;
} Metadata;

typedef struct
{
    char *datos; // Datos de los campos
    int estado;  // 1 si el registro está ocupado, 0 si está vacío
} Registro;

// Funciones para manejar la metadata
void definirEstructuraArchivo(Metadata *metadata);
void guardarMetadata(const Metadata *metadata);
// Función para mostrar el contenido del archivo metadata.bin
void verArchivoMetadata(char *nombreArchivo);

// Funciones para manejar los registros
bool esPrimo(int n);
int primoMasCercano(int cantidad);
int funcionHashing(int clave, int cantidadRegistros);
Registro datosRegistro();
void altaRegistro(Registro nuevoRegistro);

// Funciones auxiliares
void leerArchivoEntero();
void limpiarBuffer();
void crearArchivo();

int main()
{
    Metadata metadata;

    // Menú principal
    int opcion;
    do
    {
        printf("\nMenu:\n");
        printf("1. Definir estructura del archivo\n");
        printf("2. Utilizar el archivo (ABM)\n");
        printf("0. Salir\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);
        limpiarBuffer();

        switch (opcion)
        {
        case 1:
            definirEstructuraArchivo(&metadata);
            guardarMetadata(&metadata);
            int respuesta = 0;
            printf("Desea leer el archivo metadata? (1 si - 0 no)\n");
            scanf("%i", &respuesta);
            while (respuesta > 1 || respuesta < 0)
            {
                printf("Ingrese un valor valido (1 si - 0 no)\n");
                scanf("%i", &respuesta);
            }
            if (respuesta == 1)
            {
                verArchivoMetadata(METADATA_FILENAME);
            }
            crearArchivo();
            break;
        case 2:
            printf("\n1. Alta\n2. Baja\n3. Modificación\nElija una opción: ");
            scanf("%d", &opcion);
            limpiarBuffer();
            if (opcion == 1)
            {
                Registro registro = datosRegistro();
                altaRegistro(registro);
                leerArchivoEntero();
            }
            else if (opcion == 2)
            {
                // bajaRegistro(&metadata, registros);
            }
            else if (opcion == 3)
            {
                // modificarRegistro(&metadata, registros);
            }
            // guardarRegistros(&metadata, registros);
            break;
        case 0:
            printf("Saliendo...\n");
            break;
        default:
            printf("Opción no válida.\n");
        }
    } while (opcion != 0);

    return 0;
}

void definirEstructuraArchivo(Metadata *metadata)
{
    printf("Ingrese la cantidad de campos: ");
    scanf("%d", &metadata->cantidadCampos);
    limpiarBuffer();
    int longitud = 0;
    for (int i = 0; i < metadata->cantidadCampos; i++)
    {
        printf("Ingrese el nombre del campo %d: ", i + 1);
        fgets(metadata->campos[i].nombre, MAX_NOMBRE_CAMPO, stdin);
        metadata->campos[i].nombre[strcspn(metadata->campos[i].nombre, "\n")] = '\0';

        printf("Ingrese la longitud máxima del campo %d: ", i + 1);
        scanf("%d", &metadata->campos[i].longitud);
        limpiarBuffer();
        longitud += metadata->campos[i].longitud;
    }

    printf("Ingrese la cantidad de registros: ");
    scanf("%d", &metadata->cantidadRegistros);
    limpiarBuffer();
    metadata->longitudRegistro = longitud;
}

void guardarMetadata(const Metadata *metadata)
{
    FILE *archivo = fopen(METADATA_FILENAME, "wb");
    if (archivo == NULL)
    {
        printf("Error al crear el archivo de metadata.\n");
        return;
    }
    fwrite(metadata, sizeof(Metadata), 1, archivo);
    fclose(archivo);
}

void verArchivoMetadata(char *nombreArchivo)
{
    FILE *arch = fopen(nombreArchivo, "rb");
    if (arch == NULL)
    {
        printf("No se pudo abrir el archivo %s.\n", nombreArchivo);
        return;
    }

    Metadata metadata;

    // Leer el contenido del archivo
    fread(&metadata, sizeof(Metadata), 1, arch);

    // Mostrar la información leída
    printf("Cantidad de campos: %d\n", metadata.cantidadCampos);
    for (int i = 0; i < metadata.cantidadCampos; i++)
    {
        printf("Campo #%d: %s (Longitud: %d)\n", i + 1, metadata.campos[i].nombre, metadata.campos[i].longitud);
    }
    printf("Cantidad de registros: %d\n", metadata.cantidadRegistros);

    fclose(arch);
}

// Función para verificar si un número es primo
bool esPrimo(int n)
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;
    for (int i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

// Función para encontrar el primo más cercano menor o igual a cantidad
int primoMasCercano(int cantidad)
{
    for (int i = cantidad; i >= 2; i--)
    {
        if (esPrimo(i))
        {
            return i;
        }
    }
    return 2; // Si no encuentra ninguno (lo cual es improbable), devuelve 2.
}

int funcionHashing(int clave, int cantidadRegistros)
{
    int primo = primoMasCercano(cantidadRegistros); // Encontrar el primo más cercano
    return clave % primo;                           // Usar el primo como divisor para el hashing
}

// Funciones con el archivo data
void crearArchivo()
{
    FILE *arch, *metadata;

    // Abro archivo metadata para buscar la longitud del registro
    metadata = fopen(METADATA_FILENAME, "rb");
    if (metadata == NULL)
    {
        printf("No se pudo abrir el archivo de metadata.\n");
        return;
    }

    Metadata datos;
    fread(&datos, sizeof(Metadata), 1, metadata);
    fclose(metadata);

    // Abro archivo para escribirlo con la longitud necesaria
    arch = fopen(FILENAME, "wb");
    if (arch == NULL)
    {
        printf("No se pudo crear el archivo.\n");
        return;
    }

    // Inicializo el registro con la longitud del campo datos
    Registro registro;
    registro.datos = (char *)calloc(datos.longitudRegistro, sizeof(char));
    if (registro.datos == NULL)
    {
        printf("No se pudo asignar memoria.\n");
        fclose(arch);
        return;
    }
    registro.estado = 0;

    for (int i = 0; i < datos.cantidadRegistros; i++)
    {
        fwrite(&registro, sizeof(Registro), 1, arch);
    }

    fclose(arch);
    free(registro.datos); // Libera la memoria del campo datos
}

void altaRegistro(Registro nuevoRegistro)
{
    FILE *arch = fopen(FILENAME, "r+b");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    FILE *datos = fopen(METADATA_FILENAME, "rb");
    if (datos == NULL)
    {
        perror("Error al abrir el archivo de metadata");
        fclose(arch);
        return;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, datos);
    fclose(datos);

    int posicionAdarElAlta;
    Registro leido;

    long longitudRegistro = sizeof(Registro);

    // Loop para pedir la posición hasta que se seleccione una posición libre
    do
    {
        printf("EN QUE POSICIÓN DESEA DAR EL ALTA DEL REGISTRO : \n");
        scanf("%d", &posicionAdarElAlta);

        // Verifica si la posición está dentro del rango válido
        while (posicionAdarElAlta >= metadata.cantidadRegistros || posicionAdarElAlta < 0)
        {
            printf("REINGRESE UNA POSICION CORRECTA : \n");
            scanf("%d", &posicionAdarElAlta);
        }

        // Mueve el puntero a la posición seleccionada
        if (fseek(arch, posicionAdarElAlta * longitudRegistro, SEEK_SET) != 0)
        {
            perror("Error al mover el puntero del archivo");
            fclose(arch);
            return;
        }

        // Lee el registro en la posición seleccionada
        fread(&leido, sizeof(Registro), 1, arch);

        if (leido.estado == 1)
        {
            printf("La posición está ocupada, elija otra\n");
        }
    } while (leido.estado == 1);

    nuevoRegistro.estado = 1; // Marca el nuevo registro como ocupado

    // Mueve el puntero de vuelta a la posición seleccionada
    if (fseek(arch, posicionAdarElAlta * longitudRegistro, SEEK_SET) != 0)
    {
        perror("Error al mover el puntero del archivo");
        fclose(arch);
        return;
    }

    // Escribe el nuevo registro en la posición seleccionada
    if (fwrite(&nuevoRegistro, sizeof(Registro), 1, arch) != 1)
    {
        perror("Error al escribir el registro en el archivo");
    }

    fclose(arch);
}

Registro datosRegistro()
{
    Registro nuevoRegistro;

    FILE *datos = fopen(METADATA_FILENAME, "rb");
    if (datos == NULL)
    {
        perror("Error al abrir el archivo de metadata");
        return nuevoRegistro;
    }

    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, datos);
    fclose(datos);

    // Asignar memoria para los datos del registro según la longitud total de los campos
    nuevoRegistro.datos = (char *)malloc(metadata.longitudRegistro * sizeof(char));
    if (nuevoRegistro.datos == NULL)
    {
        printf("No se pudo asignar memoria para los datos del registro.\n");
        exit(1);
    }

    int offset = 0;
    for (int i = 0; i < metadata.cantidadCampos; i++)
    {
        char buffer[256]; // Buffer temporal para almacenar la entrada del usuario

        printf("\nIngrese %s: ", metadata.campos[i].nombre);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar el salto de línea

        // Copiar el valor ingresado al lugar correspondiente en datos
        int longitudCampo = metadata.campos[i].longitud;
        if ((int)strlen(buffer) > longitudCampo)
        {
            printf("Advertencia: El valor ingresado para %s es demasiado largo. Se truncará.\n", metadata.campos[i].nombre);
        }
        strncpy(nuevoRegistro.datos + offset, buffer, longitudCampo);

        // Rellenar el resto del campo con espacios si es necesario
        if ((int)strlen(buffer) < longitudCampo)
        {
            memset(nuevoRegistro.datos + offset + strlen(buffer), ' ', longitudCampo - strlen(buffer));
        }

        // Mover el offset al siguiente campo
        offset += longitudCampo;
    }

    nuevoRegistro.estado = 1;

    return nuevoRegistro;
}

void leerArchivoEntero()
{
    FILE *arch = fopen(FILENAME, "rb");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }
    FILE *datos = fopen(METADATA_FILENAME, "rb");
    if (datos == NULL)
    {
        perror("Error al abrir el archivo de metadata");
        fclose(arch);
    }
    Metadata metadata;
    fread(&metadata, sizeof(Metadata), 1, datos);
    fclose(datos);

    // Leer y procesar cada registro
    Registro cliente;
    printf("\n------------------------------\n");
    for (int i = 0; i < metadata.cantidadRegistros; i++)
    {
        // Leer un registro
        if (fread(&cliente, sizeof(Registro), 1, arch) == 1)
        {
            if (cliente.estado == 1)
            {
                printf("| Registro #%d |", i);
                printf("Datos : %s\n", cliente.datos);
            }
        }
        else
        {
            printf("Error al leer el registro #%d.\n", i);
            break;
        }
    }

    fclose(arch);
}

void limpiarBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}