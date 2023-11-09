#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.
#include <DHT.h>                                            //DHT11 sensör kütüphanemizi ekliyoruz.
#include <LiquidCrystal_I2C.h>          //i2c modülü için kütüphane

String agAdi = "eon";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "17032405";           //Ağımızın şifresini buraya yazıyoruz.

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini
int dht11Pin = 2;



String ip = "184.106.153.149";                                //Thingspeak ip adresi
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void yazdir(String yazi) {
  Serial.println(yazi);
  lcd.clear();
  int bosluk = yazi.indexOf(' ');
  lcd.setCursor(0,0);
  lcd.print(yazi.substring(0, bosluk));
  lcd.setCursor(0,1);
  lcd.print(yazi.substring(bosluk + 1));
}

void setup() { 
  Serial.begin(9600);
  dht.begin(); 
  lcd.begin();

  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  yazdir("Baslatildi...");
  esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  yazdir("AT Yollandi");
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    yazdir("ESP8266 Bulunamadi.");
  }
  yazdir("OK Komutu Alindi");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
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
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    yazdir("AT+CIPSTART Error");
  }
  
  float nem = dht.readHumidity();
  float sicaklik = dht.readTemperature();
  String veri = "GET https://api.thingspeak.com/update?api_key=WWSLZLR1BZ0S6GHV";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.                                   //Göndereceğimiz sıcaklık değişkeni
  veri += "&field1=";
  veri += String(sicaklik);
  veri += "&field2=";
  veri += String(nem);                                        //Göndereceğimiz nem değişkeni
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    yazdir("Veri gonderildi.");
    delay(1000);
  }
  yazdir("Baglanti Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}




