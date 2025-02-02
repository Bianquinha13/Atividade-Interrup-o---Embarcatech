#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

//Definindo o led vermelho.
#define led_pin_red 13

//Definindo o botão A e B.
#define BUTTON_A 5
#define BUTTON_B 6

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

// Variáveis para controle do LED vermelho piscando
uint32_t last_red_led_toggle_time = 0;
bool red_led_on = false;
#define RED_LED_TOGGLE_INTERVAL 150000 // Intervalo de 150ms (150000 microssegundos)

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

//mapeamento "zig-zag" na matriz
int getIndex(int x, int y){
	if(y%2 == 0){
		return 24-(y * 5 + x);
	}else{
		return 24-(y * 5 + (4-x));
	}
}

//função de piscar o led vermelho 5 vezes em 1s.
void toggle_red_led() {
    uint32_t current_time = time_us_32(); // Tempo atual em microssegundos
    if (current_time - last_red_led_toggle_time >= RED_LED_TOGGLE_INTERVAL) {
        // Alterna o estado do LED vermelho
        red_led_on = !red_led_on;
        gpio_put(led_pin_red, red_led_on);
        last_red_led_toggle_time = current_time; // Atualiza o tempo do último toggle
    }
}

//numeros que aparecem na matriz de led
void numero_zero(){
    int numero[5][5][3]={
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha,coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_um(){
    int numero[5][5][3]={
        {{255,255,255}, {255,255,255}, {0,255,0}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_dois(){
    int numero[5][5][3]={
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {255,255,255}, {255,255,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_tres(){
    int numero[5][5][3]={
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_quatro(){
    int numero[5][5][3]={
        {{255,255,255}, {0,255,0}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {0,255,0}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {0,255,0}, {0,255,0}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_cinco(){
    int numero[5][5][3]={
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {255,255,255}, {255,255,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_seis(){
    int numero[5][5][3]={
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,255,255}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_sete(){
    int numero[5][5][3]={
        {{255,255,255}, {0,255,0}, {0,255,0}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {0,255,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_oito(){
    int numero[5][5][3]={
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {255,255,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {255,255,255}, {0,0,255}, {255,255,255}},
        {{255,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}
void numero_nove(){
    int numero[5][5][3]={
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,0,0}, {255,0,0}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {255,0,0}, {255,255,255}},
        {{255,255,255}, {255,255,255}, {255,255,255}, {255,0,0}, {255,255,255}}};
    for(int linha = 0; linha<5; linha++){
            for(int coluna = 0; coluna <5; coluna++){
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, numero[coluna][linha][0],numero[coluna][linha][1],numero[coluna][linha][2]);
            }
        }
    npWrite();
}

void exibir_num(int numero){
  switch (numero){
  case 0:
    numero_zero();
    break;
  case 1:
    numero_um();
    break;
  case 2:
    numero_dois();
    break;
  case 3:
    numero_tres();
    break;
  case 4:
    numero_quatro();
    break;
  case 5:
    numero_cinco();
    break;
  case 6:
    numero_seis();
    break;
  case 7:
    numero_sete();
    break;
  case 8:
    numero_oito();
    break;
  case 9:
    numero_nove();
    break;
  default:
    numero_zero();
    break;
  }
  npWrite();
}


int main() {

  // Inicializa entradas e saídas.
  stdio_init_all();

  // Inicializa matriz de LEDs NeoPixel.
  npInit(LED_PIN);
  npClear();
  npWrite(); // Escreve os dados nos LEDs.

  // Iniciando o botão A.
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  // Iniciando o botão B.
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

  // Iniciando o Led vermelho.
  gpio_init(led_pin_red);
  gpio_set_dir(led_pin_red,GPIO_OUT);

  // Numero para percorrer os valores da matriz, decrementar ou acrescentar.
  int numero_atual = 0;

  // Executando o piscar led vermelho por 1s;
  

  while (true) {
      // Lê o estado do Botão A
        bool button_a_state = gpio_get(BUTTON_A);  // HIGH = solto, LOW = pressionado
      // Lê o estado do Botão B
        bool button_b_state = gpio_get(BUTTON_B);  // HIGH = solto, LOW = pressionado

      //Verifica se o botão A foi pressionado.
      if(button_a_state==0){
        numero_atual++;
        if(numero_atual>9){
            numero_atual=0; //caso o numero seja 10, ele irá para 0.
        }
        exibir_num(numero_atual);
        sleep_ms(300);
      }

      //Verifica se o botão B foi pressionado.
      if(button_b_state==0){
        numero_atual--;
        if(numero_atual<0){
            numero_atual=9;//caso o numero seja -1, ele irá para 9.
        }
        exibir_num(numero_atual);
        sleep_ms(300);
      }

      // Função para piscar o LED vermelho sem bloquear a execução
        toggle_red_led();

      // Pequeno delay para evitar leituras inconsistentes (debounce simples)
      sleep_ms(100);
  }
}
