

/*******************************************************

  KULLANILAN VIRTUAL PINLER:

  V1  : Terminal Ekranı
  V2  : Telefon Uygulamasındaki Genel/Özel display ekranı - ZORUNLU DEĞİL
  V3  : Telefon Uygulamasındadaki AÇIK/KAPALI Butonu
  V4  : Telefon Uygulamasındaki GENEL/ÖZEL seçimi yapılmasını sağlayan segment switch
  V5  : Telefon Uygulamasındaki BEKLEME SÜRESİNİ ayarlamayı sağlayan sayaç
  V6  : Telefon Uygulamasındaki TEMİZLE Butonu
  V7  : Telefon Uygulamasındaki GİRİŞ EFEKTİNİ seçmeyei sağlayanan sayaç
  V8  : Telefon Uygulamasındaki ÇIKIŞ EFEKTİNİ seçmeyei sağlayanan sayaç
  V9  : Telefon Uygulamasındaki PARLAKLIK değerini ayarlamayı sağlayan sayaç
  V10 : Telefon Uygulamasındaki EFEKT HIZINI ayarlamayı sağlayan kaydırıcı

*******************************************************/



#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <WidgetRTC.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>


#define BLYNK_PRINT Serial
WidgetTerminal terminal(V1);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN D5
#define DATA_PIN D7
#define CS_PIN D8


MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


char auth[] = "Kendi Blynk Tokeninizi Girin";

char ssid[] = "Kendi WiFi Ağınızın adını girin";
char pass[] = "Kendi WiFi Ağınızın şifresini girin";


//////////////////------------- HAVA DURUMU BİLGİSİ --------------///////////////////
const char* host = "api.thingspeak.com";
const int httpPortRead = 80;
const char* url1 = "/apps/thinghttp/send_request?api_key=QXDZI9ZCEEHBK3ON";
int To_remove;
String derece, Data_Raw;

WiFiClient client;
HTTPClient http;

BlynkTimer timer;
WidgetRTC rtc;


String currentTime;
String currentDate;

String saatAyir;
String dakikaAyir;
String gunAyir;
String ayAyir;
String dereceAyir;
int sicaklik;
int td;
bool genel = true;
bool ozel = false;
String haftaninGunu;
String deger;


//////////////////------------- Başlangıç Değerleri --------------///////////////////
int beklemeHizi = 0;
int giris = 2;
int cikis = 2;
int parlaklik = 5;
int efektHizi = 60;
int tur = 0;

uint8_t scrollSpeed = 50;                 // Kayma hızı, rakam küçüldükçe hız artar

textEffect_t scrollEffectin = PA_SCROLL_LEFT;
textEffect_t scrollEffectout = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_CENTER;
uint16_t scrollPause = 0;

#define  BUF_SIZE  75                     // Metin uzunluğu
char mesajyaz[BUF_SIZE];
const uint16_t WAIT_TIME = 1000;
int a = 0;
int sayac = 0;
String mesaj = "TA2ANI";                  // Başlangıç mesajı

uint8_t  inFX, outFX;                     // Giriş ve çıkışta uygulanacak efektler
textEffect_t  effect[] =
{
  PA_PRINT,           //0
  PA_SCAN_HORIZ,      //1
  PA_SCROLL_LEFT,     //2
  PA_WIPE,            //3
  PA_SCROLL_UP_LEFT,  //4
  PA_SCROLL_UP,       //5
  PA_OPENING_CURSOR,  //6
  PA_GROW_UP,         //7
  PA_MESH,            //8
  PA_SCROLL_UP_RIGHT, //9
  PA_BLINDS,          //10
  PA_CLOSING,         //11
  PA_RANDOM,          //12
  PA_GROW_DOWN,       //13
  PA_SCAN_VERT,       //14
  PA_SCROLL_DOWN_LEFT,//15
  PA_WIPE_CURSOR,     //16
  PA_DISSOLVE,        //17
  PA_OPENING,         //18
  PA_CLOSING_CURSOR,  //19
  PA_SCROLL_DOWN_RIGHT,//20
  PA_SCROLL_RIGHT,    //21
  PA_SLICE,           //22
  PA_SCROLL_DOWN,     //23
  PA_NO_EFFECT,       //24
};




