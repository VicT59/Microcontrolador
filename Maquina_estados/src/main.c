/*
PINES DE ENTRADA

LSC = PIN 13
LSA = PIN 12
FC  = PIN1 4
CC  = PIN 27
CA  = PIN 26

PINES DE SALIDA
LED_M   = PIN 4
LED_E   = PIN16
MC      = PIN 17
MA      = PIN 5
*/

#include <stdio.h> 
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h> 
#include <freertos/timers.h> 
#include "esp_log.h"                //Libreria para colocar un print sin evitar que el bucle condicional sea interferido.
#include "freertos/timers.h"

//definiciones de variables
static const char *tag = "Main";

#define TRUE 1
#define FALSE 0
/**************************************/
// Definición de los estados de la máquina de estados
#define ESTADO_INIT         0
#define ESTADO_ABRIENDO     1
#define ESTADO_CERRANDO     2
#define ESTADO_CERRADO      3
#define ESTADO_ABIERTO      4
#define ESTADO_EMERGENCIA   5
#define ESTADO_ERROR        6
#define ESTADO_ESPERA       7

// Definición de variables para el temporizador
TimerHandle_t xTimers;
int timerID = 1;
int INTERVALO = 50;

// Declaración de funciones
esp_err_t INTERRUPCION_50MS(void);
esp_err_t SET_TIMER(void);
void init_hw(void);

int Func_ESTADO_INIT();
int Func_ESTADO_ABRIENDO();
int Func_ESTADO_CERRANDO();
int Func_ESTADO_CERRADO();
int Func_ESTADO_ABIERTO();
int Func_ESTADO_EMERGENCIA();
int Func_ESTADO_ERROR();
int Func_ESTADO_ESPERA();

/*****************************************************/
// Variables globales para el estado actual y anterior
volatile int ESTADO_ACTUAL = ESTADO_INIT;
volatile int STATUS_NEXT = ESTADO_INIT;
volatile int ESTADO_ANTERIOR = ESTADO_INIT;
volatile unsigned int TimeCa = 0;

//Arreglos de struct para entradas
volatile struct INPUTS{
    unsigned int LSA: 1;
    unsigned int LSC: 1;
    unsigned int CA:  1;
    unsigned int CC:  1;
    unsigned int FC:  1;
}inputs;

//Arreglos de struct para salidas
volatile struct OUTPUTS{
    unsigned int MC: 1;
    unsigned int MA: 1;
    unsigned int LED_EMERGENCIA: 1;
    unsigned int LED_MOVIMIENTO: 1;
}outputs;


/***********************************************************************************************************/
// TimeHandle del temporizador
void vTimerCallback( TimerHandle_t pxTimer){

    ESP_LOGE(tag, "INTERRUPCION COMPLETADA.");
    INTERRUPCION_50MS();
}
//Funcion principal
void app_main() {

    STATUS_NEXT = Func_ESTADO_INIT();
    SET_TIMER();


    for (;;) //condicion de bucle para llamar las funcion por parte con ayuda de la variable "STATUS_NEXT"
    {    
        if (STATUS_NEXT == ESTADO_INIT)
        {
            STATUS_NEXT = Func_ESTADO_INIT();
        }
        if (STATUS_NEXT == ESTADO_ESPERA)
        {
            STATUS_NEXT = Func_ESTADO_ESPERA();
        }
        if (STATUS_NEXT == ESTADO_ABRIENDO)
        {
            STATUS_NEXT = Func_ESTADO_ABRIENDO();
        }
        if (STATUS_NEXT == ESTADO_CERRANDO)
        {
            STATUS_NEXT = Func_ESTADO_CERRANDO();
        }
        if (STATUS_NEXT == ESTADO_CERRADO)
        {
            STATUS_NEXT = Func_ESTADO_CERRADO();
        }
        if (STATUS_NEXT == ESTADO_ABIERTO)
        {
            STATUS_NEXT = Func_ESTADO_ABIERTO();
        }
        if (STATUS_NEXT == ESTADO_EMERGENCIA)
        {
            STATUS_NEXT = Func_ESTADO_EMERGENCIA();
        }
        if (STATUS_NEXT == ESTADO_ERROR)
        {
            STATUS_NEXT = Func_ESTADO_ERROR();
        }
        
    }

}


