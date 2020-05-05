#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include <esp_types.h>
#include <esp_task.h>
#include <freertos/portmacro.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "time_component.h"

#define INPUT_BUFFER_SIZE 256

char input_buffer[INPUT_BUFFER_SIZE];
int input_buffer_index = 0;

void handle_input() {
    if (match_set_timestamp_template(input_buffer)) {
        printf("Setting local time to %s\n", input_buffer);
        time_set(input_buffer);
    } else {
        printf("Unable to handle input %s\n", input_buffer);
        printf("Commands Available\n");

        // Print time help menu
        printf("\tTime:\n");
        printf("\t  %s\n", SET_TIMESTAMP_TEMPLATE);
        printf("\t  %s\n", SET_TIMESTAMP_SIMPLE_TEMPLATE);
    }
}

void input_check() {
    int inputChar = fgetc(stdin);

    if (inputChar != EOF && (isprint(inputChar) || inputChar == '\n')) {
        if (inputChar == '\n' || input_buffer_index == INPUT_BUFFER_SIZE - 1) {
            handle_input();
            memset(input_buffer, '\0', INPUT_BUFFER_SIZE);
            input_buffer_index = 0;
        } else {
            input_buffer[input_buffer_index] = inputChar;
            input_buffer[input_buffer_index + 1] = '\0';
            input_buffer_index++;
        }
    }
}

_Noreturn
void input_loop() {
    while (true) {
        input_check();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

#endif // INPUT_COMPONENT_H
