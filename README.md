# analog_HF_write

Biblioteca para geração de PWM em **alta frequência** no Arduino Uno/Nano, com controle independente de frequência e duty cycle usando acesso direto aos registradores dos **Timer 1** e **Timer 2**.

## Estrutura de pastas

```
MeuProjeto/
├── MeuProjeto.ino
├── analog_HF_write.h
└── analog_HF_write.cpp
```

## Uso

```cpp
#include "analog_HF_write.h"

int pwmPins[] = {9, 10, 3, 11};
const int pwmSize = sizeof(pwmPins) / sizeof(pwmPins[0]);


void setup() {
  for (int i = 0; i < pwmSize; i++) {
    pinMode(pwmPins[i], OUTPUT);
  }
}

void loop() {
  analog_HF_write(9,  25, 50);  // 25kHz, 50% duty
  analog_HF_write(10, 25, 75);  // 25kHz, 75% duty
  analog_HF_write(3,  25, 30);  // 25kHz, 30% duty
  analog_HF_write(11, 60);  // ~62.5kHz fixo, 60% duty (frequência ignorada)
}
```

## Assinatura

```cpp
void analog_HF_write(int pino, long frequenciaKHz, int dutyCycle);
```

| Parâmetro      | Tipo   | Descrição                              |
|----------------|--------|----------------------------------------|
| `pino`         | int    | Pino de saída (9, 10, 3 ou 11)         |
| `frequenciaKHz`| long   | Frequência desejada em **kHz**         |
| `dutyCycle`    | int    | Duty cycle de 0 a 100 (%)              |

## Pinos suportados e timers utilizados

| Pino | Timer  | Frequência         | Duty cycle   |
|------|--------|--------------------|--------------|
| 9    | Timer1 | Configurável       | Configurável |
| 10   | Timer1 | Configurável       | Configurável |
| 3    | Timer2 | Configurável       | Configurável |
| 11   | Timer2 | Fixa ~62.5kHz      | Configurável |

## Limitações do pino 11

O pino 11 usa o canal **OC2A** do Timer 2. No Timer 2, o registrador **OCR2A** tem dupla função dependendo do modo de operação:

- No **modo 7** (Fast PWM, TOP = OCR2A) — usado pelo pino 3 — OCR2A define o TOP, ou seja, a frequência. Nesse modo OCR2A não pode controlar o duty do pino 11 ao mesmo tempo.
- Para liberar OCR2A para controle de duty no pino 11, é necessário usar o **modo 3** (Fast PWM, TOP = 0xFF), onde o TOP é fixo em 255.

Com TOP fixo em 255 e clock de 16MHz, a frequência fica determinada exclusivamente pelo prescaler, e **não existe prescaler que resulte exatamente em 25kHz**. As frequências disponíveis são:

| Prescaler | Frequência resultante |
|-----------|-----------------------|
| 1         | ~62.5 kHz             |
| 8         | ~7.8 kHz              |
| 32        | ~1.96 kHz             |
| 64        | ~980 Hz               |
| 128       | ~490 Hz               |
| 256       | ~245 Hz               |

Por isso, **o pino 11 opera com frequência fixa em ~62.5kHz**. O parâmetro `frequenciaKHz` é ignorado para esse pino. Apenas o duty cycle é configurável.

### Alternativas caso precise de frequência configurável no pino 11

- **Frequência fixa em 50% de duty:** usar modo CTC com toggle (`COM2A0`), que permite frequência configurável mas duty sempre fixo em 50%.

## Observações

- Chamar a função para pinos do mesmo timer (ex: 3 e 11) reconfigura o timer inteiro. **Não use pinos do mesmo timer simultaneamente.**
- Pinos 9 e 10 compartilham o Timer 1 e podem operar simultaneamente **com a mesma frequência**, pois ICR1 é compartilhado.
- O duty cycle é automaticamente limitado ao intervalo 0–100.
- `pinMode(pino, OUTPUT)` deve ser chamado no `setup()` antes de usar a função.