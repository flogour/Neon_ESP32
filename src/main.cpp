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
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Neon Control</title>
    <link rel="stylesheet" type="text/css" href="style.css">
    <link rel="icon" href="./img/logo.jpg">
</head>
<body>
    <header id="header">
        <div>
            <a href="#home"><img src="./img/titleWhite.png" alt="Max & Flo"></a>
            <label class="switch">
                <input type="checkbox">
                <span class="slider round"></span>
            </label>
        </div>
        <nav>
            <ul>
                <li><a href="#home">HOME</a></li>
                <li><a href="#lights">LIGHTS</a></li>
                <li><a href="#timer">TIMER</a></li>
            </ul>
        </nav>
    </header>
    <div class="page1" id="home">
        <div class="neonControl">
            <h1>NEON</h1>
            <h2>CONTROL</h2>
        </div>
        <div class="luminosity">
            <h3>LUMINOSITY</h3>
            <div>
                <input type="range" min="0" max="100" value="50" step="1" id="slider">
                <div id="number" class="neonTextWhite">50</div>
            </div>
        </div>
    </div>
    <div class="page2" id="lights">
        page 2
    </div>
    <div class="transition">TIMER</div>
    <div class="page3" id="timer">
        page 3
    </div>
    <footer>
        <h3>Website developed by Max & flo</h3>
        <p>Neon Control - v1</p>
    </footer>
    <script>
        var slider = document.getElementById("slider");
        var number = document.getElementById("number");
        slider.oninput=function(){number.innerHTML = slider.value;}

        var currentScrollPos = window.pageYOffset;

        document.addEventListener("scroll", (event) => {
            currentScrollPos = window.scrollY;
            if (currentScrollPos > (Math.round(window.innerHeight)*0.85)) {
                document.getElementById("header").style.transition = "all 0.5s ease";
                document.getElementById("header").style.backgroundColor = "rgba(0, 0, 0, 0.95)";
                // document.getElementById("header").style.backgroundColor = "rgba(36, 16, 88, 0.95)";
                // document.getElementById("header").style.borderRadius = "0 0 25px 25px";
            } else {
                document.getElementById("header").style.transition = "all 0.5s ease";
                document.getElementById("header").style.backgroundColor = "rgba(36, 16, 88, 0)";
            }
        });
    </script>
</body>
</html>
)html";

const char* index_css = R"css(
  /*------STYLE------*/

