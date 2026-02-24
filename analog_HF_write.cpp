#include "analog_HF_write.h"

void analog_HF_write(int pino, long frequenciaKHz = 0, int dutyCycle = 0); {
  long frequencia = frequenciaKHz * 1000L;
  dutyCycle = constrain(dutyCycle, 0, 100);

  if (pino == 9 || pino == 10) {
    TCCR1A = 0;
    TCCR1B = 0;

    long icr_value = (16000000L / (1 * frequencia)) - 1;
    if (icr_value >= 100 && icr_value <= 65535) {
      TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);  // Prescaler = 1
    } else {
      icr_value = (16000000L / (8 * frequencia)) - 1;
      if (icr_value >= 100 && icr_value <= 65535) {
        TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);  // Prescaler = 8
      } else {
        icr_value = (16000000L / (64 * frequencia)) - 1;
        TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);  // Prescaler = 64
      }
    }

    ICR1 = icr_value;

    if (pino == 9) {
      TCCR1A = _BV(COM1A1) | _BV(WGM11);  // Non-inverting mode pino 9
      OCR1A = map(dutyCycle, 0, 100, 0, ICR1);
    } else {
      TCCR1A = _BV(COM1B1) | _BV(WGM11);  // Non-inverting mode pino 10
      OCR1B = map(dutyCycle, 0, 100, 0, ICR1);
    }
  }

  else if (pino == 3 || pino == 11) {
    TCCR2A = 0;
    TCCR2B = 0;

    if (pino == 3) {
      uint32_t top;
      uint8_t prescalerBits;

      if ((top = F_CPU / (frequencia * 1)) <= 256) prescalerBits = _BV(CS20);
      else if ((top = F_CPU / (frequencia * 8)) <= 256) prescalerBits = _BV(CS21);
      else if ((top = F_CPU / (frequencia * 32)) <= 256) prescalerBits = _BV(CS21) | _BV(CS20);
      else if ((top = F_CPU / (frequencia * 64)) <= 256) prescalerBits = _BV(CS22);
      else if ((top = F_CPU / (frequencia * 128)) <= 256) prescalerBits = _BV(CS22) | _BV(CS20);
      else if ((top = F_CPU / (frequencia * 256)) <= 256) prescalerBits = _BV(CS22) | _BV(CS21);
      else {
        top = 256;
        prescalerBits = _BV(CS22) | _BV(CS21) | _BV(CS20);
      }

      OCR2A = top - 1;
      OCR2B = (OCR2A * dutyCycle) / 100;
      TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
      TCCR2B = _BV(WGM22) | prescalerBits;
    } else {

      OCR2A = map(dutyCycle, 0, 100, 0, 255);
      TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
      TCCR2B = _BV(CS20);  // prescaler = 1 → ~62.5 kHz
    }
  }
}