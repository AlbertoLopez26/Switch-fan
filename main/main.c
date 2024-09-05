#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define firstSpeed 32
#define secondSpeed 33
#define thirdSpeed 25
#define ligth 26
#define speedButton 13
#define ligthButton 27

int fanState = 0b000;
uint8_t ligthState = 0;
TimerHandle_t xTimers;
int timerId =1;
bool readyToSwitchSpeed = 0;
bool readyToSwitchLigth = 0;

esp_err_t initializeGPIO();
esp_err_t initializeISR();
esp_err_t setTimer(void);
void speedChange();
void ligthToggle();

void readyTimerCallback(TimerHandle_t pxTimer)
{
    readyToSwitchSpeed = 1;
    readyToSwitchLigth = 1;
}


void app_main(void)
{
    initializeGPIO();
    initializeISR();
    setTimer();
    while(1){}
}

esp_err_t initializeGPIO()
{

    gpio_reset_pin(firstSpeed);
    gpio_reset_pin(secondSpeed);
    gpio_reset_pin(thirdSpeed);
    gpio_reset_pin(speedButton);
    gpio_reset_pin(ligthButton);
    gpio_reset_pin(ligth);
    gpio_set_direction(firstSpeed, GPIO_MODE_OUTPUT);
    gpio_set_direction(secondSpeed, GPIO_MODE_OUTPUT);
    gpio_set_direction(thirdSpeed, GPIO_MODE_OUTPUT);
    gpio_set_direction(speedButton, GPIO_MODE_INPUT);
    gpio_set_direction(speedButton, GPIO_MODE_INPUT);
    gpio_set_direction(ligth, GPIO_MODE_OUTPUT);

    gpio_set_pull_mode(ligthButton, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(speedButton, GPIO_PULLUP_ONLY);

    return ESP_OK;
}

esp_err_t initializeISR()
{
    gpio_config_t configurationSpeedButton;
    configurationSpeedButton.pin_bit_mask = (1ULL << speedButton);
    configurationSpeedButton.mode = GPIO_MODE_INPUT;
    configurationSpeedButton.pull_up_en = GPIO_PULLUP_ENABLE;
    configurationSpeedButton.pull_down_en = GPIO_PULLDOWN_DISABLE;
    configurationSpeedButton.intr_type = GPIO_INTR_NEGEDGE;

    gpio_config_t configurationLigthButton;
    configurationLigthButton.pin_bit_mask = (1ULL << ligthButton);
    configurationLigthButton.mode = GPIO_MODE_INPUT;
    configurationLigthButton.pull_up_en = GPIO_PULLUP_ENABLE;
    configurationLigthButton.pull_down_en = GPIO_PULLDOWN_DISABLE;
    configurationLigthButton.intr_type = GPIO_INTR_NEGEDGE;

    gpio_config(&configurationLigthButton);
    gpio_config(&configurationSpeedButton);
    gpio_install_isr_service(0);

    gpio_isr_handler_add(speedButton, speedChange, NULL);
    gpio_isr_handler_add(ligthButton, ligthToggle, NULL);

    return ESP_OK;
}

esp_err_t setTimer(void)
{
    printf("Timer initialization configuration\n");
    xTimers = xTimerCreate("Timer",         // Just a text name, not used by the kernel.
                           (pdMS_TO_TICKS(300)), // The timer period in ticks.
                           pdFALSE,          // The timers will auto-reload themselves when they expire.
                           (void *)timerId,       // Assign each timer a unique id equal to its array index.
                           readyTimerCallback   // Each timer calls the same callback when it expires.
    );

    if (xTimers == NULL)
    {
        // The timer was not created.
        printf("Timer not created\n");
    }
    else
    {
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if (xTimerStart(xTimers, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            printf("Timer not created\n");
        }
    }
    return ESP_OK;
}

void speedChange()
{
    if(readyToSwitchSpeed == 1)
    {
        switch (fanState)
        {
            case 0b000:
            fanState = 0b001;
            gpio_set_level(secondSpeed, 0);
            gpio_set_level(thirdSpeed, 0);
            gpio_set_level(firstSpeed, 1);
            break;
    
        case 0b001:
            fanState = 0b010;
            gpio_set_level(firstSpeed, 0);
            gpio_set_level(thirdSpeed, 0);
            gpio_set_level(secondSpeed, 1);
            break;

        case 0b010:
            fanState = 0b100;
            gpio_set_level(secondSpeed, 0);
            gpio_set_level(firstSpeed, 0);
            gpio_set_level(thirdSpeed, 1);
            break;

        case 0b100:
            fanState = 0b000;
            gpio_set_level(secondSpeed, 0);
            gpio_set_level(thirdSpeed, 0);
            gpio_set_level(firstSpeed, 0);
            break;

        default:
            fanState = 0b000;
            gpio_set_level(secondSpeed, 0);
            gpio_set_level(thirdSpeed, 0);
            gpio_set_level(firstSpeed, 0);
            break;
        }
        readyToSwitchSpeed = 0;
        xTimerReset(xTimers, 0); 
    }

    
}

void ligthToggle()
{
    if(readyToSwitchLigth == 1)
    {
        ligthState = !ligthState;
        gpio_set_level(ligth, ligthState);
        readyToSwitchLigth = 0;
        xTimerReset(xTimers, 0);
    }
    
}

