#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
//#include <Arduino.h>

// Paramètres du réseau Wi-Fi
const char* ssid = "Livebox-B8C0";
const char* password = "tAdwttJtCdKjhiZaY6";

// Paramètres du ruban LED
#define LED_PIN 27  // Broche de contrôle du ruban LED
#define NUM_LEDS 222  // Nombre de LEDs dans le ruban
CRGB leds[NUM_LEDS];

// Création de l'objet serveur web
WebServer server(80);

// Variables pour la couleur des LEDs
int red = 0;
int green = 0;
int blue = 0;

int r;
int g;
int b;

// Variables pour les animations
int animationMode = 0;
int brightness = 255;
int extinction;
double temps1 = 0;
double temps2 = 0;

// Déclaration de la variable contenant le code HTML
const char* index_html = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Contrôle du ruban LED</title>
</head>
<body>
    <h1>Contrôle du ruban LED</h1>

    <h3>Couleur</h3>
    <input type="color" id="colorPicker" value="#000000" onchange="updateColor()">

    <h3>Animation</h3>
    <select id="animationSelect" onchange="updateAnimation()">
        <option value="0">Aucune</option>
        <option value="1">Animation 1</option>
        <option value="2">Animation 2</option>
        <option value="3">Animation 3</option>
        <option value="4">Animation 4</option>
        <option value="5">Animation 5</option>
        <option value="6">Animation 6</option>
    </select>

    <h3>Luminosite</h3>
    <form>
    <label for='brightness'>Luminosité:</label>
    <!-- <input type='range' id='brightness' min='0' max='255' value='%d'><br> -->
    <input type='range' onclick='setBrightness()' id='brightness' min='0' max='255' value='%d'><br>
    <!-- <button type='button' onclick='setBrightness()'>Définir la luminosité</button> -->
    </form>

    <h3>Extinction auto</h3>
    <button type='button' onclick="eteindre()"> 10 min </button>

    <script>
        function updateColor() {
            var color = document.getElementById("colorPicker").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateAnimation() {
            var mode = document.getElementById("animationSelect").value;
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/animation?mode=" + mode, true);
            xhr.send();
        }

        function setBrightness(){
          var brightness = document.getElementById('brightness').value;
          var xhr = new XMLHttpRequest();
          xhr.open("GET", '/brightness?brightness=' + brightness, true);
          xhr.send();
        }

        function eteindre(){
            var xhr = new XMLHttpRequest();
            xhr.open("GET", '/eteindre?appuyer=1', true);
            xhr.send();
        }
    </script>
</body>
</html>
)html";



void handleRoot();
void handleColor();
void setLedColor(int r, int g, int b);
void handleAnimation();
void setBrightness();
void setAnimation(int mode);
void set_extinction();
void pride();
void va_et_vient();
void arc_en_ciel();



void setup() {
  // Connexion au réseau Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }

  // Configuration du ruban LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.show();

  // Routes du serveur web
  server.on("/", handleRoot);
  server.on("/color", handleColor);
  server.on("/animation", handleAnimation);
  server.on("/brightness", setBrightness);
  server.on("/eteindre", set_extinction);

  // Démarrage du serveur web
  server.begin();

  Serial.println("Serveur démarré !");
}


void loop() {
  // Gestion des requêtes du serveur web
  server.handleClient();

  // Ajoutez ici le code pour les animations du ruban LED
  temps2 = millis();
  if(extinction == 1 && temps2 >= (temps1+60000)){
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    FastLED.show();
    extinction = 0;
  }
}






void handleRoot() {
  String htmlContent = String(index_html);
  server.send(200, "text/html", htmlContent);
}

// Route pour définir la couleur des LEDs
void handleColor() {
  r = server.arg("r").toInt();
  g = server.arg("g").toInt();
  b = server.arg("b").toInt();
  setLedColor(r, g, b);
  server.send(200, "text/plain", "Couleur définie !");
}

// Fonction pour définir la couleur des LEDs
void setLedColor(int r, int g, int b) {
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
}

// Route pour définir l'animation des LEDs
void handleAnimation() {
  int mode = server.arg("mode").toInt();
  setAnimation(mode);
  server.send(200, "text/plain", "Animation définie !");
}

void setBrightness(){
  brightness = server.arg("brightness").toInt();
  FastLED.setBrightness(brightness);
  FastLED.show();
  server.send(200, "text/plain", "Luminosité définies !");
}

// Fonction pour définir l'animation des LEDs
void setAnimation(int mode) {
  animationMode = mode;
  // Ajoutez ici le code pour démarrer l'animation souhaitée

  switch (animationMode)
  {
    case 1:
      fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
      va_et_vient();
      fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
      FastLED.show();
      break;
    
    case 2:
      arc_en_ciel();
      FastLED.show();
      break;
    
    case 3:
      pride();
      FastLED.show();
      break;
    
    default:
      break;
  }
}

void set_extinction(){
  extinction = server.arg("appuyer").toInt();
  temps1 = millis();
}

void arc_en_ciel(){
  
}

void va_et_vient(){
  for(int j=0; j<= 3; j++){
    for (int i=0 ; i<= NUM_LEDS; i++){
      leds[i] = CRGB (r, g, b);
      FastLED.show();
      delay(10);
      server.handleClient();
    }

    for (int i=NUM_LEDS ; i>= 0; i--){
      leds[i] = CRGB (0, 0, 0);
      FastLED.show();
      delay(10);
      server.handleClient();
    }
    server.handleClient();
  }
}

void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}