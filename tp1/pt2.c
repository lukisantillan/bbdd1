#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

const int NULO = -1;
char nada[200];

typedef struct
{
    int codigoCliente;
    char nombre[20];
    char apellido[20];
    int estado; // 1 activo , 2 borrado, 0 vacio;
    int siguiente;
} t_clientes;

void crearArchivo(char name[], int cantidadDeRegistros);
int funcionHashing(int codigoCliente, int cantidadDeRegistros);
bool esPrimo(int numero);
void altaCliente(char name[], int cantidadDeRegistros, t_clientes nuevoCliente);
int mayorPrimoMasCercano(int numero);
t_clientes datosCliente();
void limpiarBuffer();
void leerArchivoEntero(char name[]);

int main()
{
    crearArchivo("clientes.bin", 700);
    t_clientes nuevo = datosCliente();
    t_clientes nuevo2 = datosCliente();
    t_clientes nuevo3 = datosCliente();
    altaCliente("clientes.bin", 700, nuevo);
    altaCliente("clientes.bin", 700, nuevo2);
    altaCliente("clientes.bin", 700, nuevo3);
    leerArchivoEntero("clientes.bin");
    return 0;
}

// Funcion hashing, toma el resto de la division del codigo de cliente dividio el primo mas cercano al numero de registros.
int funcionHashing(int codigoCliente, int cantidadDeRegistros)
{
    int mayorPrimo = mayorPrimoMasCercano(cantidadDeRegistros);
    return codigoCliente % mayorPrimo;
}

void limpiarBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

t_clientes datosCliente()
{
    t_clientes nuevoCliente;

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
    nuevoCliente.siguiente = NULO;

    return nuevoCliente;
}
// Función para verificar si un número es primo
bool esPrimo(int numero)
{
    if (numero <= 1)
        return false;
    if (numero == 2 || numero == 3)
        return true;
    if (numero % 2 == 0 || numero % 3 == 0)
        return false;
    for (int i = 5; i * i <= numero; i += 6)
    {
        if (numero % i == 0 || numero % (i + 2) == 0)
            return false;
    }
    return true;
}

// Función para encontrar el mayor primo más cercano
int mayorPrimoMasCercano(int numero)
{
    while (numero > 1)
    {
        if (esPrimo(numero))
        {
            return numero;
        }
        numero--;
    }
    return -1;
}

void crearArchivo(char name[], int cantidadDeRegistros)
{
    FILE *arch = fopen(name, "wb");
    if (arch == NULL)
    {
        printf("No se pudo crear el archivo.\n");
        return;
    }

    t_clientes nuevoCliente;
    memset(&nuevoCliente, 0, sizeof(t_clientes)); // Inicializa todos los campos a 0
    nuevoCliente.siguiente = NULO;

    for (int i = 0; i < cantidadDeRegistros; i++)
    {
        fwrite(&nuevoCliente, sizeof(t_clientes), 1, arch);
    }
    fclose(arch);
}

