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
            <a href="#home"><img src="data:image/png;base64,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" alt="Max & Flo"></a>
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

  /*server.on("/color", handleColor);
  server.on("/animation", handleAnimation);
  server.on("/brightness", setBrightness);
  server.on("/eteindre", set_extinction);*/

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