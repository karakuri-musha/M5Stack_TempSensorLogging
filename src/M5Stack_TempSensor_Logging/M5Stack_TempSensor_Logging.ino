//--------------------------------------------------------------
// 水温管理プログラム for M5Stack 
// 
//--------------------------------------------------------------
// ------------------------------------------------------------
// ライブラリインクルード部 Library include section.
// ------------------------------------------------------------
#define LGFX_M5STACK                                        // M5Stack (Basic / Gray / Go / Fire)
#define LGFX_AUTODETECT                                     // 自動認識 (D-duino-32 XS, PyBadge はパネルID読取りが出来ないため自動認識の対象から外れています)
#define LGFX_USE_V1
#include <ArduinoJson.h>                                    // JSONライブラリ
#include <WiFi.h>                                           // Wifi制御用ライブラリ
#include <time.h>                                           // 時刻制御用ライブラリ
#include <OneWire.h>                                        // 水温系制御用ライブラリ

#include <M5Stack.h>                                        // M5Stack 用ライブラ

#include <LovyanGFX.hpp>                                    // 画面制御用ライブラリ（LovyanGFX）
#include <LGFX_AUTODETECT.hpp>                              // 画面制御用ライブラリ（LovyanGFX）

#include "Screen_img.h"                                     // M5StickC 用ライブラリ

// 画面制御関連
int display_rotation = 1;                                   // 画面向きの初期化
int display_stat = 1;                                       // 画面表示切替用（1：FAN制御、2:LED点灯制御、3：ゲーム）

// LovyanGFXインスタンス生成
static LGFX lcd;                                            // LGFXのインスタンスを作成。
static auto transpalette = 0;                               // 透過色パレット番号

String JsonData;                                            // JSON形式データの格納用
int sdstat = 0;                                             // microSDカードからのデータ読み出し状況（False:0,True:1）
StaticJsonDocument<192> n_jsondata;                         // JSON形式データ格納用メモリの確保
String i_ssid, i_pass;                                      // SSIDとパスワードの格納用
char buf[60];

// NTP接続情報　NTP connection information.
const char* NTPSRV      = "ntp.jst.mfeed.ad.jp";            // NTPサーバーアドレス NTP server address.
const long  GMT_OFFSET  = 9 * 3600;                         // GMT-TOKYO(時差９時間）9 hours time difference.
const int   DAYLIGHT_OFFSET = 0;                            // サマータイム設定なし No daylight saving time setting
struct tm timeInfo;                                         // tmオブジェクトをtimeinfoとして生成
int smin;

int TEMPSENSOR_PIN = 5;                                     // 水温センサーのデータ取得用GPIO
OneWire ds(TEMPSENSOR_PIN);                                 // 水温センサーの有効化
float sensor_value = 0.0;                                   // 水温センサー値の格納用

// 色指定
static int c_BLACK           = 0x0000;                        /*   0,   0,   0 */
static int c_NAVY            = 0x000F;                        /*   0,   0, 128 */
static int c_DARKGREEN       = 0x03E0;                        /*   0, 128,   0 */
static int c_DARKCYAN        = 0x03EF;                        /*   0, 128, 128 */
static int c_MAROON          = 0x7800;                        /* 128,   0,   0 */
static int c_PURPLE          = 0x780F;                        /* 128,   0, 128 */
static int c_OLIVE           = 0x7BE0;                        /* 128, 128,   0 */
static int c_LIGHTGREY       = 0xC618;                        /* 192, 192, 192 */
static int c_DARKGREY        = 0x7BEF;                        /* 128, 128, 128 */
static int c_BLUE            = 0x001F;                        /*   0,   0, 255 */
static int c_GREEN           = 0x07E0;                        /*   0, 255,   0 */
static int c_CYAN            = 0x07FF;                        /*   0, 255, 255 */
static int c_RED             = 0xF800;                        /* 255,   0,   0 */
static int c_MAGENTA         = 0xF81F;                        /* 255,   0, 255 */
static int c_YELLOW          = 0xFFE0;                        /* 255, 255,   0 */
static int c_WHITE           = 0xFFFF;                        /* 255, 255, 255 */
static int c_ORANGE          = 0xFD20;                        /* 255, 165,   0 */
static int c_GREENYELLOW     = 0xAFE5;                        /* 173, 255,  47 */
static int c_PINK            = 0xF81F;

