#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

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
void manejarInterrupcion(int signum);
// Función para mostrar el contenido del archivo metadata.bin
void verArchivoMetadata(char *nombreArchivo);

// Funciones para manejar los registros
Registro datosRegistro();
void altaRegistro(char *nombreArchivo, Registro nuevoRegistro);
void bajaRegistro(char *nombreArchivo);
void modificarRegistro(char *nombreArchivo);

// Funciones auxiliares
void leerArchivoEntero(char *nombreArchivo);
void limpiarBuffer();
void crearArchivo(char *nombreMetadata, char *nombreArchivo);

int main()
{
    // Menú principal
    signal(SIGINT, manejarInterrupcion);
    Metadata metadata;
    int opcion;
    bool flag = false;
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
            if (flag)
            {
                printf("\n La estructura ya esta definida, utilice ABM\n");
                break;
            }
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
            crearArchivo(METADATA_FILENAME, FILENAME);
            flag = true;
            break;
        case 2:
            if (!flag)
            {
                printf("\nNo definio la estructura del archivo\n");
                break;
            }
            printf("\n1. Alta\n2. Baja\n3. Modificación\n4. Leer archivo \nElija una opción: ");
            scanf("%d", &opcion);
            limpiarBuffer();
            if (opcion == 1)
            {
                Registro registro = datosRegistro();
                altaRegistro(FILENAME, registro);
            }
            else if (opcion == 2)
            {
                bajaRegistro(FILENAME);
            }
            else if (opcion == 3)
            {
                modificarRegistro(FILENAME);
            }
            else if (opcion == 4)
            {
                leerArchivoEntero(FILENAME);
            }

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

// Funciones con el archivo data
void crearArchivo(char *nombreMetadata, char *nombreArchivo)
{
    FILE *arch, *metadata;

    // Abro archivo metadata para buscar la longitud del registro
    metadata = fopen(nombreMetadata, "rb");
    if (metadata == NULL)
    {
        printf("No se pudo abrir el archivo de metadata.\n");
        return;
    }

    Metadata datos;
    size_t bytesLeidos = fread(&datos, sizeof(Metadata), 1, metadata);
    if (bytesLeidos != 1)
    {
        printf("Error al leer el archivo de metadata.\n");
        fclose(metadata);
        return;
    }
    fclose(metadata);

    // Abro archivo para escribirlo con la longitud necesaria
    arch = fopen(nombreArchivo, "wb");
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

void altaRegistro(char *nombreArchivo, Registro nuevoRegistro)
{
    FILE *arch = fopen(nombreArchivo, "r+b");
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

void bajaRegistro(char *nombreArchivo)
{
    FILE *arch = fopen(nombreArchivo, "r+b");
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
    int posicionAdarDeBaja;
    Registro leido;
    // Loop para pedir la posición hasta que se seleccione una posición libre
    do
    {
        printf("EN QUE POSICIÓN DESEA DAR LA BAJA DEL REGISTRO : \n");
        scanf("%d", &posicionAdarDeBaja);

        // Verifica si la posición está dentro del rango válido
        while (posicionAdarDeBaja >= metadata.cantidadRegistros || posicionAdarDeBaja < 0)
        {
            printf("REINGRESE UNA POSICION CORRECTA : \n");
            scanf("%d", &posicionAdarDeBaja);
        }

        // Mueve el puntero a la posición seleccionada
        if (fseek(arch, posicionAdarDeBaja * sizeof(Registro), SEEK_SET) != 0)
        {
            perror("Error al mover el puntero del archivo");
            fclose(arch);
            return;
        }

        // Lee el registro en la posición seleccionada
        fread(&leido, sizeof(Registro), 1, arch);

        if (leido.estado != 1)
        {
            printf("La posición está Libre, por lo que no se puede realizar una baja, elija otra\n");
            printf("Desea ver la lista de los registros? (1-SI) \n");
            scanf("%d", &posicionAdarDeBaja);
            if (posicionAdarDeBaja == 1)
            {
                leerArchivoEntero(nombreArchivo);
            }
        }
    } while (leido.estado != 1);

    int confirmacion;
    leido.estado = 2;
    fseek(arch, posicionAdarDeBaja * sizeof(Registro), SEEK_SET);
    printf("El registro #%d tiene los siguientes datos: \n", posicionAdarDeBaja);
    printf("Datos : %s \n", leido.datos);
    printf("Estado : %d", leido.estado);
    printf("Desea confirmar la eliminación? (1-SI 0-NO) \n");
    scanf("%d", &confirmacion);
    while (confirmacion < 0 || confirmacion > 1)
    {
        printf("Reingrese un valor correcto - (1 SI - 0 NO)");
        scanf("%d", &confirmacion);
    }
    if (confirmacion == 1)
    {
        printf("escribiendo nuevo registro\n");
        fwrite(&leido, sizeof(Registro), 1, arch);
    }
    fclose(arch);
}

void modificarRegistro(char *nombreArchivo)
{
    FILE *arch = fopen(nombreArchivo, "r+b");
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

    int indice = 0;
    Registro leido;

    // Loop para pedir la posición hasta que se seleccione una posición válida
    do
    {
        printf("EN QUE POSICIÓN DESEA MODIFICAR UN REGISTRO : \n");
        scanf("%d", &indice);

        // Verifica si la posición está dentro del rango válido
        while (indice >= metadata.cantidadRegistros || indice < 0)
        {
            printf("REINGRESE UNA POSICION CORRECTA : \n");
            scanf("%d", &indice);
        }

        // Mueve el puntero a la posición seleccionada
        if (fseek(arch, indice * sizeof(Registro), SEEK_SET) != 0)
        {
            perror("Error al mover el puntero del archivo");
            fclose(arch);
            return;
        }

        // Lee el registro en la posición seleccionada
        fread(&leido, sizeof(Registro), 1, arch);

        if (leido.estado != 1)
        {
            printf("La posición está Libre, por lo que no se puede realizar una modificación, elija otra\n");
            printf("Desea ver la lista de los registros? (1-SI) \n");
            int verRegistros;
            scanf("%d", &verRegistros);
            if (verRegistros == 1)
            {
                leerArchivoEntero(nombreArchivo);
            }
        }
    } while (leido.estado != 1);

    // Mostrar información del registro actual
    printf("El registro #%d tiene los siguientes datos: \n", indice);
    printf("Datos : %s \n", leido.datos);
    printf("Estado : %d\n", leido.estado);

    // Selección del campo a modificar
    int campo;
    printf("¿Qué campo desea modificar? (1-%d)\n", metadata.cantidadCampos);
    scanf("%d", &campo);

    // Validación del campo seleccionado
    if (campo <= 0 || campo > metadata.cantidadCampos)
    {
        printf("Campo inválido.\n");
        fclose(arch);
        return;
    }

    // Limpiar el buffer antes de leer nuevos datos
    limpiarBuffer();

    // Obtener el nuevo valor para el campo
    printf("\nIngrese el nuevo valor para el campo %s: ", metadata.campos[campo - 1].nombre);
    char nuevoValor[metadata.campos[campo - 1].longitud + 1];  // +1 para el terminador nulo
    fgets(nuevoValor, sizeof(nuevoValor), stdin);
    nuevoValor[strcspn(nuevoValor, "\n")] = '\0';  // Eliminar el salto de línea

    // Verificar que el nuevo valor no exceda la longitud permitida
    if ((int)strlen(nuevoValor) > metadata.campos[campo - 1].longitud)
    {
        printf("El valor ingresado excede la longitud permitida para el campo.\n");
        fclose(arch);
        return;
    }

    // Actualizar el campo en el registro leído
    int offset = 0;
    for (int i = 0; i < metadata.cantidadCampos; i++)
    {
        if (i == (campo - 1))
        {
            // Copiar el nuevo valor en el campo correspondiente del registro
            strncpy(leido.datos + offset, nuevoValor, metadata.campos[i].longitud);
            break;
        }
        offset += metadata.campos[i].longitud;
    }

    // Mover el puntero a la posición original del registro
    fseek(arch, indice * sizeof(Registro), SEEK_SET);

    // Escribir el registro modificado de nuevo en el archivo
    fwrite(&leido, sizeof(Registro), 1, arch);

    fclose(arch);
    printf("Modificación realizada con éxito.\n");
}

Registro datosRegistro()
{
    Registro nuevoRegistro;

    FILE *datos = fopen(METADATA_FILENAME, "rb");
    if (datos == NULL)
    {
        perror("Error al abrir el archivo de metadata");
        exit(0);
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

void leerArchivoEntero(char *nombreArchivo)
{
    FILE *arch = fopen(nombreArchivo, "rb");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    Registro cliente;
    int index = 0;

    printf("\n------------------------------\n");
    printf(" Mostrando todos los registros \n");
    printf("------------------------------\n");

    while (fread(&cliente, sizeof(Registro), 1, arch) == 1)
    {
        // Verificar si el registro está activo
        if (cliente.estado == 1)
        {
            printf("Registro #%d:\n", index);
            printf("Estado   : %d", cliente.estado);
            printf("\tDatos    : %s\n", cliente.datos);
            printf("------------------------------\n");
        }
        index++;
    }

    fclose(arch);
}

void limpiarBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void manejarInterrupcion(int signum)
{
    printf("\nInterrupción detectada. Cerrando correctamente...\n");
    // Realiza limpieza aquí (cierra archivos, libera memoria, etc.)
    exit(signum);
}