int Func_ESTADO_INIT(){

    ESP_LOGE(tag, "INICIANDO PROGRAMA");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INIT;
    ESP_LOGE(tag, "CONFIGURACION DE LOS PINES");
    
    init_hw();
    for (;;)
    {
        return ESTADO_ESPERA;
    }
}


int Func_ESTADO_ABRIENDO(){

    printf("ABRIENDO EL PUERTON");

    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABRIENDO;

    outputs.LED_MOVIMIENTO = TRUE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = TRUE;
    outputs.MC = FALSE;

    for (;;)
    {
        if(inputs.LSA == TRUE){

            return ESTADO_ABIERTO; 
        }
        if (inputs.LSA == TRUE && inputs.LSC == TRUE)
        {
            return ESTADO_ERROR;
        }
        if (inputs.FC == TRUE)
        {
            return ESTADO_EMERGENCIA;
        }
        if(inputs.CC == TRUE ){

            return ESTADO_CERRANDO;
        }
         
    }
}


int Func_ESTADO_CERRANDO(){

    printf("CERRANDO EL PUERTON");

    
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRANDO;

    outputs.LED_MOVIMIENTO = TRUE;
    outputs.LED_EMERGENCIA = FALSE;

    outputs.MA = FALSE;
    outputs.MC = TRUE;

    for (;;)
    {
        if (inputs.LSC == TRUE )
        {
            return ESTADO_CERRADO;
        }

        if (inputs.LSA == TRUE && inputs.LSC == TRUE)
        {
            return ESTADO_ERROR;
        }
        if (inputs.FC == TRUE)
        {
            return ESTADO_EMERGENCIA;
        }
        if (inputs.CA == TRUE)
        {
            return ESTADO_ABRIENDO;
        }
            
        }
}


int Func_ESTADO_CERRADO() {
    printf("PUERTON ESTA CERRADO");

    // Actualizar estados
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRADO;

    // Configurar salidas
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    // Bucle de espera
    for (;;) {
        return ESTADO_ESPERA;
    }
}


int Func_ESTADO_ABIERTO() {
    printf("PUERTON ESTA ABIERTO");

    // Actualizar estados
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABIERTO;

    // Configurar salidas
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    // Bucle de espera
    for (;;) {
        return ESTADO_ESPERA;
    }
}

int Func_ESTADO_EMERGENCIA() {
    printf("ALERTA!");

    // Actualizar estados
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_EMERGENCIA;

    // Configurar salidas
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = TRUE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    // Bucle de parpadeo del LED de emergencia
    for (;;) {
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Encender por 0.5 segundos
        outputs.LED_EMERGENCIA = FALSE;
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Apagar por 0.5 segundos
        outputs.LED_EMERGENCIA = TRUE;
    }
}


int Func_ESTADO_ERROR() {
    printf("\nERROR!\nERROR!\nERROR!");
    
    // Actualizar estados
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ERROR;

    // Configurar salidas
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.LED_EMERGENCIA = TRUE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    // Bucle de parpadeo de LED de emergencia
    for (;;) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        outputs.LED_EMERGENCIA = FALSE;
        vTaskDelay(500 / portTICK_PERIOD_MS);
        outputs.LED_EMERGENCIA = TRUE;
        vTaskDelay(500 / portTICK_PERIOD_MS);

        return ESTADO_ESPERA;
    }
}


