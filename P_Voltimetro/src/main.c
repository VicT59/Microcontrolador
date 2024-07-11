#include <stdio.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/ledc.h>

adc1_channel_t adc_pot = ADC1_CHANNEL_6;
// Configuración de otras inicializaciones necesarias

#define LED 2
//FUNCION INIT
void init_hw(void);

//Funcion
void app_main() {
    float target = 18;      // Voltaje targetetivo (ejemplo)
    float voltajeRMS;       // Variable para voltaje RMS (no utilizada en el código actual)
    float Vmedido;          // Voltaje medido
    float voltajes[100];    // Arreglo para almacenar voltajes medidos
    int i = 0;              // Contador

    // Inicialización del hardware
    init_hw();

    while (1) {
        // Medición de voltajes
        for (i = 0; i < 100; i++) {
            // Lectura del potenciómetro
            int pot = adc1_get_raw(adc_pot);
            // Cálculo del voltaje medido
            Vmedido = (pot * target / 4095.0);
            // Almacenamiento del voltaje medido en el arreglo
            voltajes[i] = Vmedido;

            // Impresión del voltaje medido
            printf("Voltaje%d: %fV\n", i, voltajes[i]);

            // Pequeña pausa
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        // Procesamiento de voltajes medidos
        for (size_t j = 1; j < 100; j++) {
            voltajes[j] = pow(voltajes[j], 2);   // Elevar al cuadrado
            voltajes[j] = voltajes[j] + voltajes[j - 1]; // Sumar al anterior
        }

        // Cálculo del voltaje RMS promedio
        float vtemp = (voltajes[99] / 100); // Promedio de los últimos 100 valores
        vtemp = sqrt(vtemp); // Calcular la raíz cuadrada para obtener el RMS

        // Impresión del voltaje RMS promedio
        printf("Voltaje RMS: %4.2fV\n", vtemp);

        // Pausa antes de repetir el bucle principal
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void init_hw(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adc_pot, ADC_ATTEN_DB_12);

    gpio_config_t io_config;
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pin_bit_mask = (1 << LED);
    io_config.pull_down_en = GPIO_PULLDOWN_ONLY;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);
}