void setup()
{
  Serial.begin(57600);
  P.begin();
  P.displayText(mesajyaz, scrollAlign, scrollSpeed, scrollPause, scrollEffectin, scrollEffectout);
  inFX = 2;   //giriş efekti
  outFX = 2;  //çıkış efekti
  mesaj.toCharArray(mesajyaz, BUF_SIZE);

  for (int i = 0; i <= 24; i++)
  {
    terminal.println("");     // Açılışta Terminal Ekranını Temizle
  }
  Blynk.begin(auth, ssid, pass);
  terminal.flush();
  Blynk.virtualWrite(V4, 1);
  Blynk.virtualWrite(V5, 0);
  Blynk.virtualWrite(V7, 2);
  Blynk.virtualWrite(V8, 2);
  Blynk.virtualWrite(V9, 5);
  Blynk.virtualWrite(V10, 60);
  rtc.begin();

  timer.setInterval(15000L, clockDisplay);
  timer.setInterval(50L, dongu);
  timer.setInterval(20000L, weather);
}




void dongu()
{
  if (P.displayAnimate())
  {
    P.setIntensity(parlaklik);
    P.setSpeed(60);


    if (second() >= 43 && second() <= 48 )
    {
      a = 1;
    }

    if (second() >= 49 && second() <= 53 )
    {
      a = 2;
    }

    if (second() >= 54 && second() <= 59 )
    {
      a = 3;
    }

    if (second() >= 0 && second() <= 42 )
    {
      a = 0;
    }

    if (!ozel && genel)
    {
      if (sayac == 0 )
      {

        if (a == 0)
        {
          fonksiyon_1();          // SAAT EKRANI

        }


        if (a == 1)
        {
          fonksiyon_2();          // TARİH EKRANI
        }


        if (a  == 2)
        {
          fonksiyon_3();         // Şehir isminin olduğu Fonksiyon
        }


        if (a  == 3)
        {
          fonksiyon_4();         // DERECE EKRANI
        }
      }

      if (sayac == 1 )
      {
        fonksiyon_0();            // TEMİZ EKRAN
      }


      if (sayac >= 2)
      {
        sayac = 0;
      }
    }

    else
    {
      if (tur == 0)
      {
        fonksiyon_0();
        tur = 1;
        delay(100);
      }
      fonksiyon_5();              // TERMİNAL EKRANI
    }
    P.displayReset();
  }

  if (sayac == 0)
  {
    Blynk.virtualWrite(V2, "ACIK");
  }
  else
  {
    Blynk.virtualWrite(V2, "KAPALI");
  }
}


//////////////////------------- HAVA DURUMU --------------///////////////////

