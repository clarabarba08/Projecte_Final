#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "SD.h"
 
const char* ssid = "huawei_clara";
const char* password =  "608838558";

extern String user;
extern String id;
extern String expiration;
extern String status;
extern String hora_data;

void allowed();

void notAllowed();

AsyncWebServer server(80);

void init_wifi();

const char index_html[] PROGMEM = R"rawliteral(
  <!doctype html>
<html lang="ca">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="">
    <title>Administrar accessos</title>
    

    <!-- Bootstrap core CSS -->
    <link rel="stylesheet" type="text/css" href="style.css">

    <!-- <style>
      .bd-placeholder-img {
        font-size: 1.125rem;
        text-anchor: middle;
        -webkit-user-select: none;
        -moz-user-select: none;
        user-select: none;
      }

      @media (min-width: 768px) {
        .bd-placeholder-img-lg {
          font-size: 3.5rem;
        }
      }
    </style> -->

    
    <!-- Custom styles for this template -->
    <!-- <link href="starter-template.css" rel="stylesheet"> -->
  </head>
  <body>
    
<div class="col-lg-8 mx-auto p-3 py-md-5">
  <header class="d-flex align-items-center pb-3 mb-5 border-bottom">
    <a href="/" class="d-flex align-items-center text-dark text-decoration-none">
      <!-- <svg xmlns="http://www.w3.org/2000/svg" width="40" height="32" class="me-2" viewBox="0 0 118 94" role="img"><title>Bootstrap</title><path fill-rule="evenodd" clip-rule="evenodd" d="M24.509 0c-6.733 0-11.715 5.893-11.492 12.284.214 6.14-.064 14.092-2.066 20.577C8.943 39.365 5.547 43.485 0 44.014v5.972c5.547.529 8.943 4.649 10.951 11.153 2.002 6.485 2.28 14.437 2.066 20.577C12.794 88.106 17.776 94 24.51 94H93.5c6.733 0 11.714-5.893 11.491-12.284-.214-6.14.064-14.092 2.066-20.577 2.009-6.504 5.396-10.624 10.943-11.153v-5.972c-5.547-.529-8.934-4.649-10.943-11.153-2.002-6.484-2.28-14.437-2.066-20.577C105.214 5.894 100.233 0 93.5 0H24.508zM80 57.863C80 66.663 73.436 72 62.543 72H44a2 2 0 01-2-2V24a2 2 0 012-2h18.437c9.083 0 15.044 4.92 15.044 12.474 0 5.302-4.01 10.049-9.119 10.88v.277C75.317 46.394 80 51.21 80 57.863zM60.521 28.34H49.948v14.934h8.905c6.884 0 10.68-2.772 10.68-7.727 0-4.643-3.264-7.207-9.012-7.207zM49.948 49.2v16.458H60.91c7.167 0 10.964-2.876 10.964-8.281 0-5.406-3.903-8.178-11.425-8.178H49.948z" fill="currentColor"></path></svg> -->
      <span class="fs-4">Administrador d'accessos al recinte</span>
    </a>
  </header>

  <main>
    <div class="row g-5">
      <div class="col-md-6">
        <h2>Gestionar accés</h2>
        <p id="no_user">Cap usuari ha sol·licitat l'accés.</p>
        <!-- <p>Read more detailed instructions and documentation on using or contributing to Bootstrap.</p> -->
       <div id="user_card">
        <div class="card mb-3" style="max-width: 540px;">
          <div class="row g-0">
            <div class="col-md-4">
              <img src="https://images-wixmp-ed30a86b8c4ca887773594c2.wixmp.com/f/271deea8-e28c-41a3-aaf5-2913f5f48be6/de7834s-6515bd40-8b2c-4dc6-a843-5ac1a95a8b55.jpg?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1cm46YXBwOjdlMGQxODg5ODIyNjQzNzNhNWYwZDQxNWVhMGQyNmUwIiwiaXNzIjoidXJuOmFwcDo3ZTBkMTg4OTgyMjY0MzczYTVmMGQ0MTVlYTBkMjZlMCIsIm9iaiI6W1t7InBhdGgiOiJcL2ZcLzI3MWRlZWE4LWUyOGMtNDFhMy1hYWY1LTI5MTNmNWY0OGJlNlwvZGU3ODM0cy02NTE1YmQ0MC04YjJjLTRkYzYtYTg0My01YWMxYTk1YThiNTUuanBnIn1dXSwiYXVkIjpbInVybjpzZXJ2aWNlOmZpbGUuZG93bmxvYWQiXX0.BopkDn1ptIwbmcKHdAOlYHyAOOACXW0Zfgbs0-6BY-E" class="img-fluid rounded-start" alt="...">
            </div>
            <div class="col-md-8">
              <div class="card-body">
                <h5 class="card-title" id="user">%USER%</h5>
                <p class="card-text"><b>ID: </b><span id="id">%ID%</span></p>
                <p class="card-text"><b>CAD: </b><span id="expiration">%EXPIRATION%</span></p>
                <p class="card-text"><small class="text-muted">Sol·licita accés a les <span id="hora">%HORA%</span></small></p>
              </div>
            </div>
          </div>
        </div>
        <div>
          <button type="button" class="btn btn-success" id="allow" onclick="toggleCheckbox('yes');">Permetre accés</button>
          <button type="button" class="btn btn-danger" id="not_allow" onclick="toggleCheckbox('no');">Denegar accés</button>
        </div>
        </div>
        
      </div>
    </div>
  </main>
  <footer class="pt-5 my-5 text-muted border-top">
    Clara Barba i Àlex Mata &middot; Processadors Digitals ESEIAAT UPC &middot; Curs 2021/2022
  </footer>
