from telegram import Update, InlineKeyboardButton, InlineKeyboardMarkup
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes, CallbackContext
from datetime import datetime, timezone
from telegram.constants import ParseMode
import json
import requests



#token okuma------------------------
with open('ayarlar.json') as config_file:       #özel verileri json dosyasından alıyor
    key = json.load(config_file)
api_key = key['openWeather_API']
#-----------------------------------


#foksiyınlar

def get_weather(lat, lon):

    base_url = "http://api.openweathermap.org/data/2.5/forecast"

    params = {
        "lat": lat,
        "lon": lon,
        "appid": api_key
    }

    response = requests.get(base_url, params=params)
    data = response.json()

    weather_translation = {
        "clear sky": "Gökyüzü açık",
        "overcast clouds": "Bulutlu",
        "few clouds": "Az Bulutlu",
        "light rain": "Hafif yağmurlu",
        "broken clouds": "Parçalı bulutlu"
    }

    month_translation = {
        "January": "Ocak",
        "February": "Şubat",
        "March": "Mart",
        "April": "Nisan",
        "May": "Mayıs",
        "June": "Haziran",
        "July": "Temmuz",
        "August": "Ağustos",
        "September": "Eylül",
        "October": "Ekim",
        "November": "Kasım",
        "December": "Aralık"
    }
    durum = []
    oneri = []
    for i in range(5):
        timestamp = data['list'][i * 8]['dt']  # Burada hata vardı. Her döngüde, listenin ilk elemanını alıyordunuz.
        date = datetime.fromtimestamp(timestamp, timezone.utc).strftime('%B %d')
        month, day = date.split()
        date_tr = "{} {}".format(day, month_translation.get(month, month))

        weather_description = data['list'][i * 8]['weather'][0]['description']
        try:
            rain_volume = data['list'][i * 8]['rain']['3h']
        except KeyError:
            rain_volume = 0

        weather_description_tr = weather_translation.get(weather_description, weather_description)

        if rain_volume == 0:
            rain_comment = "Yağış oranı 10%"
        elif rain_volume < 2.5:
            rain_comment = "Yağış oranı 40%"
        elif rain_volume < 7.6:
            rain_comment = "Yağış oranı 60%"
        else:
            rain_comment = "Yağış oranı 80%"

        durum.append("{}: {}, {}".format(date_tr, weather_description_tr, rain_comment))  # Çıktıyı listeye ekleyin

        if "rain" in weather_description or rain_volume > 0:
            oneri.append("Bugün dışarı çıkmanı önermem, çünkü hava yağışlı görünüyor.")
        elif "clouds" in weather_description:
            oneri.append("Bugün dışarı çıkabilirsin, ancak hava bulutlu olabilir.")
        else:
            oneri.append("Bugün dışarı çıkmak için harika bir gün!")
    durum += oneri
    return durum

durum = get_weather(37.0000, 35.3213)  # Adana için enlem ve boylam
print(durum[0])
print(durum[1])
print(durum[2])
print(durum[3])
print(durum[4])
print(durum[5])
print(durum[6])
print(durum[7])
print(durum[8])
print(durum[9])

