#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include <FS.h>
#include <SPIFFS.h>
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
int brightness = 127;
int extinction;
double temps1 = 0;
double temps2 = 0;

//changer href css -> style.css
//changer href icon -> logo.jpg
//changer src img -> titleWhite.png
//changer background -> wallpaper.jpg

// Déclaration de la variable contenant le code HTML
const char* index_html = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Neon Control</title>
    <link rel="stylesheet" href="style.css">
    <link rel="icon" href="logo.jpg">
</head>
<body>
    <header id="header">
        <div>
            <a href="#home"><img src="titleWhite.png" alt="Max & Flo"></a>
            <label class="switch">
                <input type="checkbox" id="switch_on_off" onclick="checkbox_status()">
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
            <h2>LUMINOSITY</h2>
            <div>
                <input type="range" onclick='setBrightness()' min="1" max="100" value='%d' step="1" id="brightness">
                <div id="number" class="neonTextWhite">50</div>
            </div>
        </div>
    </div>
    <div class="page2" id="lights">
        <h2>Lights</h2>
        <div id="colors_and_animations">
            <div id="colors">
                <h3>COLORS</h3><br><br>
                <div id="colors_choices">
                    <button><input type="button" value="#F43545" onclick="updateColor_red()" id="b-red"></button>
                    <button><input type="button" value="#FF8901" onclick="updateColor_orange()" id="b-orange"></button>
                    <button><input type="button" value="#FAD717" onclick="updateColor_yellow()" id="b-yellow"></button>
                    <button><input type="button" value="#00BA71" onclick="updateColor_green()" id="b-green"></button>
                    <button><input type="button" value="#00C2DE" onclick="updateColor_cyan()" id="b-cyan"></button>
                    <button><input type="button" value="#005ec9" onclick="updateColor_blue()" id="b-blue"></button>
                    <button><input type="button" value="#ff3ae1" onclick="updateColor_pink()" id="b-pink"></button>
                    <button><input type="button" value="#71129c" onclick="updateColor_purple()" id="b-purple"></button>
                    <button><input type="button" value="#FFFFFF" onclick="updateColor_white()" id="b-white"></button>
                    <button><input type="button" value="#FFFFFF" onclick="updateColor_white()" id="b-white"></button>
                    <button><input type="button" value="#FFFFFF" onclick="updateColor_white()" id="b-white"></button>
                    <button><input type="button" value="#FFFFFF" onclick="updateColor_white()" id="b-white"></button>
                    <button><input type="color" value="#FF0000" id="color-picker" onclick="updateColor()"></button>
                </div>
            </div>
            <div id="animations">
                <h3>FLOWS</h3><br><br>
                <div id="animations_choices">
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                    <button type="button" id="nom_du_flow">nom_du_flow</button>
                </div>
            </div>
        </div>
    </div>
    <div class="transition"></div>
    <div class="page3" id="timer">
        <h2>Timer</h2>
        <div class="page3_2">
            <div class="clock-container">
                <div class="clock-col">
                    <p contenteditable="true" id="clock-day" class="clock-day clock-timer">00</p>
                    <p class="clock-label">Day</p>
                </div>
                <div class="clock-col">
                    <p contenteditable="true" id="clock-hours" class="clock-hours clock-timer">00</p>
                    <p class="clock-label">Hours</p>
                </div>
                <div class="clock-col">
                    <p contenteditable="true" id="clock-minutes" class="clock-minutes clock-timer">00</p>
                    <p class="clock-label">Minutes</p>
                </div>
                <div class="clock-col">
                    <p contenteditable="true" id="clock-seconds" class="clock-seconds clock-timer">00</p>
                    <p class="clock-label">Seconds</p>
                </div>
            </div>

            <div class="button-container">
                <button id="start-timer" class="start-button">Start</button>
                <button id="stop-timer" class="start-button" >Stop</button>
            </div>
        </div>
    </div>

    <footer>
        <h3>Website developped by Max & flo</h3>
        <p>Neon Control - v1</p>
    </footer>
    <script>
        var slider = document.getElementById("brightness");
        var number = document.getElementById("number");
        slider.oninput=function(){number.innerHTML = slider.value;}

        var currentScrollPos = window.pageYOffset;

        document.addEventListener("scroll", (event) => {
            currentScrollPos = window.scrollY;
            if (currentScrollPos > (Math.round(window.innerHeight)*0.85)) {
                document.getElementById("header").style.transition = "all 0.5s ease";
                document.getElementById("header").style.backgroundColor = "rgba(0, 0, 0, 0.95)";
            } else {
                document.getElementById("header").style.transition = "all 0.5s ease";
                document.getElementById("header").style.backgroundColor = "rgba(36, 16, 88, 0)";
            }
        });

        function checkbox_status() {
            if (event.target.checked) {
                slider.style.pointerEvents = "auto";
                slider.disabled = false;
            } else {
                slider.style.pointerEvents = "none";
                slider.disabled = true;
            }
        }

        function updateColor() {
            var color = document.getElementById("color-picker").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }


        function updateColor_red() {
            var color = document.getElementById("b-red").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_orange() {
            var color = document.getElementById("b-orange").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_yellow() {
            var color = document.getElementById("b-yellow").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_green() {
            var color = document.getElementById("b-green").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_cyan() {
            var color = document.getElementById("b-cyan").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_blue() {
            var color = document.getElementById("b-blue").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_pink() {
            var color = document.getElementById("b-pink").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_purple() {
            var color = document.getElementById("b-purple").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        function updateColor_white() {
            var color = document.getElementById("b-white").value;
            var red = parseInt(color.substr(1, 2), 16);
            var green = parseInt(color.substr(3, 2), 16);
            var blue = parseInt(color.substr(5, 2), 16);
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/color?r=" + red + "&g=" + green + "&b=" + blue, true);
            xhr.send();
        }

        

        function setBrightness(){
          var brightness = document.getElementById('brightness').value;
          var xhr = new XMLHttpRequest();

          //brightness = brightness * 2.55;
          //2.48*x + 7.52;
          brightness = Math.round(2.48 * brightness + 7.52);

          xhr.open("GET", '/brightness?brightness=' + brightness, true);
          xhr.send();
        }

        const switchElement = document.getElementById("switch_on_off");
        var switchState = 0;

        switchElement.addEventListener("change",function(){
            if(this.checked){
                switchState = 1;
            }else{
                switchState = 0;
            }

            var xhr = new XMLHttpRequest();
            xhr.open("GET", '/switchState?switchState=' + switchState, true);
            xhr.send();

        });


        //----------------TIMER---------------------------

        let timerInterval; // Variable pour l'intervalle du compte à rebours
        let longPressTimer; // Timer pour détecter un appui long
        let isLongPress = false; // Flag pour identifier un appui long

        document.getElementById('stop-timer').style.display = 'none';

        document.addEventListener('DOMContentLoaded', function () {
            const startButton = document.getElementById('start-timer');
            const stopButton = document.getElementById('stop-timer');

            startButton.addEventListener('click', startCountdown);

            stopButton.addEventListener('mousedown', function() {
                isLongPress = false;
                longPressTimer = setTimeout(function() {
                    isLongPress = true;
                    resetTimerDisplay();
                    stopCountdown();
                    document.getElementById('stop-timer').style.display = 'none';
                }, 1000); // Durée d'appui long (1 seconde)
            });

            stopButton.addEventListener('mouseup', function() {
                clearTimeout(longPressTimer);
                if (!isLongPress) {
                    stopCountdown();
                }
            });

            stopButton.addEventListener('mouseleave', function() {
                clearTimeout(longPressTimer);
            });

            function startCountdown() {
                let days = parseInt(document.getElementById('clock-day').textContent) || 0;
                let hours = parseInt(document.getElementById('clock-hours').textContent) || 0;
                let minutes = parseInt(document.getElementById('clock-minutes').textContent) || 0;
                let seconds = parseInt(document.getElementById('clock-seconds').textContent) || 0;

                setEditable(false);

                let countdownTime = (days * 24 * 60 * 60) + (hours * 60 * 60) + (minutes * 60) + seconds;

                updateTimerDisplay(countdownTime);
                timerInterval = setInterval(() => {
                    if (countdownTime > 0) {
                        countdownTime--;
                        updateTimerDisplay(countdownTime);
                    } else {
                        finishCountdown();
                    }
                }, 1000);

                document.getElementById('stop-timer').style.display = 'inline-block';
            }

            function stopCountdown() {
                clearInterval(timerInterval);
                setEditable(true);
                /*document.getElementById('stop-timer').style.display = 'none';*/
            }

            function resetTimerDisplay() {
                document.getElementById('clock-day').textContent = '00';
                document.getElementById('clock-hours').textContent = '00';
                document.getElementById('clock-minutes').textContent = '00';
                document.getElementById('clock-seconds').textContent = '00';

                document.documentElement.style.setProperty('--timer-day', "00");
                document.documentElement.style.setProperty('--timer-hours', "00");
                document.documentElement.style.setProperty('--timer-minutes', "00");
                document.documentElement.style.setProperty('--timer-seconds', "00");
            }

            function setEditable(isEditable) {
                document.getElementById('clock-day').contentEditable = isEditable;
                document.getElementById('clock-hours').contentEditable = isEditable;
                document.getElementById('clock-minutes').contentEditable = isEditable;
                document.getElementById('clock-seconds').contentEditable = isEditable;
            }

            function updateTimerDisplay(time) {
                let d = Math.floor(time / (24 * 60 * 60));
                let h = Math.floor((time % (24 * 60 * 60)) / (60 * 60));
                let m = Math.floor((time % (60 * 60)) / 60);
                let s = time % 60;

                document.getElementById('clock-day').textContent = d < 10 ? '0' + d : d;
                document.getElementById('clock-hours').textContent = h < 10 ? '0' + h : h;
                document.getElementById('clock-minutes').textContent = m < 10 ? '0' + m : m;
                document.getElementById('clock-seconds').textContent = s < 10 ? '0' + s : s;
            }

            function finishCountdown() {
                clearInterval(timerInterval);
                //alert('Countdown finished!');
                setEditable(true);
                document.getElementById('stop-timer').style.display = 'none';

                //checkbox_status();
                document.getElementById('switch_on_off').checked = false;
                
                var switchState = 0;
                var xhr = new XMLHttpRequest();
                xhr.open("GET", '/switchState?switchState=' + switchState, true);
                xhr.send();
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
    --nav-padding: 1%;
    --red:#F43545;
    --orange: #FF8901;
    --yellow: #FAD717;
    --green: #00BA71;
    --cyan: #00C2DE;
    --blue: #005ec9;
    --pink: #ff3ae1;
    --purple: #71129c;
    --white: #fff;
    --button_radius: 20px;
    --button_size: 90px;
    --button_scale_hover: 1.05;
}
/*------ALL FILE------*/
* {
    margin: 0;
    padding: 0;
    font-family: Tahoma, sans-serif;
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
    z-index: 1;
    /* padding-top: var(--nav-padding); */
    padding-bottom: var(--nav-padding);
    justify-content: space-around;
    align-items: baseline;
} header a { /* LOGO */
    color: white;
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
    width: 220px;
    height: 30px;
    /* margin-left: 20px; */
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
    -webkit-transition: 0.5s;
    transition: 0.5s;
    border-radius: 30px;
}
.slider:before {
    position: absolute;
    content: "";
    height: 24px;
    width: 24px;
    left: 4px;
    bottom: 3px;
    background-color: white;
    -webkit-transition: 0.5s;
    transition: 0.5s;
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
    -webkit-transform: translateX(188px);
    -ms-transform: translateX(188px);
    transform: translateX(188px);
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
    font-size: 22px;
    letter-spacing: 10px;
}
/*------PAGE 1------*/
.page1 {
    height: 100vh;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
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
    color: white;
    font-size: 1580%;
    /* border: 1px solid black; */
    word-break: break-all;
    text-align: center;
    letter-spacing: 5px;
    display: inline-block;
    line-height: 200px;
    width: 50%;
}.page1 h2 {
    color: white;
    font-size: 380%;
    letter-spacing: 16px;
    display: inline;
    writing-mode: vertical-lr;
    padding-top: 3%;
    /* border: 1px solid black; */
}.page1 .luminosity h2 {
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
    width: 5px;
    /* border: 2px solid rgb(200, 23, 23); */
}

/*------PAGE 2------*/
.page2 {
    height: 100vh;
    display: flex;
    align-items: center;
    padding: 2%;
    /* border: 2px solid rgb(200, 23, 23); */
    color: black;
    /* box-shadow: 0px -20px 40px rgba(0, 0, 0, 0.5); */
}.page2 h2 {
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    -webkit-text-fill-color: transparent;
    -webkit-background-clip: text;
    font-size: 1100%;
    letter-spacing: 30px;
    display: inline;
    writing-mode: vertical-lr;
    /* margin-right: 2%; */
    /* border: 2px solid rgb(23, 200, 200); */
    /* text-align: right; */
}.page2 h3 {
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    /* color: white; */
    /* padding: 2%; */
    /* height: 15vh; */
    -webkit-text-fill-color: transparent;
    -webkit-background-clip: text;
    font-size: 600%;
    letter-spacing: 10px;
    /* width: 100%; */
    /* border: 2px solid rgb(179, 200, 23); */
    text-align: center;
}#colors_and_animations {
    /* height: 75vh; */
    width: 90%;
    display: flex;
    flex-wrap: wrap;
    /* justify-content: space-around; */
    /* border: 2px solid rgb(23, 200, 23); */
}#colors {
    /* border: 2px solid rgb(159, 23, 200); */
    display: flex;
    flex-direction: column;
    align-items: center;
    width: 50%;
}#colors_choices {
    display: flex;
    flex-wrap: wrap;
    justify-content: space-evenly;
    width: 70%;
    /* border: 2px solid rgb(159, 23, 200); */
}#colors_choices button {
    display: flex;
    color: white;
    align-items: center;
    /* flex-wrap: wrap; */
    justify-content: center;
    width: 100%;
    border: 1px solid black; 
    width: var(--button_size);
    height: var(--button_size);
    border-radius: var(--button_radius);
    border: 0;
    margin: 3%;
    cursor: pointer;
    box-shadow: 0px 5px 10px 0px rgba(0, 0, 0, 0.5);
    /* font-size: 200%; */
    overflow: hidden;
}#colors_choices button:hover {
    transform: scale(var(--button_scale_hover));
    box-shadow: 0px 10px 20px 2px rgba(0, 0, 0, 0.25);
}
#colors_choices button input {
    width: 100px;
    height: 100px;
    border: 0;
    /* padding: -10px; */
    /* border-radius: 20px; */
    transform: scale(1.1);
    /* padding: 0; */
    cursor: pointer;
    /* -webkit-appearance: none; */
}
#b-red {
    background-color: var(--red);
    color: transparent;
}
#b-orange {
    background-color: var(--orange);
    color: transparent;
}
#b-yellow {
    background-color: var(--yellow);
    color: transparent;
}
#b-green {
    background-color: var(--green);
    color: transparent;
}
#b-cyan {
    background-color: var(--cyan);
    color: transparent;
}
#b-blue {
    background-color: var(--blue);
    color: transparent;
}
#b-pink {
    background-color: var(--pink);
    color: transparent;
}
#b-purple {
    background-color: var(--purple);
    color: transparent;
}

