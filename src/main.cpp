#include <Arduino.h>
#include <ESP8266WiFi.h>        //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
#include <DHT.h>                // Biblioteca DHT
#include <SPI.h>                // SSD1396
#include <Wire.h>               // SSD1396
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>       // SSD1396
#include <Adafruit_SSD1306.h>   // SSD1396

/* Defines do projeto */
#define DHTPIN D5         // pin onde dht esta conectado
#define REED D0

#define DIAMETRO 125       // diametro interno do balde
#define VOLUME 4.5         // volume da bascula (em cm3) (1cm3 == 1mm)


#define SSID_REDE "" /* coloque aqui o nome da rede que se deseja conectar */
#define SENHA_REDE "" /* coloque aqui a senha da rede que se deseja conectar */
  
#define DHTTYPE DHT11 // DHT 11
 
/* constantes e variáveis globais */
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64);           			// Cria objeto display

/* prototypes */
void init_wifi(void);
void conecta_wifi(void);
void verifica_conexao_wifi(void);

float temperatura_lida = 0.0;
float umidade_lida = 0.0;

// Variáveis:
int val = 0;
int old_val = 0;
int REEDCOUNT = 0;

void init_wifi(void)
{
    Serial.println("------WI-FI -----");
    Serial.println("Conectando-se a rede: ");
    Serial.println(SSID_REDE);
    Serial.println("\nAguarde...");
 
    conecta_wifi();
}

void conecta_wifi(void)
{
    /* Se ja estiver conectado, nada é feito. */
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
     
    /* refaz a conexão */
    WiFi.begin(SSID_REDE, SENHA_REDE);
     
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
    }
 
    Serial.println("Conectado com sucesso a rede wi-fi \n");
    Serial.println(SSID_REDE);
}

void verifica_conexao_wifi(void)
{
    conecta_wifi();
}

void setup()
{
    Serial.begin(115200);

    /* Inicializa sensor de temperatura e umidade relativa do ar */
    dht.begin();

    /* Inicializa o display OLED */
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);      //INICIALIZA O DISPLAY COM ENDEREÇO I2C 0x3C
    display.setTextColor(WHITE);                    //DEFINE A COR DO TEXTO
    display.setTextSize(0);                         //DEFINE O TAMANHO DA FONTE DO TEXTO
    display.clearDisplay();                         //LIMPA AS INFORMAÇÕES DO DISPLAY
    display.dim(0);                                 //DEFINE BRILHO ('0' PARA MAXIMO E '1' PARA MINIMO)

    pinMode(REED, INPUT_PULLUP);
    /* Inicializa e conecta-se ao wi-fi */
    init_wifi();
}
 
void loop()
{
    /* Garante que a conexão wi-fi esteja ativa */
    verifica_conexao_wifi();

    temperatura_lida = dht.readTemperature();
    umidade_lida = dht.readHumidity();    

    bool val = digitalRead(REED);      // Lê o Status do Reed Switch

    if ((val == LOW && old_val == HIGH)) {    // Verefica se o Status mudou
        delay(10);                              // Atraso colocado para lidar com qualquer "salto" no switch.
        REEDCOUNT = REEDCOUNT + 1;              // Adiciona 1 à cntagem de pulsos
        old_val = val;                          //Iguala o valor antigo com o atual
    }
    else {
        old_val = val;               //If the status hasn't changed then do nothing
    }


    float area_recipiente = 3.1415 * DIAMETRO; // área da seção transversal do recipiente em cm²
    float volume_por_virada = (VOLUME/area_recipiente);  // altura lamina de agua no interior do balde por virada
    float volume_coletado = (REEDCOUNT * volume_por_virada); // volume total coletado em cm³

    display.clearDisplay();
    display.setCursor(10,10);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.print("T: ");                    //ESCREVE O TEXTO NO DISPLAY
    display.setCursor(20,10);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.println(temperatura_lida);
    display.setCursor(10,20);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.print("U: ");                    //ESCREVE O TEXTO NO DISPLAY
    display.setCursor(20,20);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.println(umidade_lida);
    display.display();
    display.setCursor(10,30);
    display.print("Viradas: ");
    display.setCursor(60,30);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.print(REEDCOUNT);
    display.display();
    display.setCursor(10,40);
    display.print("Chuva: ");
    display.setCursor(60,40);               //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
    display.print(volume_coletado);
    display.print(" mm");
    display.display();

}