void altaCliente(char name[], int cantidadDeRegistros, t_clientes nuevoCliente)
{
    FILE *arch = fopen(name, "r+b");
    if (arch == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    int indiceAguardarElCliente = funcionHashing(nuevoCliente.codigoCliente, cantidadDeRegistros);
    long longitudRegistro = sizeof(t_clientes);
    if (fseek(arch, indiceAguardarElCliente * longitudRegistro, SEEK_SET) != 0)
    {
        perror("Error al mover el puntero del archivo");
        fclose(arch);
        return;
    }
    t_clientes leido;
    fread(&leido, sizeof(t_clientes), 1, arch);
    fseek(arch, -longitudRegistro, SEEK_CUR);
    if (leido.estado = 1)
    {
        // OBTENGO LONGITUD DEL ARCHIVO;
        if (fseek(arch, 0, SEEK_END) != 0)
        {
            perror("Error al mover el puntero del archivo");
            fclose(arch);
            return;
        }
        long longitudArchivo = ftell(arch);
        long registrosReales = longitudArchivo / longitudRegistro;
        // GUARDO INDICE LIBRE
        int indiceLibre = (int)registrosReales;
        if (indiceLibre > registrosReales)
        {
            printf("\n NO HAY ESPACIO PARA ALMACENAR EL CLIENTE");
            return;
        }
        // MUEVO PUNTERO AL LIBRE;
        fseek(arch, indiceLibre * longitudRegistro, SEEK_SET);
        // ESCRIBO EL REGISTRO EN EL LIBRE;
        fwrite(&nuevoCliente, sizeof(t_clientes), 1, arch);
        // PREGUNTO SI EL QUE OCUPA EL LUGAR QUE LE CORRESPONDE TIENE SIGUIENTE;
        if (leido.siguiente != -1)
        {
            t_clientes aux;
            fseek(arch, leido.siguiente * longitudRegistro, SEEK_SET);
            fread(&aux, sizeof(t_clientes), 1, arch);
            // ACTUALIZO HASTA QUE EL QUE OCUPA LUGAR NO TENGA SIGUIENTE;
            while (aux.siguiente != -1 && aux.estado != 2)
            {
                // BUSCO HASTA QUE NO TENGA SIGUIENTE O HASTA QUE HAYA UNO DADO DE BAJA
                fread(&aux, sizeof(t_clientes), 1, arch);
            }
            // PREGUNTO EL ESTADO DEL ULTIMO QUE TENGO
            if (aux.estado == 2)
            {
                // SI ESTE ULTIMO ESTA DADO DE BAJA, MUEVO EL PUNTERO DOS REGISTROS ATRAS
                fseek(arch, -longitudRegistro * 2, SEEK_CUR);
                // LEO EL REGISTRO Y ACTUALIZO PUNTEROS;
                fread(&aux, sizeof(t_clientes), 1, arch);
                nuevoCliente.siguiente = aux.siguiente;
                aux.siguiente = indiceLibre;
            }
            else
            {
                // ACTUALIZO EL SIGUIENTE DEL ULTIMO QUE TUVO SIGUIENTE.
                aux.siguiente = indiceLibre;
            }
        }
        else
        {
            // SI EL LEIDO NO TENIA SIGUIENTE YA GUARDO EL INDICE;
            leido.siguiente = indiceLibre;
        }
    }
    else
    {
        if (leido.estado == 2)
        {
            // OBTENGO LONGITUD DEL ARCHIVO;
            if (fseek(arch, 0, SEEK_END) != 0)
            {
                perror("Error al mover el puntero del archivo");
                fclose(arch);
                return;
            }
            long longitudArchivo = ftell(arch);
            long registrosReales = longitudArchivo / longitudRegistro;
            // GUARDO INDICE LIBRE
            int indiceLibre = (int)registrosReales;
            if (indiceLibre > registrosReales)
            {
                printf("\n NO HAY ESPACIO PARA ALMACENAR EL CLIENTE");
                return;
            }
            // MUEVO PUNTERO AL LIBRE;
            fseek(arch, indiceLibre * longitudRegistro, SEEK_SET);
            // ESCRIBO EL REGISTRO QUE ESTA DADO DE BAJA EN EL LIBRE;
            fwrite(&leido, sizeof(t_clientes), 1, arch);
            // MUEVO PUNTERO AL INDICE A GUARDAR
            fseek(arch, indiceAguardarElCliente * longitudRegistro, SEEK_SET);
            // APUNTO EL SIGUIENTE DEL NUEVO AL LIBRE QUE CREE CON EL DADO DE BAJA
            nuevoCliente.siguiente = indiceLibre;
        }
        // ESCRIBO EN EL REGISTRO CORRESPONDIENTE;
        fwrite(&nuevoCliente, sizeof(t_clientes), 1, arch);
    }
    fclose(arch);
}

void bajaCliente(char name[], int codigoCliente, int cantidadDeRegistros)
{
    FILE *arch;
    t_clientes cliente;
    int contador = 0;

    // Abrimos el archivo original en modo lectura y el archivo temporal en modo escritura
    arch = fopen(name, "rb");
    if (arch == NULL)
    {
        printf("Error al abrir el archivo original.\n");
        return false;
    }

    long longitudRegistro = sizeof(t_clientes);
    int indiceHash = funcionHashing(codigoCliente, cantidadDeRegistros);
    if (fseek(arch, indiceHash * longitudRegistro, SEEK_SET) != 0)
    {
        perror("Error al mover el puntero del archivo");
        fclose(arch);
        return;
    }
    // leo cliente
    fread(&cliente, sizeof(t_clientes), 1, arch);
    bool flag = true;
    // si codigo cliente es distinto al enviado por parametro, entro al ciclo para ver los siguientes;
    while (cliente.codigoCliente != codigoCliente)
    {
        fseek(arch, cliente.siguiente * longitudRegistro, SEEK_CUR);
        fread(&cliente, sizeof(t_clientes), 1, arch);
        if (cliente.siguiente = -1)
        {
            printf("\n No se encontro un cliente con ese codigo");
            return;
        }
    }

    printf("El cliente al que le quiere realizar la baja es el siguiente: \n");
    printf("\n------------------------------\n");
    printf(" ID: %d |", cliente.codigoCliente);
    printf(" Nombre: %s |", cliente.nombre);
    printf(" Apellido: %s\n", cliente.apellido);

    if (cliente.siguiente != -1)
    {
        //SI TIENE SIGUIENTE LO PISO CON EL SIGUIENTE, SI NO TIENE LE CAMBIO EL ESTAOD.
    }
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
    printf("\n------------------------------\n");
    for (size_t i = 0; i < numRegistros; i++)
    {
        // Leer un registro
        if (fread(&cliente, sizeof(t_clientes), 1, arch) == 1)
        {
            if (cliente.estado == 1)
            {
                printf("| Registro #%zu |", i);
                printf(" ID: %d |", cliente.codigoCliente);
                printf(" Nombre: %s |", cliente.nombre);
                printf(" Apellido: %s\n", cliente.apellido);
            }
        }
        else
        {
            printf("Error al leer el registro #%zu.\n", i);
            break;
        }
    }

    fclose(arch);
}