// ------------------------------------------------------------
// 水温メーター画面描画関数 
// ------------------------------------------------------------
void draw_sprite_mater_lcd(float temp_value) {

  // 水温メーターレベルゲージ 画像の表示
  lcd.pushImage(0, 0, templevel_Width,templevel_Height, templevel);

  int i_point_y =  120 - (temp_value-22.0)*5;
  
  // 水温メーターポインタ の表示
  lcd.fillRect(120, 0, 180, 240, c_BLACK);              // 表示域を塗りつぶし
  lcd.pushImage(70, i_point_y, temppoint_Width,temppoint_Height, temppoint, transpalette);
  
  // 水温メーター温度数値 の表示
  lcd.setTextColor(c_GREEN, c_BLACK);                   // テキストカラーの指定
  lcd.setTextFont(&fonts::Font8);                       // フォントの設定
  lcd.setTextDatum(top_left);                           // テキスト座標指定
  if(i_point_y-25 < 0) {
    lcd.setCursor(170, 0);                              // カーソル位置
  }
  else if(i_point_y > 180) {
    lcd.setCursor(170, 140);                            // カーソル位置
  }
  else {
    lcd.setCursor(170, i_point_y-25);                   // カーソル位置
  }
  lcd.print(int(temp_value));                           // 水温（数値）の描画

  // 水温メーター温度単位 の表示
  lcd.fillRect(290, 0, 50, 240, c_BLACK);               // 表示域を塗りつぶし
  lcd.setTextColor(c_YELLOW, c_BLACK);                  // テキストカラーの指定
  lcd.setTextFont(&fonts::lgfxJapanGothic_24);          // フォントの設定
  lcd.setTextDatum(top_left);                           // テキスト座標指定
  if(i_point_y+25 < 0) {
    lcd.setCursor(290, 50);                             // カーソル位置
  }
  else if(i_point_y+25 > 190) {
    lcd.setCursor(290, 215);                            // カーソル位置
  }
  else {
    lcd.setCursor(290, i_point_y+25);                   // カーソル位置
  }
  lcd.print("℃");                                      // 水温（単位）の描画

  // sprite6 注意表示プライト作成（On memory）
  if (temp_value > 26 || temp_value < 19 ) {
    lcd.pushImage(0, 0, caution_Width,caution_Height, caution);
  }
}
// ------------------------------------------------------------
// 時刻同期 関数　Time synchronization function.
// ------------------------------------------------------------
void time_sync(const char* ntpsrv, long gmt_offset, int daylight_offset) {

  configTime(gmt_offset, daylight_offset, ntpsrv);          // NTPサーバと同期

  if (getLocalTime(&timeInfo)) {                            // timeinfoに現在時刻を格納
    Serial.print("NTP : ");                                 // シリアルモニターに表示
    Serial.println(ntpsrv);                                 // シリアルモニターに表示

    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d\n",         // 表示内容の編集
    timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
    timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    Serial.println(buf);                                    // シリアルモニターに表示
  }
  else {
    Serial.print("NTP Sync Error ");                        // シリアルモニターに表示
  }
}

// ------------------------------------------------------------
// microSDカードからのSSID/PASS取得 関数
// ------------------------------------------------------------
void get_wifi_info() {
  if (!SD.begin()) {                                        // SDカードの初期化
    Serial.println("Card failed, or not present");          // シリアルコンソールへの出力
    while (1);
  }
  Serial.println("microSD card initialized.");              // シリアルコンソールへの出力

  if ( SD.exists("/SSID.txt")) {                            // ファイルの存在確認（SSID.ini）
    Serial.println("SSID.txt exists.");                     // ファイルがある場合の処理
    delay(500);
    File myFile = SD.open("/SSID.txt", FILE_READ);          // 読み取り専用でファイルを開く

    if (myFile) {                                           // ファイルが正常に開けた場合
      Serial.println("/SSID.txt Content:");
      while (myFile.available()) {                          // ファイル内容を順に変数に格納
        JsonData.concat(myFile.readString());
      }
      myFile.close();                                       // ファイルのクローズ
      sdstat = 1;
    } else {
      Serial.println("error opening /SSID.txt");
      sdstat = 0;
    }
  } else {
    Serial.println("SSID.txt doesn't exit.");               // シリアルコンソールへの出力
    sdstat = 0;
  } 
}