#b-white {
    background-color: var(--white);
    color: transparent;
}

#animations {
    /* border: 2px solid rgb(23, 23, 200); */
    display: flex;
    flex-wrap: wrap;
    flex-direction: column;
    align-items: center;
    /* justify-content: center; */
    width: 50%;
}#animations_choices {
    /* border: 2px solid rgb(23, 200, 32); */
    display: flex;
    flex-wrap: wrap;
    justify-content: space-evenly;
    width: 70%;
}#animations_choices button{
    /* display: flex; */
    /* color: white; */
    /* flex-wrap: wrap; */
    /* justify-content: center; */
    /* width: 45%; */
    border: 1px solid black; 
    /* width: 50%; */
    height: var(--button_size);
    border-radius: var(--button_radius);
    border: 0;
    margin: 3%;
    padding: 4%;
    cursor: pointer;
    box-shadow: 0px 5px 10px 0px rgba(0, 0, 0, 0.5);
    /* border: 2px solid rgb(200, 23, 23); */
    font-size: 150%;
}#animations_choices button:hover {
    transform: scale(var(--button_scale_hover));
    box-shadow: 0px 10px 20px 2px rgba(0, 0, 0, 0.25);
}

/*------TRANSITION------*/
.transition {
    height: 20vh;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 380%;
    letter-spacing: 30px;
    /* font-weight: bold; */
    /* border: 2px solid rgb(200, 23, 23); */
    /* box-shadow: 0px 20px -40px rgba(0, 0, 0, 0.5); */
}

