# PROJECTE FINAL PROCESSADORS DIGITALS

<div align="right"> 
Clara Barba i Àlex Mata
</div>

## EXPLICACIÓ
<div align="justify">
 El nostre projecte es basa en un dispositiu que controla l’accés a un edifici. Es passa una targeta contactless i el porter de l’edifici accedeix a una web i decideix a qui dona autorització per entrar i a qui no.
</div> 

## MATERIAL 
- Lector de targetes RFID, 
- Lector de targetes micro SD, 
- 4 leds (un verd, un groc, un vermell i un blau)
- Buzzer 
- Display
- Microprocessador ESP32  
- Cables i resisències 

## FUNCIONAMENT GENERAL

<div align="justify">

Primer hem de pujar el projecte a la ESP32. Dins del Platformio, un entorn de desenvolupament per programadors, hem d'obrir el codi del projecte i clicar primer a *Upload Filesystem Image* per guardar els fitxer de la carpeta data. En el nostre cas tenim el fitxer d'estil per la web en html *style.css*. I finalment clicar a *Upload and Monitor*. 

Un cop compilat el projecte es crea una web asíncrona i s’inicialitzen tots els components del Hardware. El led blau encès indica que el lector està en funcionament i que ja es pot passar la targeta. Es llegeix la base de dades que està guardada en el fitxer *users.txt* de la targeta SD i que ens servirà per associar cada codi de les targetes amb una persona. El display ens mostra la IP de la web, i si escrivim la IP al buscador accedim a la web on el porter pot gestionar els accessos.

Un cop passada la targeta pel lector, el display et dona la benvinguda junt amb el nom de la persona que està associada. A la web apareix una nova pantalla on podem veure el nom de la persona, l'ID de la seva targeta, la data i hora quan ha sol·licitat l'accés i la caducitat de la targeta. La caducitat és útil si es tracta d'un gimnàs per exemple; així el porter pot valorar si se li ha acabat la inscripció o no. Si la targeta no està registrada a la base de dades es mostra el perfil d'un convidat. 

Sota aquest requadre hi ha dos botons, un per autoritzar l'accés i un per denegar-lo. Si el porter prém a *permetre accés*, s'il·lumina el led verd del muntatge, sona un buzzer i en el display es llegeix *Accés autoritzat*. Si prém a *Denegar accés* s'il·lumina un led vermell i en el display veiem *Accés denegat*. 

Mentre el porter no respon a la sol·licitud d'accés s'il·lumina el led groc, per indicar que no es passin més targetes perquè s'està esperant una autorització. Quan s'apaga el led groc i el display mostra *Passi la targeta* ja es pot tornar a passar la següent targeta. 

Podem veure un vídeo del funcionament del nostre dispositiu en el següent enllaç: 
**[Vídeo Muntatge](https://drive.google.com/file/d/1DXv9dDXb7zCLm_fVEkN5MAoQOyZVDbf4/view?usp=sharing )**.

</div>

## MUNTATGE DEL HARDWARE
<div align="justify">

El muntatge l'hem fet sobre una protoboard amb cables fixes sense soldar. En la següent imatge el podem veure:
</div>

![Muntatge del dispositiu](/images/muntatge.jpeg)

<div align="justify">
Hem intentat utilitzar tots els pins possibles de la mateixa banda del ESP32 per simplificar el muntatge. Hem seguit la següent assignació de pins:
</div>

```cpp
// pins SD i RFID
#define SS_PIN_RFID 5
#define CS_PIN_SD 15
#define RST_PIN_RFID 17
// SCK RFID I SCK SD pin18
// MOSI RFID I MOSI SD pin23
// MISO RFID I MISO SD pin19 (amb resistència)

// pins Display
#define SDA_PIN 21
#define SCL_PIN 22

// pins Leds
#define LED_READY 2
#define LED_NOT 0
#define LED_WAIT 4
#define LED_OK 16

// pin Buzzer
#define BUZZER 26
```
<div align="justify">

Els 4 leds estan connectats en sèrie a una resistència de 330Ohm cadascun d'ells. I el port MISO de l'adaptador SD també té una resistència en sèrie, en aquest cas de 1kOhm. El display, els leds, el buzzer i el lector de targetes RFID estan conectats a 3.3V, mentre que l'adaptador de targetes SD està conectat a 5V
</div>



## EXPLICACIÓ DEL CODI
El codi ha estat explicat en la presentació oral.