// ------------------------------------------------------------
// JSON形式データからのSSID/PASS読み出し 関数
// ------------------------------------------------------------
void read_json_wifi_info(){
  DeserializationError error = deserializeJson(n_jsondata, JsonData); // JSON形式データの読み込み
  
  if (error) {                                              // JSON形式データ読み込みエラーの場合
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());                          // エラーメッセージのディスプレイ表示
  } else {                                                  // 正常な場合
    i_ssid = n_jsondata["ssid"].as<String>();               // "ssid"の値を取得             
    i_pass = n_jsondata["pass"].as<String>();               // "pass"の値を取得

    Serial.println("Can read from JSON Data!");             // シリアルコンソールへの出力
    Serial.print("ID: ");
    Serial.println(i_ssid);                                 // "ssid"の値をディスプレイ表示
    Serial.print("PW: ");
    Serial.println(i_pass);                                 // "pass"の値をディスプレイ表示
  }
}

// ------------------------------------------------------------
// Wi-Fiアクセスポイントへの接続 関数
// ------------------------------------------------------------
void connect_wifi(){
  Serial.print("Conecting Wi-Fi ");                         // シリアルコンソールへの出力

  char buf_ssid[33], buf_pass[65];                          // SSID,パスワードをChar型へ変更
  i_ssid.toCharArray(buf_ssid, 33);
  i_pass.toCharArray(buf_pass, 65);

  WiFi.begin(buf_ssid, buf_pass);                           // Wi-Fi接続開始
  // Wi-Fi接続の状況を監視（WiFi.statusがWL_CONNECTEDになるまで繰り返し 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); } 
  
  Serial.print("");                                         // Wi-Fi接続結果をディスププレイへ出力
  Serial.println("WiFi connected"); 
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());                           // IPアドレスをディスププレイへ出力
}

// ------------------------------------------------------------
// 水温センサーデータのロギング関数
// ------------------------------------------------------------
void temp_sensor_logging() {

  if (!SD.begin()) {                                        // SDカードの初期化
    Serial.println("Card failed, or not present");          // シリアルコンソールへの出力
    while (1);
  }
  if ( SD.exists("/TempSensor_log.txt")) {                  // ファイルの存在確認（SSID.ini）
    Serial.println("/TempSensor_log.txt exists.");           // ファイルがある場合の処理
    delay(500);
    File myFile = SD.open("/TempSensor_log.txt", FILE_APPEND);// 追記モードでファイルを開く

    if (myFile) {                                           // ファイルが正常に開けた場合
      if (getLocalTime(&timeInfo)) {                        // timeinfoに現在時刻を格納
        delay(500);
        sensor_value = getTemp();
        draw_sprite_mater_lcd(sensor_value);
        String date_str = (String)(timeInfo.tm_year + 1900)
                          + "/" + (String)(timeInfo.tm_mon + 1)
                          + "/" + (String)timeInfo.tm_mday;
        String time_Str = (String)timeInfo.tm_hour
                          + ":" + (String)timeInfo.tm_min
                          + ":" + (String)timeInfo.tm_sec;
        myFile.println(date_str + " " + time_Str + "," + sensor_value);
      }
      myFile.close();                                       // ファイルのクローズ
      sdstat = 1;
    } else {
      Serial.println("error opening /TempSensor_log.txt");
      sdstat = 0;
    }
  } else {
    Serial.println("/TempSensor_log.txt doesn't exit.");               // シリアルコンソールへの出力
    sdstat = 0;
  }
}

//--------------------------------------------------------------
// 水温センサーからの計測値取得　関数 
//--------------------------------------------------------------
float getTemp(){
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  for (int i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}

//--------------------------------------------------------------
// 初期設定　関数 
//--------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  M5.begin();                                               // M5Stack初期化
  Serial.begin(115200);                                     // シリアル接続の開始
  delay(500);                                               // 待ち

  lcd.init();

  // 初期画面生成
  lcd.setRotation(display_rotation);                        // 画面の向きを変更

  lcd.fillScreen(BLACK);                                    // 画面の塗りつぶし　Screen fill.
  lcd.setTextColor(WHITE, BLACK);                           // テキストカラーの設定

  StaticJsonDocument<192> n_jsondata;                       // JSON形式データ格納用メモリの確保
  
  get_wifi_info();                                          // microSDカードからWi-Fi情報の取得

  if (sdstat == 1) {
    read_json_wifi_info();                                  // JSON形式データの読み出し
    connect_wifi();                                         // Wi-Fiアクセスポイントへの接続
  }
  time_sync(NTPSRV, GMT_OFFSET, DAYLIGHT_OFFSET);           // 時刻同期関数

}

//--------------------------------------------------------------
// Loop　関数 
//--------------------------------------------------------------
void loop() {
  M5.update();                                              // M5状態更新　M5 status update.
  temp_sensor_logging();                                    // 時計表示　Clock display.
  delay(60000);                                             // 60秒に一回測定
}