/*------PAGE 3------*/
.page3 {
    height: 100vh;
    display: flex;
    align-items: center;
    padding: 2%;
    /* border: 2px solid rgb(200, 23, 23); */
    color: black;
    /* box-shadow: 0px -20px 40px rgba(0, 0, 0, 0.5); */
}.page3 h2 {
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;
    -webkit-text-fill-color: transparent;
    -webkit-background-clip: text;
    font-size: 1100%;
    letter-spacing: 30px;
    display: inline;
    writing-mode: vertical-lr;
    margin-right: 20%;
    /* text-align: left; */
}

/*------FOOTER------*/
footer {
    height: 20vh;
    color: white;
    background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
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


/*----------------TIMER---------------------------*/

.clock-day:before {
    content: var(--timer-day);
}
.clock-hours:before {
    content: var(--timer-hours);
}
.clock-minutes:before {
    content: var(--timer-minutes);
}
.clock-seconds:before {
    content: var(--timer-seconds);
}

.page3_2 {
    /*background: linear-gradient(45deg, #1870ed 0, #f18f88 100%);*/
    font-family: 'Montserrat', sans-serif;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
}

.clock-container {
    margin-top: 30px;
    margin-bottom: 30px;
    background-color: #080808;
    /*background-image: linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),url(wallpaper.jpg);
    background-position: center;
    background-repeat: no-repeat;
    background-attachment: fixed;
    background-size: cover;*/


    /*border-radius: 20px;*/
    border-radius: var(--button_radius);
    padding: 60px 20px;
    /*box-shadow: 1px 1px 5px rgba(255, 255, 255, .15), 0 15px 90px 30px rgba(0, 0, 0, .25);*/
    box-shadow: 0px 10px 20px 2px rgba(0, 0, 0, 0.5);
    display: flex;
}

.clock-col {
    text-align: center;
    margin-right: 40px;
    margin-left: 40px;
    min-width: 90px;
    position: relative;
    
}

.clock-col:not(:last-child):before,
.clock-col:not(:last-child):after {
    content: "";
    background-color: rgba(255, 255, 255, .3);
    height: 5px;
    width: 5px;
    border-radius: 50%;
    display: block;
    position: absolute;
    right: -42px;
}

.clock-col:not(:last-child):before {
    top: 35%;
}

.clock-col:not(:last-child):after {
    top: 50%;
}

.clock-timer {
    color: white;
    font-size: 10rem;
    margin-top: 10px;
    margin-bottom: 10px;
}

.clock-timer:before {
    font-size: 4.2rem;
    text-transform: uppercase;
}

.clock-label {
    color: rgba(255, 255, 255, .35);
    text-transform: uppercase;
    font-size: .7rem;
    margin-top: 10px;
}

@media (max-width: 825px) {
    .clock-container {
        flex-direction: column;
        padding-top: 40px;
        padding-bottom: 40px;
    }
    .clock-col + .clock-col {
        margin-top: 20px;
    }
    .clock-col:before,
    .clock-col:after {
        display: none !important;
    }
}

.button-container {
    text-align: center;
    width: 100%; /* Utilise la largeur totale pour le centrage */
    margin-top: -1rem; /* Ajuste la marge au-dessus du bouton */
    /*margin-bottom: 2rem; /* Ajuste la marge en dessous du bouton, si nécessaire */
}

.start-button {
    background-color: #000000; /* Choisir une couleur de fond */
    color: #ffffff; /* Choisir une couleur de texte */
    border: 2px solid #000000; /* Choisir une couleur de bordure */
    padding: 10px 20px; /* Ajuster le rembourrage pour la taille du bouton */
    font-size: 2rem; /* Ajuster la taille de la police */
    /*border-radius: 5px; /* Rayon de la bordure pour les coins arrondis */
    border-radius: var(--button_radius);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.5); /* Ombre optionnelle pour la profondeur */
    cursor: pointer; /* Curseur de pointeur pour indiquer la fonctionnalité cliquable */
    transition: background-color 0.3s, color 0.3s; /* Transition pour les interactions hover et click */
}

