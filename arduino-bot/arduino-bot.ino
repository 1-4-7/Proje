#include <SoftwareSerial.h>                 //SoftwareSerial kütüphanimizi ekliyoruz.
#include <DHT.h>                            //DHT11 sensör kütüphanemizi ekliyoruz.
#include <LiquidCrystal_I2C.h>              //i2c modülü için kütüphane
#include <Wire.h>
#include <Adafruit_BMP085.h>

String agAdi = "eon";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "";        //Ağımızın şifresini buraya yazıyoruz.

#define rxPin 10             //ESP8266 TX pini
#define txPin 11            //ESP8266 RX pini



String ip = "184.106.153.149";    //Thingspeak ip adresi

#define DHTPIN 2            //sıcaklık nem sensörünün pini
#define DHTTYPE DHT11       //sıcaklık nem sensörünün modeli
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;
SoftwareSerial esp(rxPin, txPin);          //digital pinleri software olarak tx/rx pini yapıyoruz.
LiquidCrystal_I2C lcd(0x27, 16, 2);        //lcd ekranın bilgilerni giriyoruz

//---------yazdır foksiyonu-------------
void yazdir(String yazi) {
  Serial.println(yazi);
  lcd.clear();
  int bosluk = yazi.indexOf(' ');
  lcd.setCursor(0,0);
  lcd.print(yazi.substring(0, bosluk));
  lcd.setCursor(0,1);
  lcd.print(yazi.substring(bosluk + 1));
}
//---------------------------------------
// zararlı gaz
void gazhata(int ppm) {
lcd.clear();
    lcd.noBacklight();
    delay(100);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("  ZARARLI GAZ  ");
    lcd.setCursor(3, 1);
    lcd.print("ppm :");
    lcd.print(ppm);
    delay(1000);
    lcd.clear();
    lcd.noBacklight();
    delay(100);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("  ZARARLI GAZ  ");
    lcd.setCursor(3, 1);
    lcd.print("ppm :");
    lcd.print(ppm);
    delay(1000);
    lcd.clear();
}

void setup() { 
  Serial.begin(9600);
  dht.begin(); 
  lcd.begin();
  lcd.backlight();
  if (!bmp.begin()) {
    Serial.println("Geçerli bir BMP180 sensörü bulunamadı, kablolamayı kontrol edin!");
    while (1) {}
  }

  yazdir("Baslatildi...");
  esp.begin(115200);                  //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                   //AT komutu ile modül kontrolünü yapıyoruz.
  yazdir("AT Yollandi");
  
  while(!esp.find("OK")){               //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    yazdir("ESP8266 Bulunamadi.");
  }

  yazdir("OK Komutu Alindi");

  esp.println("AT+CWMODE=1");      //ESP8266 modülünü client olarak ayarlıyoruz.
  
  while(!esp.find("OK")){                //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    yazdir("Ayar Yapiliyor....");
  }

  yazdir("Client olarak ayarlandi");
  yazdir("Aga Baglaniliyor...");

  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  yazdir("Aga Baglandi.");
  delay(1000);
}

void loop() {
  int ppm = analogRead(A0);
  
  //-----------------veri okuma--------------------------------------

  int basinc = bmp.readPressure(); // basınç
  int yukseklik = bmp.readAltitude();  // yükseklik M
  float nem = dht.readHumidity();
  float sicaklik = dht.readTemperature();
  //-----------------------------------------------------------------
  

  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    yazdir("Siteye baglanilamadi.");
  }
  // gaz hatası------------------------------
  if (ppm > 100) {
    gazhata(ppm);
  }

  //-----------------veri gönderme--------------------------------------
  
  String veri = "GET https://api.thingspeak.com/update?api_key=WWSLZLR1BZ0S6GHV";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.                                   //Göndereceğimiz sıcaklık değişkeni
  veri += "&field1=";
  veri += String(sicaklik);       //Göndereceğiz sıcaklık verisi
  veri += "&field2=";
  veri += String(nem);             //Göndereceğiz nem verisi                   
  veri += "&field3=";
  veri += String(ppm);            // hava kalite sensörü
  veri += "&field4=";
  veri += String(basinc);
  veri += "&field5=";
  veri += String(yukseklik);
  veri += "\r\n\r\n";          //HTTP isteğinin sonunu belirtmek için
  esp.print("AT+CIPSEND=");
  yazdir("Veri olusturuluyor.");                 //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);           //+2 nin sebebi \r\n
  delay(2000);
  if(esp.find(">")){                        //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                     //Veriyi gönderiyoruz.
    Serial.println(veri);
    yazdir("Veri gonderildi.");
    delay(1000);
  }
  yazdir("Baglanti Kapatildi.");
  esp.println("AT+CIPCLOSE");                //Bağlantıyı kapatıyoruz
  delay(1000);                              //Yeni veri gönderimi için 1 dakika bekliyoruz.
}