/*------VARIABLES------*/
:root {
    /* nav */
    --nav-logo-width: 20%;
    --nav-bar-width: 70%;
    --nav-padding: 1.5%;
    --red:#F43545;
    --orange: #FF8901;
    --yellow: #FAD717;
    --green: #00BA71;
    --cyan: #00C2DE;
    --blue: #00418D;
    --pink: #ff3ae1;
    --purple: #5F2879;
}
/*------ALL FILE------*/
* {
    margin: 0;
    padding: 0;
    font-family: Tahoma, sans-serif;
    color: white;
    transition: all 0.2s ease;
    scroll-behavior: smooth;
}
body {
    /* background-color:rgba(0, 0, 0, 0.7); */
}
.neonTextBlue {
    color: #fff;
    text-shadow:
        0 0 7px #00c8ff,
        0 0 10px #00c8ff,
        0 0 21px #00c8ff,
        0 0 42px #00c8ff,
        0 0 82px #00c8ff,
        0 0 92px #00c8ff,
        0 0 102px #00c8ff,
        0 0 151px #00c8ff;
}.neonTextWhite {
    color: #fff;
    text-shadow:
        0 0 7px #fff,
        0 0 10px #fff,
        0 0 21px #fff,
        0 0 42px #fff,
        0 0 82px #fff,
        0 0 92px #fff,
        0 0 102px #000,
        0 0 151px #000;
}.neonTextPink {
    color: #fff;
    text-shadow:
        0 0 7px #ea00ff,
        0 0 10px #ea00ff,
        0 0 21px #ea00ff,
        0 0 42px #ea00ff,
        0 0 82px #ea00ff,
        0 0 92px #ea00ff,
        0 0 102px #ea00ff,
        0 0 151px #ea00ff;
}
header {
    position: fixed;
    top: 0px;
    width: 100%;
    display: flex;
    flex-wrap: wrap;
    padding-top: var(--nav-padding);
    padding-bottom: var(--nav-padding);
    justify-content: space-around;
} header a { /* LOGO */
    text-decoration: none;
} header a img {
    width: 220px;
} header div {
    display: flex;
    flex-direction: column;
    align-items: center;
}
/* The switch - the box around the slider */
.switch {
    position: relative;
    display: inline-block;
    width: 120px;
    height: 34px;
    margin-left: 20px;
}
/* Hide default HTML checkbox */
.switch input {
    opacity: 0;
    width: 0;
    height: 0;
}
/* The slider */
.slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: rgba(255, 255, 255, 0.15);
    -webkit-transition: .4s;
    transition: .4s;
    border-radius: 34px;
}
.slider:before {
    position: absolute;
    content: "";
    height: 26px;
    width: 26px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    -webkit-transition: .4s;
    transition: .4s;
    border-radius: 50%;
}
input:checked + .slider {
    background-color: var(--cyan);
    box-shadow: 0px 0px 40px 0px rgba(0, 194, 222, 1);
}
input:focus + .slider {
    /* box-shadow: 0 0 1px var(--cyan); */
}
input:checked + .slider:before {
    -webkit-transform: translateX(85px);
    -ms-transform: translateX(85px);
    transform: translateX(85px);
}
nav { /* NAVBAR BOX */
    width: var(--nav-bar-width);
}nav ul{ 
    display: flex;
    flex-wrap: wrap;
    justify-content: space-around;
}nav ul li{
    list-style: none;
    padding-top: 15px;
}nav ul li:hover {
    text-shadow:
        0 0 7px #fff,
        0 0 10px #fff,
        0 0 21px #fff,
        0 0 42px #fff,
        0 0 82px #fff,
        0 0 92px #fff,
        0 0 102px #000,
        0 0 151px #000;
}nav ul li a {
    font-size: 18px;
    letter-spacing: 8px;
}
/*------PAGE 1------*/
.page1 {
    height: 100vh;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(../img/wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-evenly;
    /* justify-content: space-evenly; */
    /* border: 2px solid rgb(200, 23, 23); */
}.page1 .neonControl {
    display: flex;
    align-items: center;
    /* border: 2px solid black; */
}.page1 h1 {
    font-size: 1580%;
    /* border: 1px solid black; */
    word-break: break-all;
    text-align: center;
    letter-spacing: 5px;
    display: inline-block;
    line-height: 200px;
    width: 50%;
}.page1 h2 {
    font-size: 380%;
    letter-spacing: 16px;
    display: inline;
    writing-mode: vertical-lr;
    padding-top: 3%;
    /* border: 1px solid black; */
}.page1 h3 {
    font-size: 260%;
    letter-spacing: 16px;
    display: inline;
    writing-mode: vertical-lr;
    margin-right: 20%;
    /* border: 2px solid rgb(181, 41, 41); */
}.page1 .luminosity {
    display: flex;
    /* width: 200px; */
    align-items: center;
    text-align: center;
    /* border: 2px solid rgb(181, 41, 41); */
}#number {
    width: 50px;
    font-size: 150%;
    margin-top: 20px;
    text-align: center;
}input[type="range"]{
    -webkit-appearance: slider-vertical;
    height: 350px;
    width: 0.1px;
    border: 2px solid rgb(200, 23, 23);
}

/*------PAGE 2------*/
.page2 {
    height: 100vh;
    /* border: 2px solid rgb(200, 23, 23); */
    /* box-shadow: 0px -20px 40px rgba(0, 0, 0, 0.5); */
}

/*------TRANSITION------*/
.transition {
    height: 30vh;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(../img/wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 380%;
    letter-spacing: 20px;
    /* border: 2px solid rgb(200, 23, 23); */
    /* box-shadow: 0px 20px -40px rgba(0, 0, 0, 0.5); */
}

/*------PAGE 3------*/
.page3 {
    height: 100vh;
    /* border: 2px solid rgb(200, 23, 23); */
}

/*------FOOTER------*/
footer {
    height: 25vh;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(../img/wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    font-style: italic;
    /* font-weight: lighter; */
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    /* border: 2px solid rgb(200, 23, 23); */
}
)css";

void handleRoot();
void handleCss();
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
  server.on("/style.css", handleCss);
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
  if(extinction == 1 && temps2 >= (temps1+900000)){
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    FastLED.show();
    extinction = 0;
  }
}






void handleRoot() {
  String htmlContent = String(index_html);
  server.send(200, "text/html", htmlContent);
}
void handleCss(){
  String cssContent = String(index_css);
  server.send(200, "text/css", cssContent);
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