.start-button:hover {
    background-color: #ffffff; /* Couleur de fond au survol */
    color: #000000; /* Couleur de texte au survol */
    
}

.start-button:active {
    transform: translateY(2px); /* Effet de pression du bouton */
}
)css";

void handleRoot();
void handleCss();
void handleLogo();
void handleTitle();
void handleWallpaper();
void on_off();
//void handleAnimation();

void setBrightness();
void handleColor();
void setLedColor(int r, int g, int b);

//void setAnimation(int mode);
//void set_extinction();
//void pride();
//void va_et_vient();
//void arc_en_ciel();



void setup() {
  // Connexion au réseau Wi-Fi
  //while(!Serial);
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }

  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }

  // Configuration du ruban LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.show();

  // Routes du serveur web
  server.on("/", handleRoot);
  server.on("/style.css", handleCss);
  server.on("/logo.jpg", handleLogo);
  server.on("/titleWhite.png", handleTitle);
  server.on("/wallpaper.jpg", handleWallpaper);

  server.on("/brightness", setBrightness);
  server.on("/switchState", on_off);

  server.on("/color", handleColor);
  /*server.on("/animation", handleAnimation);
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

void handleLogo(){
  //server.send(SPIFFS, "/logo.jpg", "image/png");
  File file = SPIFFS.open("/logo.jpg", "r");
  server.streamFile(file, "logo/jpg");
  file.close();
}

void handleTitle(){
  File file = SPIFFS.open("/titleWhite.png", "r");
  server.streamFile(file, "titleWhite/png");
  file.close();
}

void handleWallpaper(){
  File file = SPIFFS.open("/wallpaper.jpg", "r");
  server.streamFile(file, "wallpaper/jpg");
  file.close();
}

void on_off(){
    int etat_switch = server.arg("switchState").toInt();
    if(etat_switch == 1){
        fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
        //fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
        FastLED.setBrightness(brightness);
    }
    else{
        FastLED.setBrightness(0);
    }
    
    FastLED.show();
    server.send(200, "text/plain", "Néon allumé !");
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

void setBrightness(){
    //fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
    brightness = server.arg("brightness").toInt();
    if(brightness > 255){
        brightness = 255;
    }

    FastLED.setBrightness(brightness);
    FastLED.show();
    server.send(200, "text/plain", "Luminosité définies !"); 
}

// Route pour définir l'animation des LEDs
/*void handleAnimation() {
  int mode = server.arg("mode").toInt();
  setAnimation(mode);
  server.send(200, "text/plain", "Animation définie !");
}*/

// Fonction pour définir l'animation des LEDs
/*void setAnimation(int mode) {
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
}*/