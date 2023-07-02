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
int test;

int r;
int g;
int b;

// Variables pour les animations
int animationMode = 0;

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

    <h2>Couleur</h2>
    <input type="color" id="colorPicker" value="#000000" onchange="updateColor()">

    <h2>Animation</h2>
    <select id="animationSelect" onchange="updateAnimation()">
        <option value="0">Aucune</option>
        <option value="1">Animation 1</option>
        <option value="2">Animation 2</option>
    </select>

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
    </script>
</body>
</html>
)html";


// Fonction pour définir la couleur des LEDs
void setLedColor(int r, int g, int b) {
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
}

// Fonction pour définir l'animation des LEDs
void setAnimation(int mode) {
  animationMode = mode;
  // Ajoutez ici le code pour démarrer l'animation souhaitée

  switch (animationMode)
  {
  case 1:
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    //for(int j=0; j<= 50; j++){
      for (int i=0 ; i<= NUM_LEDS; i++){
        leds[i] = CRGB (r, g, b);
        FastLED.show();
        delay(10);
      }

      for (int i=NUM_LEDS ; i>= 0; i--){
        leds[i] = CRGB (0, 0, 0);
        FastLED.show();
        delay(10);
      }
    //}
    break;
  
  default:
    break;
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

// Route pour définir l'animation des LEDs
void handleAnimation() {
  int mode = server.arg("mode").toInt();
  setAnimation(mode);
  server.send(200, "text/plain", "Animation définie !");
}

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

  // Démarrage du serveur web
  server.begin();

  Serial.println("Serveur démarré !");
}

void loop() {
  // Gestion des requêtes du serveur web
  server.handleClient();

  // Ajoutez ici le code pour les animations du ruban LED
}