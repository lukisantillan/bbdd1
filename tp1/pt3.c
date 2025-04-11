#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

#define MAX_CAMPOS 10
#define MAX_NOMBRE_CAMPO 50
#define MAX_REGISTROS 1000
#define FILENAME "datos.txt"
#define METADATA_FILENAME "metadata.txt"
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
// Función para mostrar el contenido del archivo metadata.txt
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
bool ValidarEntero(int *numero);

int main()
{
  signal(SIGINT, manejarInterrupcion);
  Metadata metadata;
  int opcion;
  bool flag = false;

  // chequeo si hay un archivo metadata.txt creado
  FILE *archivo = fopen(METADATA_FILENAME, "r");
  if (archivo != NULL)
  {
    flag = true;
    fclose(archivo);
  }

  do
  {
    printf("\nMenu:\n");
    printf("1. Definir estructura del archivo\n");
    printf("2. Utilizar el archivo (ABM)\n");
    printf("3. Leer archivo ABM \n");
    printf("0. Salir\n");

    if (!ValidarEntero(&opcion))
    {
      printf("Opción no válida. Intente nuevamente.\n");
      continue;
    }

    switch (opcion)
    {
    case 1:
      if (flag)
      {
        printf("\nLa estructura ya está definida, utilice ABM\n");
        break;
      }
      definirEstructuraArchivo(&metadata);
      guardarMetadata(&metadata);
      int respuesta;
      printf("Desea leer el archivo metadata? (1 si - 0 no)\n");
      if (!ValidarEntero(&respuesta))
      {
        printf("Valor no válido. Intente nuevamente.\n");
        continue;
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
        printf("\nNo ha definido la estructura del archivo\n");
        break;
      }
      // si el archivo datos no existe lo creo
      FILE *datos = fopen(FILENAME, "r");
      if (datos == NULL)
      {
        crearArchivo(METADATA_FILENAME, FILENAME);
      }
      else
      {
        fclose(datos);
      }
      printf("\n1. Alta\n2. Baja\n3. Modificación\n4. Volver \nElija una opción: ");
      if (!ValidarEntero(&opcion) || opcion < 1 || opcion > 4)
      {
        if (opcion == 4)
        {
          break;
        }
        printf("Opción no válida. Intente nuevamente.\n");
        continue;
      }
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
      break;
    case 3:
      if (!flag)
      {
        printf("\n --- Todavia no definiste la estructura ---\n");
        break;
      }
      else
        leerArchivoEntero(FILENAME);

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
  bool resultado;
  do
  {
    printf("---- Ingrese la cantidad de campos ----\n");
    resultado = ValidarEntero(&metadata->cantidadCampos);
  } while (!resultado);
  int longitud = 0;
  for (int i = 0; i < metadata->cantidadCampos; i++)
  {
    printf("--- Ingrese el nombre del campo %d ----", i + 1);
    printf("\nNombre:");
    fgets(metadata->campos[i].nombre, MAX_NOMBRE_CAMPO, stdin);
    metadata->campos[i].nombre[strcspn(metadata->campos[i].nombre, "\n")] = '\0';

    printf("--- Ingrese la longitud máxima del campo %d ----\n", i + 1);
    if (!ValidarEntero(&metadata->campos[i].longitud))
    {
      printf("Opción no válida. Intente nuevamente.\n");
      continue;
    }
    longitud += metadata->campos[i].longitud;
  }

  do
  {
    printf("--- Ingrese la cantidad de registros ----\n");
    resultado = ValidarEntero(&metadata->cantidadRegistros);
    if (!resultado)
    {
      printf("Opción no válida. Intente nuevamente.\n");
      continue;
    }
  } while (!resultado);
  metadata->longitudRegistro = longitud;
}

void guardarMetadata(const Metadata *metadata)
{
  FILE *archivo = fopen(METADATA_FILENAME, "w");
  if (archivo == NULL)
  {
    printf("Error al crear el archivo de metadata.\n");
    return;
  }
  fprintf(archivo, "%d\n", metadata->cantidadCampos);
  for (int i = 0; i < metadata->cantidadCampos; i++)
  {
    fprintf(archivo, "%s %d\n", metadata->campos[i].nombre, metadata->campos[i].longitud);
  }
  fprintf(archivo, "%d\n", metadata->cantidadRegistros);
  fprintf(archivo, "%d\n", metadata->longitudRegistro);
  fclose(archivo);
}

void verArchivoMetadata(char *nombreArchivo)
{
  FILE *arch = fopen(nombreArchivo, "r");
  if (arch == NULL)
  {
    printf("No se pudo abrir el archivo %s.\n", nombreArchivo);
    return;
  }

  Metadata metadata;

  // Leer el contenido del archivo
  fscanf(arch, "%d", &metadata.cantidadCampos);
  for (int i = 0; i < metadata.cantidadCampos; i++)
  {
    fscanf(arch, "%s %d", metadata.campos[i].nombre, &metadata.campos[i].longitud);
  }
  fscanf(arch, "%d", &metadata.cantidadRegistros);
  fscanf(arch, "%d", &metadata.longitudRegistro);

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
  metadata = fopen(nombreMetadata, "r");
  if (metadata == NULL)
  {
    printf("No se pudo abrir el archivo de metadata.\n");
    return;
  }

  Metadata datos;
  fscanf(metadata, "%d", &datos.cantidadCampos);
  for (int i = 0; i < datos.cantidadCampos; i++)
  {
    fscanf(metadata, "%s %d", datos.campos[i].nombre, &datos.campos[i].longitud);
  }
  fscanf(metadata, "%d", &datos.cantidadRegistros);
  fscanf(metadata, "%d", &datos.longitudRegistro);
  fclose(metadata);

  // Abro archivo para escribirlo con la longitud necesaria
  arch = fopen(nombreArchivo, "w");
  if (arch == NULL)
  {
    printf("No se pudo crear el archivo.\n");
    return;
  }

  // Inicializo el registro con la longitud del campo datos
  for (int i = 0; i < datos.cantidadRegistros; i++)
  {
    fprintf(arch, "0\n"); // Estado inicial vacío
    for (int j = 0; j < datos.longitudRegistro; j++)
    {
      fprintf(arch, " "); // Espacios para los datos
    }
    fprintf(arch, "\n");
  }

  fclose(arch);
}

Registro datosRegistro()
{
  Registro registro;
  registro.datos = NULL;
  registro.estado = 1; // Estado ocupado
  return registro;
}

void altaRegistro(char *nombreArchivo, Registro nuevoRegistro)
{
  FILE *archivo = fopen(nombreArchivo, "r+");
  if (archivo == NULL)
  {
    printf("No se pudo abrir el archivo para alta.\n");
    return;
  }

  Metadata metadata;
  FILE *metadataFile = fopen(METADATA_FILENAME, "r");
  if (metadataFile == NULL)
  {
    printf("No se pudo abrir el archivo de metadata.\n");
    fclose(archivo);
    return;
  }

  fscanf(metadataFile, "%d", &metadata.cantidadCampos);
  for (int i = 0; i < metadata.cantidadCampos; i++)
  {
    fscanf(metadataFile, "%s %d", metadata.campos[i].nombre, &metadata.campos[i].longitud);
  }
  fscanf(metadataFile, "%d", &metadata.cantidadRegistros);
  fscanf(metadataFile, "%d", &metadata.longitudRegistro);
  fclose(metadataFile);

  char buffer[metadata.longitudRegistro + 1];
  for (int i = 0; i < metadata.cantidadRegistros; i++)
  {
    int estado;
    fscanf(archivo, "%d", &estado);
    fgets(buffer, sizeof(buffer), archivo);

    if (estado == 0)
    {
      fseek(archivo, -((long)strlen(buffer) + 2), SEEK_CUR);
      fprintf(archivo, "1\n");
      for (int j = 0; j < metadata.cantidadCampos; j++)
      {
        char valor[metadata.campos[j].longitud + 1];
        printf("Ingrese el valor para el campo '%s': ", metadata.campos[j].nombre);
        fgets(valor, sizeof(valor), stdin);
        valor[strcspn(valor, "\n")] = '\0';
        fprintf(archivo, "%-*s", metadata.campos[j].longitud, valor);
      }
      fprintf(archivo, "\n");
      fclose(archivo);
      printf("Registro agregado exitosamente.\n");
      return;
    }
  }

  printf("No hay espacio disponible para agregar un nuevo registro.\n");
  fclose(archivo);
}

void bajaRegistro(char *nombreArchivo)
{
  FILE *archivo = fopen(nombreArchivo, "r+");
  if (archivo == NULL)
  {
    printf("No se pudo abrir el archivo para baja.\n");
    return;
  }

  int registroID;
  printf("Ingrese el ID del registro a eliminar: ");
  if (!ValidarEntero(&registroID))
  {
    printf("ID no válido.\n");
    fclose(archivo);
    return;
  }

  Metadata metadata;
  FILE *metadataFile = fopen(METADATA_FILENAME, "r");
  if (metadataFile == NULL)
  {
    printf("No se pudo abrir el archivo de metadata.\n");
    fclose(archivo);
    return;
  }

  fscanf(metadataFile, "%d", &metadata.cantidadCampos);
  for (int i = 0; i < metadata.cantidadCampos; i++)
  {
    fscanf(metadataFile, "%s %d", metadata.campos[i].nombre, &metadata.campos[i].longitud);
  }
  fscanf(metadataFile, "%d", &metadata.cantidadRegistros);
  fscanf(metadataFile, "%d", &metadata.longitudRegistro);
  fclose(metadataFile);

  if (registroID < 0 || registroID >= metadata.cantidadRegistros)
  {
    printf("ID fuera de rango.\n");
    fclose(archivo);
    return;
  }

  fseek(archivo, registroID * (metadata.longitudRegistro + 3), SEEK_SET);
  fprintf(archivo, "0\n");
  fclose(archivo);
  printf("Registro eliminado exitosamente.\n");
}

void modificarRegistro(char *nombreArchivo)
{
  FILE *archivo = fopen(nombreArchivo, "r+");
  if (archivo == NULL)
  {
    printf("No se pudo abrir el archivo para modificación.\n");
    return;
  }

  int registroID;
  printf("Ingrese el ID del registro a modificar: ");
  if (!ValidarEntero(&registroID))
  {
    printf("ID no válido.\n");
    fclose(archivo);
    return;
  }

  Metadata metadata;
  FILE *metadataFile = fopen(METADATA_FILENAME, "r");
  if (metadataFile == NULL)
  {
    printf("No se pudo abrir el archivo de metadata.\n");
    fclose(archivo);
    return;
  }

  fscanf(metadataFile, "%d", &metadata.cantidadCampos);
  for (int i = 0; i < metadata.cantidadCampos; i++)
  {
    fscanf(metadataFile, "%s %d", metadata.campos[i].nombre, &metadata.campos[i].longitud);
  }
  fscanf(metadataFile, "%d", &metadata.cantidadRegistros);
  fscanf(metadataFile, "%d", &metadata.longitudRegistro);
  fclose(metadataFile);

  if (registroID < 0 || registroID >= metadata.cantidadRegistros)
  {
    printf("ID fuera de rango.\n");
    fclose(archivo);
    return;
  }

  fseek(archivo, registroID * (metadata.longitudRegistro + 3), SEEK_SET);
  int estado;
  fscanf(archivo, "%d", &estado);
  if (estado == 0)
  {
    printf("El registro está vacío.\n");
    fclose(archivo);
    return;
  }

  fseek(archivo, registroID * (metadata.longitudRegistro + 3) + 2, SEEK_SET);
  for (int i = 0; i < metadata.cantidadCampos; i++)
  {
    char valor[metadata.campos[i].longitud + 1];
    printf("Ingrese el nuevo valor para el campo '%s': ", metadata.campos[i].nombre);
    fgets(valor, sizeof(valor), stdin);
    valor[strcspn(valor, "\n")] = '\0';
    fprintf(archivo, "%-*s", metadata.campos[i].longitud, valor);
  }
  fprintf(archivo, "\n");
  fclose(archivo);
  printf("Registro modificado exitosamente.\n");
}

void leerArchivoEntero(char *nombreArchivo)
{
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL)
    {
        printf("No se pudo abrir el archivo para lectura.\n");
        return;
    }

    Metadata metadata;
    FILE *metadataFile = fopen(METADATA_FILENAME, "r");
    if (metadataFile == NULL)
    {
        printf("No se pudo abrir el archivo de metadata.\n");
        fclose(archivo);
        return;
    }

    fscanf(metadataFile, "%d", &metadata.cantidadCampos);
    for (int i = 0; i < metadata.cantidadCampos; i++)
    {
        fscanf(metadataFile, "%s %d", metadata.campos[i].nombre, &metadata.campos[i].longitud);
    }
    fscanf(metadataFile, "%d", &metadata.cantidadRegistros);
    fscanf(metadataFile, "%d", &metadata.longitudRegistro);
    fclose(metadataFile);

    printf("\n\nContenido del archivo:\n\n");
    for (int i = 0; i < metadata.cantidadRegistros; i++)
    {
        int estado;
        fscanf(archivo, "%d", &estado);
        printf("Registro #%d - Estado: %s\n", i, estado == 1 ? "Ocupado" : "Vacío");

        if (estado == 1)
        {
            for (int j = 0; j < metadata.cantidadCampos; j++)
            {
                char valor[metadata.campos[j].longitud + 1];
                fscanf(archivo, "%s", valor);
                printf("  %s: %s\n", metadata.campos[j].nombre, valor);
            }
        }
        else
        {
            fseek(archivo, metadata.longitudRegistro + 1, SEEK_CUR);
        }
    }
    printf("\n\n");

    fclose(archivo);
}

bool ValidarEntero(int *numero)
{
  char cadena[12];
  bool esValido = false;

  while (!esValido)
  {
    printf("Ingresar numero: ");
    fgets(cadena, 12, stdin);

    // Verificamos si es un número válido
    char *endPtr;
    *numero = strtol(cadena, &endPtr, 10);

    // Verificamos que toda la cadena sea un número y esté dentro del rango
    if (endPtr == cadena || *endPtr != '\n')
    {
      printf("ERROR: Ingresa un número válido.\n");
    }
    else
    {
      esValido = true;
    }
  }

  return esValido;
}

void manejarInterrupcion(int signum)
{
  printf("\nInterrupción detectada. Cerrando correctamente...\n");
  // Realiza limpieza aquí (cierra archivos, libera memoria, etc.)
  exit(signum);
}