void weather ()
{
  if ( http.begin(host, httpPortRead, url1))     //Connect to the host and the url
  {
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        Data_Raw = http.getString();
        derece = Data_Raw;
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else
  {
    Serial.printf("[HTTP} Unable to connect\n");
  }
  sicaklik = derece.toInt();
}




//////////////////------------- SAAT VE GÜN BİLGİSİ --------------///////////////////

void clockDisplay()
{
  int td = weekday();

  switch (td)
  {
    case 1 :
      haftaninGunu = "Pazar";
      break;
    case 2 :
      haftaninGunu = "Pazartesi";
      break;
    case 3 :
      haftaninGunu = "Sali";
      break;
    case 4 :
      haftaninGunu = "Carsamba";
      break;
    case 5 :
      haftaninGunu = "Persembe";
      break;
    case 6 :
      haftaninGunu = "Cuma";
      break;
    case 7 :
      haftaninGunu = "Cumartesi";
      break;
  }


  if (hour() < 10)
  {
    saatAyir = '0' + String(hour());
  }
  else
  {
    saatAyir = String(hour());
  }


  if (minute() < 10)
  {
    dakikaAyir = '0' + String(minute());
  }
  else
  {
    dakikaAyir = String(minute());
  }


  if (day() < 10)
  {
    gunAyir = '0' + String(day());
  }
  else
  {
    gunAyir = String(day());
  }


  if (month() < 10 )
  {
    ayAyir = '0' + String(month());
  }
  else
  {
    ayAyir = String(month());
  }

  currentTime = saatAyir + ":" + dakikaAyir ; // + ":" + second();
  currentDate = "   " + gunAyir + "." + ayAyir + "." + year() + "   " + haftaninGunu ;

}




void fonksiyon_0()                                // TEMİZ EKRAN
{
  if (P.displayAnimate())
  {
    P.setIntensity(parlaklik);
    String metin_0 = " ";
    metin_0.toCharArray(mesajyaz, BUF_SIZE);
    inFX = 24;
    outFX = 24;
    P.setTextEffect(effect[inFX], effect[outFX]);
    P.setPause(0);
    P.displayReset();
  }
}




void fonksiyon_1()                                // SAAT EKRANI
{
  currentTime.toCharArray(mesajyaz, BUF_SIZE);
  inFX = 24;
  outFX = 24;
  P.setTextEffect(effect[inFX], effect[outFX]);
}




void fonksiyon_2()                              // TARİH EKRANI
{
  currentDate.toCharArray(mesajyaz, BUF_SIZE);
  inFX = 2;
  outFX = 2;
  P.setTextEffect(effect[inFX], effect[outFX]);
  P.setPause(0);
}



void fonksiyon_3()                              // Şehir isminin olduğu Fonksiyon
{
  String metin_3 = "Samsun";
  metin_3.toCharArray(mesajyaz, BUF_SIZE);
  inFX = 2;
  outFX = 2;
  P.setTextEffect(effect[inFX], effect[outFX]);
  P.setPause(0);
}



void fonksiyon_4()                            // Sıcaklık Ekranı
{
  dereceAyir = String(sicaklik);
  String metin_4 = dereceAyir + " " + "C";
  metin_4.toCharArray(mesajyaz, BUF_SIZE);
  inFX = 2;
  outFX = 2;
  P.setTextEffect(effect[inFX], effect[outFX]);
  P.setPause(3000);
}




void fonksiyon_5()                            // KENDİ GÖNDERECEĞİMİZ YAZININ YAZILDIĞI YER
{
  if (P.displayAnimate())
  {
    P.setIntensity(parlaklik);
    P.setSpeed(efektHizi);

    deger.toCharArray(mesajyaz, BUF_SIZE);
    inFX = giris;
    outFX = cikis;
    P.setTextEffect(effect[inFX], effect[outFX]);
    P.setPause(beklemeHizi);
    P.displayReset();
  }
}



BLYNK_WRITE(V1)                             // TERMİNAL EKRANI
{
  if (ozel && !genel)
  {
    deger = param.asStr();
    terminal.println("-------");
    terminal.println(deger);
    terminal.flush();
  }
}



BLYNK_WRITE(V3)                              // SAYAÇ ARTTIRICI
{
  if (!ozel && genel)
  {
    if (param.asInt() == 1)
    {
      sayac++;
    }
  }
}





BLYNK_WRITE(V4)                              // SEÇİCİ SWITCH
{
  switch (param.asInt())
  {
    case 1 :                                 // GENEL
      genel = true;
      ozel = false;
      deger = "";
      sayac = 0;
      giris = 2;
      cikis = 2;
      beklemeHizi = 0;
      efektHizi = 60;
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V7, 2);
      Blynk.virtualWrite(V8, 2);
      Blynk.virtualWrite(V10, 60);
      for (int i = 0; i <= 24; i++)
      {
        terminal.println("");
      }
      terminal.flush();
      break;

    case 2 :                              // ÖZEL
      ozel = true;
      genel = false;
      a = 0;
      tur = 0;
      sayac = 1;
      giris = 2;
      cikis = 2;
      Blynk.virtualWrite(V7, 2);
      Blynk.virtualWrite(V8, 2);
      break;
  }
}



BLYNK_WRITE(V5)                              // BEKLEME SÜRESİ
{
  beklemeHizi =  param.asInt() * 1000;
}




BLYNK_WRITE(V6)                              // TEMİZLE TUŞU
{
  if ( param.asInt() == 1)
  {
    deger = "";
    for (int i = 0; i <= 24; i++)
    {
      terminal.println("");
    }
    terminal.flush();
    beklemeHizi = 0;
    giris = 2;
    cikis = 2;
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V7, 2);
    Blynk.virtualWrite(V8, 2);
  }
}




BLYNK_WRITE(V7)                              // GİRİŞ EFEKTİ
{
  giris =  param.asInt();
}




BLYNK_WRITE(V8)                              // ÇIKIŞ EFEKTİ
{
  cikis =  param.asInt();
}



BLYNK_WRITE(V9)                              // PARLAKLIK
{
  parlaklik = param.asInt();
}



BLYNK_WRITE(V10)                              // Efekt Hızı
{
  efektHizi = param.asInt();
}



void loop()
{
  Blynk.run();
  timer.run();
}