// Función para manejar el estado de espera
int Func_ESTADO_ESPERA() {
    ESP_LOGE(tag, "ESTADO ESPERA");

    // Actualizar estados
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ESPERA;

    // Configurar salidas
    outputs.LED_EMERGENCIA = FALSE;
    outputs.LED_MOVIMIENTO = FALSE;
    outputs.MA = FALSE;
    outputs.MC = FALSE;

    // Bucle de espera hasta que se detecte una acción
    for (;;) {
        if (inputs.LSA == FALSE && inputs.FC == FALSE && inputs.LSC == FALSE) {
            return ESTADO_CERRANDO;
        }
        if (inputs.CA == TRUE && inputs.FC == FALSE && inputs.LSA == FALSE && inputs.FC == FALSE) {
            return ESTADO_ABRIENDO;
        }
        if (inputs.CC == TRUE && inputs.FC == FALSE) {
            return ESTADO_CERRANDO;
        }
        if (inputs.CA == TRUE && inputs.FC == FALSE) {
            return ESTADO_ABRIENDO;
        }
        if (inputs.FC == TRUE) {
            return ESTADO_EMERGENCIA;
        }
        if (inputs.LSA == TRUE && inputs.LSC == TRUE) {
            return ESTADO_ERROR;
        }
    }

    // Retardo de 3 segundos antes de cambiar de estado
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    return ESTADO_CERRANDO; // Ejemplo de estado a retornar después del retardo
}


// Función para configurar y activar un timer
esp_err_t SET_TIMER(void) {
    printf("Inicializando configuracion del timer...");

    // Crear timer
    xTimers = xTimerCreate("Timer",
                          pdMS_TO_TICKS(INTERVALO),
                          pdTRUE,
                          (void *)timerID,
                          vTimerCallback
    );

    // Verificar si se creó correctamente
    if (xTimers == NULL) {
        printf("El timer no fue creado");
    } else {
        // Iniciar el timer
        if (xTimerStart(xTimers, 0) != pdPASS) {
            printf("El timer podria no ser seteado en el estado activo");
        }
    }

    return ESP_OK;
}

// Función para manejar una interrupción cada 50ms
esp_err_t INTERRUPCION_50MS(void) {
    // Leer estados de los pines de entrada
    inputs.LSC = (gpio_get_level(13) == TRUE) ? TRUE : FALSE;
    inputs.LSA = (gpio_get_level(12) == TRUE) ? TRUE : FALSE;
    inputs.FC = (gpio_get_level(14) == TRUE) ? TRUE : FALSE;
    inputs.CC = (gpio_get_level(27) == TRUE) ? TRUE : FALSE;
    inputs.CA = (gpio_get_level(26) == TRUE) ? TRUE : FALSE;

    // Configurar estados de los pines de salida
    gpio_set_level(4, outputs.LED_MOVIMIENTO);
    gpio_set_level(16, outputs.LED_EMERGENCIA);
    gpio_set_level(17, outputs.MC);
    gpio_set_level(5, outputs.MA);

    return ESP_OK;
}

void init_hw(void){

    gpio_config_t IO_CONFIG;
    IO_CONFIG.mode = GPIO_MODE_INPUT;
    IO_CONFIG.pin_bit_mask = (1 << 32) | (1 << 33) | (1 << 25) | (1 << 26) | (1 << 27);
    IO_CONFIG.pull_down_en = GPIO_PULLDOWN_ENABLE;
    IO_CONFIG.pull_up_en = GPIO_PULLUP_DISABLE;
    IO_CONFIG.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&IO_CONFIG);

    IO_CONFIG.mode = GPIO_MODE_OUTPUT;
    IO_CONFIG.pin_bit_mask = (1 << 2) | (1 << 4) | (1 << 16) | (1 << 17);
    IO_CONFIG.pull_down_en = GPIO_PULLDOWN_DISABLE;
    IO_CONFIG.pull_up_en = GPIO_PULLUP_DISABLE;
    IO_CONFIG.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&IO_CONFIG);

}