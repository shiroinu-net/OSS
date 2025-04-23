#include <Arduino_LED_Matrix.h>
#include <SPI.h>
#include <WiFiS3.h>
#include <MFRC522.h>

// — Wi‑Fi設定 —
// テザリング（iPhone）と自宅 Wi‑Fi の２つを試します
const char* ssid1 = "1Ph0ne";
const char* pass1 = "smjf4etp26dut";
const char* ssid2 = "pacchan_net";
const char* pass2 = "patty0613";

// — サーバー情報 —
const char* serverHost = "katousaorinoMacBook-Pro.local";
const int port = 3000;
IPAddress serverIP;

// — RC522ピン定義 —
#define SS_PIN1   10
#define RST_PIN1   9
#define SS_PIN2    8
#define RST_PIN2   7

MFRC522   rfid1(SS_PIN1, RST_PIN1);
MFRC522   rfid2(SS_PIN2, RST_PIN2);
WiFiClient client;
ArduinoLEDMatrix matrix;

// — タイミング設定 —
const unsigned long tagTimeout    = 2200;
const unsigned long resetInterval = 2000;
unsigned long lastResetTime = 0;

// — タグ状態管理 —
String       currentUid1    = "";
String       currentUid2    = "";
unsigned long lastSeenTime1 = 0;
unsigned long lastSeenTime2 = 0;
bool         tagPresent1    = false;
bool         tagPresent2    = false;

// — LEDマトリクス用フレーム（ハート） —
uint32_t heartFrame[3] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};

// ✔／×／● 表示フレーム
uint32_t checkMark[3] = { 0x200400, 0x201000, 0xa02000 };
uint32_t crossMark[3] = { 0x8100000, 0x24090000, 0x8100000 };
uint32_t dotFrame [3] = { 0x0,       0x180000,   0x0       };

// — 表示制御 —
void showConnecting(){
  matrix.clear();
  for(int i=0;i<3;i++){
    matrix.loadFrame(dotFrame);
    delay(300);
    matrix.clear();
    delay(300);
  }
}
void showSuccess(){ matrix.loadFrame(checkMark); delay(500); matrix.clear(); }
void showFailure(){ matrix.loadFrame(crossMark); delay(1000); matrix.clear(); }
void showTagDetected(){ matrix.loadFrame(heartFrame); }
void clearMatrix(){ matrix.clear(); }

// — UIDを16進文字列化 —
String getUidString(MFRC522 &reader){
  String uid="";
  for(byte i=0;i<reader.uid.size;i++){
    if(reader.uid.uidByte[i]<0x10) uid += "0";
    uid += String(reader.uid.uidByte[i], HEX);
    if(i<reader.uid.size-1) uid += ":";
  }
  return uid;
}

// — サーバーへ送信 —
void sendUid(String uid, uint8_t readerId){
  client.stop();
  if(client.connect(serverIP, port)){
    client.print("GET /nfc?uid="+uid+"&reader="+String(readerId)+" HTTP/1.1\r\n");
    client.print("Host: "+String(serverHost)+"\r\nConnection: close\r\n\r\n");
    client.stop();
    // Serial.printf の代わりに Serial.print 系で出力
    Serial.print("送信完了 (reader ");
    Serial.print(readerId);
    Serial.print("): ");
    Serial.println(uid);
  } else {
    Serial.print("接続失敗 (reader ");
    Serial.print(readerId);
    Serial.println(")");
  }
}

// — Wi‑Fi接続＆動的名前解決 —
void connectToWiFi(){
  Serial.println("Wi‑Fi接続開始…");
  showConnecting();

  int attempt = 0;
  while(WiFi.status()!=WL_CONNECTED && attempt<5){
    Serial.print("① ");
    WiFi.begin(ssid1, pass1);
    delay(3000);
    if(WiFi.status()==WL_CONNECTED){
      Serial.println("iPhoneテザリング接続成功");
      break;
    }
    Serial.print("② ");
    WiFi.begin(ssid2, pass2);
    delay(3000);
    if(WiFi.status()==WL_CONNECTED){
      Serial.println("自宅Wi‑Fi接続成功");
      break;
    }
    Serial.println("再試行…");
    showConnecting();
    attempt++;
  }

  if(WiFi.status()!=WL_CONNECTED){
    Serial.println("❌ Wi‑Fi接続失敗");
    showFailure();
    while(true) delay(1000);
  }
  showSuccess();

  Serial.print("Local IP: "); Serial.println(WiFi.localIP());
// Wi-Fi接続直後、名前解決のあとに追加
if (WiFi.hostByName(serverHost, serverIP)) {
  Serial.print("Resolved server IP: "); Serial.println(serverIP);
} else {
  Serial.println("⚠️ mDNS 名が見つかりません。固定 IP にフォールバックします。");
  serverIP = IPAddress(192, 168, 68, 119);  // ← Mac の IP
  Serial.print("Fallback server IP: "); Serial.println(serverIP);
}
}

