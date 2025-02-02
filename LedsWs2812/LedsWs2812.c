#include <stdio.h>
#include "lib/numeros.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

#define DEBOUNCE_LINE 300000 // 300ms em microsegundos
#define NUMERO_DE_LEDS 25    // Numero de LED's na matriz
#define BTN_A 5              // Pino do botao A
#define BTN_B 6              // Pino do botao B
#define OUT_PINO 7           // Pino da matriz de LED's
#define GREEN_PINO 11        // Pino do LED verde do RGB
#define BLUE_PINO 12         // Pino do LED azul do RGB
#define RED_PINO 13          // Pino do LED vermelho do RGB

volatile int contador = 0;       // Contador de controle dos estado de numeros atual
volatile int btn_a_acionado = 0; // Variavel de controle do 'botao a' pressioando
volatile int btn_b_acionado = 0; // Variavel de controle do 'botao b' pressioando

// Funcao responsavel por manter o led vermelho piscando duas vezes por segundo
void pisca_led_rgb()
{
    gpio_put(RED_PINO, true);
    sleep_ms(100);
    gpio_put(RED_PINO, false);
    sleep_ms(100);
}

// Função de callback para interrupção (IRQ) que detecta e controla os botões pressionados
void button_callback(uint gpio, uint32_t events)
{
    // Captura do tempo atual em microsegundos numa variável uint
    uint32_t marco = time_us_32();
    // Utilização de debounce controlado por software para garantir que o botão pressionado execute a instrução apenas uma vez
    if (gpio == BTN_A && (marco - btn_a_acionado) > DEBOUNCE_LINE)
    {

        btn_a_acionado = marco;
        contador++;
        printf("Contador: %d.\n", contador); // Comunicação serial sobre o estado atual do contador
    }
    else if (gpio == BTN_B && (marco - btn_b_acionado) > DEBOUNCE_LINE)
    {

        btn_b_acionado = marco;
        contador--;
        printf("Contador: %d.\n", contador); // Comunicação serial sobre o estado atual do contador
    }
}

// Função responsável por converter valores RGB em um único valor de 32 bits para a matriz de LEDs
uint32_t matrix_rgb(double r, double g, double b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função responsável por exibir o desenho matricial passado como parâmetro nos LEDs
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUMERO_DE_LEDS; i++)
    {
        valor_led = matrix_rgb(desenho[24 - i], 0.0, 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

int main()
{

    PIO pio = pio0;
    uint32_t valor_led = 0;
    double r = 0.0, b = 0.0, g = 0.0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PINO);
    set_sys_clock_khz(128000, false);

    stdio_init_all();
    // Inicialização dos pinos do LED RGB
    gpio_init(RED_PINO);
    gpio_init(GREEN_PINO);
    gpio_init(BLUE_PINO);
    // Configuração dos pinos do LED RGB
    gpio_set_dir(RED_PINO, GPIO_OUT);
    gpio_set_dir(GREEN_PINO, GPIO_OUT);
    gpio_set_dir(BLUE_PINO, GPIO_OUT);
    // Exibe o desenho inicial no começo da execução
    desenho_pio(frame_0, valor_led, pio, sm, r, g, b);
    // Inicialização dos botões 'A' e 'B'
    gpio_init(BTN_A);
    gpio_init(BTN_B);
    // Configuração dos botões 'A' e 'B'
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_set_dir(BTN_B, GPIO_IN);
    // Habilitação dos resistores pull-up via software para os botões 'A' e 'B'
    gpio_pull_up(BTN_A);
    gpio_pull_up(BTN_B);
    // Configuração das interrupções para os botões 'A' e 'B'
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (true)
    {

        pisca_led_rgb();

        switch (contador)
        {
        case 0:
            desenho_pio(frame_0, valor_led, pio, sm, r, g, b);
            break;
        case 1:
            desenho_pio(frame_1, valor_led, pio, sm, r, g, b);
            break;
        case 2:
            desenho_pio(frame_2, valor_led, pio, sm, r, g, b);
            break;
        case 3:
            desenho_pio(frame_3, valor_led, pio, sm, r, g, b);
            break;
        case 4:
            desenho_pio(frame_4, valor_led, pio, sm, r, g, b);
            break;
        case 5:
            desenho_pio(frame_5, valor_led, pio, sm, r, g, b);
            break;
        case 6:
            desenho_pio(frame_6, valor_led, pio, sm, r, g, b);
            break;
        case 7:
            desenho_pio(frame_7, valor_led, pio, sm, r, g, b);
            break;
        case 8:
            desenho_pio(frame_8, valor_led, pio, sm, r, g, b);
            break;
        case 9:
            desenho_pio(frame_9, valor_led, pio, sm, r, g, b);
            break;
        default:
            desenho_pio(frame_0, valor_led, pio, sm, r, g, b);
            break;
        }

        sleep_ms(100);
        // Controle de limites para ciclar os números quando ultrapassam os valores permitidos
        if (contador > 9)
        {
            contador = 0;
        }
        if (contador < 0)
        {
            contador = 9;
        }
    }
}