</div>


    <script src="js/bootstrap.bundle.min.js"></script>
    <script>
      document.getElementById('no_user').style.display = 'block';
      document.getElementById('user_card').style.display = 'none';
    </script>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
     document.getElementById("allow").disabled = true;
      document.getElementById("not_allow").disabled = true;

   }
  </script>

    <script>
      
      setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            if(this.responseText=="1")
            {
              document.getElementById('no_user').style.display = 'none';
              document.getElementById('user_card').style.display = 'block';
                   document.getElementById("allow").disabled = false;
      document.getElementById("not_allow").disabled = false;

            }
            else {
              document.getElementById('no_user').style.display = 'block';
              document.getElementById('user_card').style.display = 'none';
            }
          }
        };
        xhttp.open("GET", "/status", true);
        xhttp.send();
      }, 1000 ) ;

      setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("user").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/user", true);
        xhttp.send();
      }, 1000 ) ;

      setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("id").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/id", true);
        xhttp.send();
      }, 1000 ) ;

            setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("hora").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/hora", true);
        xhttp.send();
      }, 1000 ) ;

      setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("expiration").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/expiration", true);
        xhttp.send();
      }, 1000 ) ;
      </script>

      
  </body>
</html>)rawliteral";

String processor(const String& var){
  //Serial.println(var);
  if(var == "USER"){
    return user;
  }
  else if(var == "EXPIRATION")
  {
    return expiration;
  }
  else if(var == "ID")
  {
    return id;
  }
  else if(var=="HORA")
  {
    return hora_data;
  }
  return String();
}

void websensor(void* param);

void init_wifi(){
    xTaskCreate(
       websensor,
       "websensor",
       10000,
       NULL,
       1,
       NULL
    );
}

void websensor(void* param) {
  // Serial.begin(115200);
 
  if(!SPIFFS.begin(true)){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/yes", HTTP_GET, [] (AsyncWebServerRequest *request) {
    allowed();
    request->send(200, "text/plain", "ok");
  });

    server.on("/no", HTTP_GET, [] (AsyncWebServerRequest *request) {
    notAllowed();
    request->send(200, "text/plain", "ok");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/user", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", user.c_str());
  });

  server.on("/expiration", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", expiration.c_str());
  });

  server.on("/id", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", id.c_str());
  });

  server.on("/hora", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", hora_data.c_str());
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", status.c_str());
  });


  server.begin();
  
  vTaskDelay(100 / portTICK_PERIOD_MS);

  vTaskDelete(NULL);

}
