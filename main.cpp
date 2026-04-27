/*
 * main.cpp: 애플리케이션 엔트리 포인트로, setup()에서 초기화하고 loop()에서 LED 테스트와 LTE 업데이트를 수행합니다.
 *
 * 전체 FW 구조 다이어그램 (ASCII 아트):
 *
 * +-------------------+     +-------------------+     +-------------------+
 * |       App         |     |   LED_Module      |     |   LTE_Module      |
 * |                   |     |                   |     |                   |
 * |   main.cpp        | --> |   LED.h           |     |   LTE.h           |
 * |                   |     |   LED.cpp         | --> |   LTE.cpp         |
 * +-------------------+     +-------------------+     |   LTECommands.h   |
 *                                                     |   LTECommands.cpp |
 *                                                     +-------------------+
 *                                                            |
 * +-------------------+     +-------------------+           v
 * |   Serial_Module   |     |   Adapters        |     +-------------------+
 * |                   |     |                   |     |   Hardware HAL    |
 * |   Serial.h        | --> | HardwareSerialAdap|     |                   |
 * |   Serial.cpp      |     | ArduinoGPIOAdapter| <-- | ESP32-S3 GPIO    |
 * +-------------------+     | ArduinoLEDGPIOAdap|     | ESP32-S3 Serial  |
 *        |                  +-------------------+     +-------------------+
 *        |
 *        v
 * +-------------------+
 * | GPIO/Serial HAL   |
 * | ISerialPort       |
 * | ILTEGPIO          |
 * | ILEDGPIO          |
 * +-------------------+
 */

#include <Arduino.h>
#include "Serial.h"
#include "LED.h"
#include "LTE.h"
#include "BMP280.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>

void setup() {
  // USB CDC를 통한 디버깅 메시지
  Serial.begin(115200);
  //while (!Serial) delay(100); // 시리얼 모니터 대기 => usb 미연결 시 무한 대기 방지 위해 주석 처리
  ledController.setup();
  lteController.begin();

   // BMP280.cpp에 정의된 초기화 함수 호출
  initBMP280(); 

  // =========================================================
    // 🚀 FW 수정 여부를 확인할 수 있는 디버깅 메시지 출력
    // =========================================================
    Serial.println(F("\n======================================="));
    Serial.println(F("🤖 [시스템 시작] 고도 측정 모니터링 장치"));
    Serial.print(F("📅 컴파일 날짜: "));
    Serial.println(F(__DATE__)); // 예: "Apr 27 2026"
    Serial.print(F("⏰ 컴파일 시간: "));
    Serial.println(F(__TIME__)); // 예: "16:15:30"
    Serial.println(F("=======================================\n"));

}


void loop() {
  
  ledController.test(LED_BAT_ST_RED);
  ledController.test(LED_LTE_GREEN);
  lteController.update(); 
  
 // 1. 고도 리셋 신호가 들어왔는지 수시로 체크
    checkAltitudeReset();

    // 2. 0.5초마다 고도 값 측정 및 화면 출력
    static unsigned long lastMeasureTime = 0;
    if (millis() - lastMeasureTime >= 500) {
        lastMeasureTime = millis();
        updateAltitude();
    }
}