// — setup —
void setup(){
  Serial.begin(9600);

  // — SS/RST ピンを出力モード & HIGH —
  pinMode(SS_PIN2, OUTPUT);
  pinMode(RST_PIN2, OUTPUT);
  digitalWrite(SS_PIN2, HIGH);
  digitalWrite(RST_PIN2, HIGH);

  SPI.begin();

  // ■■ 手動トグル■■
  Serial.println("=== Reader2 手動トグルテスト ===");
  // CS をLOW→HIGH
  digitalWrite(SS_PIN2, LOW);
  delay(5);
  digitalWrite(SS_PIN2, HIGH);
  Serial.print("CS2 state: "); Serial.println(digitalRead(SS_PIN2));

  // RST をLOW→HIGH
  digitalWrite(RST_PIN2, LOW);
  delay(5);
  digitalWrite(RST_PIN2, HIGH);
  Serial.print("RST2 state: "); Serial.println(digitalRead(RST_PIN2));

  // PCD_Init 後 VersionReg 読み出し
  rfid2.PCD_Init();
  Serial.print("Ver2=0x"); 
  Serial.println(rfid2.PCD_ReadRegister(MFRC522::VersionReg), HEX);
  Serial.println("=== テスト終了 ===");
  
  SPI.begin();
  rfid1.PCD_Init();
  rfid2.PCD_Init();
  matrix.begin();
  matrix.clear();
  Serial.println("RFID Ready!");
  connectToWiFi();

    //— リーダー1 初期化 & バージョン確認 —//
  rfid1.PCD_Init();
  Serial.print("Reader 1 firmware version: 0x");
  Serial.println(rfid1.PCD_ReadRegister(MFRC522::VersionReg), HEX);

  //— リーダー2 用の SS/RST ピンを出力モードにして初期化 —//
  pinMode(SS_PIN2, OUTPUT);
  pinMode(RST_PIN2, OUTPUT);
  rfid2.PCD_Init();
  Serial.print("Reader 2 firmware version: 0x");
  Serial.println(rfid2.PCD_ReadRegister(MFRC522::VersionReg), HEX);

}

// — loop —
void loop(){
  unsigned long now = millis();

  // １台目リーダー処理
  if(rfid1.PICC_IsNewCardPresent() && rfid1.PICC_ReadCardSerial()){
    String uid = getUidString(rfid1);
    rfid1.PICC_HaltA();
    if(uid != currentUid1){
      currentUid1 = uid;
      sendUid(uid, 1);
      showTagDetected();
    }
    lastSeenTime1 = now;
    tagPresent1 = true;
  }
  if(tagPresent1 && now - lastSeenTime1 > tagTimeout){
    sendUid("none", 1);
    Serial.println("タグ離脱 (reader 1)");
    currentUid1 = "";
    tagPresent1 = false;
    clearMatrix();
  }

  // ２台目リーダー処理
  if(rfid2.PICC_IsNewCardPresent() && rfid2.PICC_ReadCardSerial()){
    String uid = getUidString(rfid2);
    rfid2.PICC_HaltA();
    if(uid != currentUid2){
      currentUid2 = uid;
      sendUid(uid, 2);
      showTagDetected();
    }
    lastSeenTime2 = now;
    tagPresent2 = true;
  }
  if(tagPresent2 && now - lastSeenTime2 > tagTimeout){
    sendUid("none", 2);
    Serial.println("タグ離脱 (reader 2)");
    currentUid2 = "";
    tagPresent2 = false;
    clearMatrix();
  }

  // タグ接触中のリセット
  if((tagPresent1||tagPresent2) && now - lastResetTime > resetInterval){
    if(tagPresent1){
      rfid1.PCD_Init();
      Serial.println("RC522リセット (reader 1)");
    }
    if(tagPresent2){
      rfid2.PCD_Init();
      Serial.println("RC522リセット (reader 2)");
    }
    lastResetTime = now;
  }

  delay(200);
}