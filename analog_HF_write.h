#ifndef ANALOG_HF_WRITE_H
#define ANALOG_HF_WRITE_H

#include <Arduino.h>

// Pinos suportados: 9, 10 (Timer1) e 3, 11 (Timer2)
// Pino 11: frequência fixa em ~62.5kHz, duty variável
// Pinos 9, 10: frequência e duty configuráveis
// Pino 3: frequência e duty configuráveis
void analog_HF_write(int pino, long frequenciaKHz, int dutyCycle);

#endif