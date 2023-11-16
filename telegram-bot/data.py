import json
import requests
from datetime import datetime, timezone

#token okuma------------------------
with open('ayarlar.json') as config_file:       #özel verileri json dosyasından alıyor
    data = json.load(config_file)
TOKEN = data['TOKEN']                        #bot tokeni
THINGSPEAK_CHANNEL_ID = data['THINGSPEAK_CHANNEL_ID']       #thingspeak kanalının ıd si
THINGSPEAK_API_READ_KEY = data['THINGSPEAK_API_READ_KEY']       #thingspeak kanalının okuma anahtarı
api_key = data['openWeather_API']
#-----------------------------------

def get_thingspeak_data():
    url = f'https://api.thingspeak.com/channels/{THINGSPEAK_CHANNEL_ID}/feeds.json?api_key={THINGSPEAK_API_READ_KEY}'
    response = requests.get(url)
    data = json.loads(response.text)
    return data

data = get_thingspeak_data()

sicaklik = data['feeds'][-1]['field1']  # 'sıcaklık' yerine gerçek anahtarınızı yazın
nem = data['feeds'][-1]['field2']  # 'nem' yerine gerçek anahtarınızı yazın
enlem = data['feeds'][-1]['field3']
boylam = data['feeds'][-1]['field4']
gaz = data['feeds'][-1]['field5']
basinc = data['feeds'][-1]['field6']
uydu = data['feeds'][-1]['field7']


print(sicaklik)
print(nem)
print(enlem)
print(boylam)
print(gaz)
print(basinc)
print(uydu)
#-----------------------------------
#hava durumu datası




