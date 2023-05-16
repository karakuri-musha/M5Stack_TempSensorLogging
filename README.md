# M5Stack_TempSensorLogging

## OverView
Arduino を使って M5Stack Basic と温度センサーを組み合わて、熱帯魚観賞用の水槽の水温を計測し、M5Stackに搭載されているmicroSDカードへデータを記録するプログラムを作りました。
画面描画はLovyanGFXライブラリを使っています。
プログラムを実行するとWi-Fi接続し、NTPサーバと時刻同期します。

詳しくは、次のブログで紹介していますので確認してください。

Using Arduino, I combined M5Stack Basic and a temperature sensor to create a program that measures the water temperature of a tropical fish aquarium and records the data to the microSD card installed in M5Stack.
Screen drawing uses the LovyanGFX library.
Run the program to connect to Wi-Fi and synchronize the time with the NTP server.

For details, please check the following blog.

### Blog URL
https://karakuri-musha.com/inside-technology/arduino-m5stack-watertheam_sensor01/

## Src File
"src"フォルダに以下のソース一式が入っています。
- M5Stack_TempSensor_v1.ino（今回作成したプログラム本体）
- Screen_image.h（画面表示用画像データ）

The "src" folder contains the following set of sources.
- M5Stack_TempSensor_v1.ino (program body created this time)
- Screen_image.h (image data for screen display)

## How to use
1. GitHubの右上にある「Code」から「Download Zip」を選択します。
2. ダウンロードしたファイルを解凍します。
3. "microSD"フォルダにある次のファイルをmicroSDカードへ保存します。
- SSID.txt
- TempSensor_log.txt
4. microSDカードに保存した、"SSID.txt"をテキストエディタで開き、使用するWi-Fi接続情報（アクセスポイント情報）を書き換えて保存します。
5. microSDカードをM5Stackに挿入します。
6. 温度センサーをM5Stackに接続します。
7. ダウンロードした"src"フォルダにある"M5Stack_TempSensor_v1.ino"をArduino IDEで開きます。
8. Arduino IDEからM5Stackに書き込みます。
---
1. Select "Download Zip" from "Code" at the top right of GitHub.
2. Unzip the downloaded file.
3. Save the following files in the "microSD" folder to the microSD card.
-SSID.txt
-TempSensor_log.txt
4. Open "SSID.txt" saved in the microSD card with a text editor, rewrite the Wi-Fi connection information (access point information) to be used, and save it.
5. Insert the microSD card into M5Stack.
6. Connect the temperature sensor to M5Stack.
7. Open "M5Stack_TempSensor_v1.ino" in the downloaded "src" folder with Arduino IDE.
8. Write to M5Stack from Arduino IDE.

## Operating environment
動作確認環境は以下の通りです。

### 1.HARDWARE
- M5Stack Basic
- Temperature sensor (DS18B20)

### 2.IDE
- Arduino IDE (ver:2.1.0）

### 3.BoardManager
- [Name] M5Stack/M5Stack-Core-ESP32

### 4.Additional library
- ArduinoJson (6.21.2)
- OneWire (2.3.7)
- LovyanGFX (1.1.6)


### 3.Other
 ライブラリやボードマネージャなどの構成は、上記のBlog URLを参照してください。
 
 Please refer to the blog URL above for configuration of libraries, boards manager, etc.
