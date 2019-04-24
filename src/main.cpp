// Подключение:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND

#include <Arduino.h>
#include <Wire.h>             // Данная библиотека позволяет взаимодействовать с I2C/TWI устройствами
#include <RtcDS3231.h>        // Библиотека для часов на микросхеме DS3231
RtcDS3231<TwoWire> Rtc(Wire); // Создание экземпляра класса RtcDs3231

#define countof(a) (sizeof(a) / sizeof(a[0]))

// Функция для вывода даты и времени
void printDateTime(const RtcDateTime &dt)
{
  char datestring[20]; // Определение массива типа char
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring); // Вывод даты и времени в формате: "Дата/Месяц/Год Час:Минуты:Секунды"
}

void setup()
{
  Serial.begin(57600);                                    // Инициализация последовательного соединения и задание скорости передачи данных в бит/с(бод)
  Serial.print("compiled: ");                             // Трансляция
  Serial.print(__DATE__);                                 // Вывод даты прошивки микроконтроллера
  Serial.println(__TIME__);                               // Вывод времени прошивки микроконтроллера
  Rtc.Begin();                                            //
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); // Запись даты и времени в переменную класса
  printDateTime(compiled);                                // Вывод даты и времени
  Serial.println();                                       // Пропуск строки

  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      Serial.print("RTC communications error = "); // Вывод сообщение о ошибке
      Serial.println(Rtc.LastError());             // Вывод номер ошибки (Номер ошибки находится здесь: https://www.arduino.cc/en/Reference/WireEndTransmission)
    }
    else
    {
      // Распространённые причины:
      // 1) В первый раз вы запустили, а устройство еще не работало
      // 2) Батарея на устройстве разряжена или вообще отсутствует
      Serial.println("RTC lost confidence in the DateTime!"); // Вывод надписи о потери уверенности в правильности времени
      // Следующая строка устанавливает RTC на дату и время составления этого эскиза
      // Строка также сбросит допустимый флаг, если только у RTC не возникла проблема
      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now"); // Вывод надписи "RTC не была активна, начинаем сейчас"
    Rtc.SetIsRunning(true);                                       // Запуск RTC
  }

  RtcDateTime now = Rtc.GetDateTime(); // Запись в переменную значения даты и времени RTC
  // Сравнение "даты и времени" текущего и "даты и времени" прошивки
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)"); // Вывод надписи "Данные RTC старее, чем время прошивки (Обновление даты и времени)"
    Rtc.SetDateTime(compiled);                                              // Обновление даты и времени
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)"); // Вывод надписи "Данные RTC новее, чем время прошивки (Это ожидаемое поведение)"
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)"); // Вывод надписи "данные RTC совпадают с временем прошивки(Не ожидаемое поведение, но всё работает)"
  }

  // Очистка RTC до нужного состояния
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

void loop()
{
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      Serial.print("RTC communications error = "); // Вывод сообщение о ошибке
      Serial.println(Rtc.LastError());             // Вывод номер ошибки (Номер ошибки находится здесь: https://www.arduino.cc/en/Reference/WireEndTransmission)
    }
    else
    {
      // Распространённые причины:
      //    1) Батарея на устройстве разряжена или даже отсутствует, и электропитание отключено
      Serial.println("RTC lost confidence in the DateTime!"); // Вывод надписи о потери уверенности в правильности времени
    }
  }
  RtcDateTime now = Rtc.GetDateTime();        // Получение текущих даты и времени RTC
  printDateTime(now);                         // Вывод на экран текущих даты и времени
  Serial.println();                           // Пропуск строки
  RtcTemperature temp = Rtc.GetTemperature(); // Получение температуры в в Цельсиях
  Serial.print(temp.AsFloatDegC());           // Вывод температуры в формате "00.00"
  Serial.println("C");                        // Вывод символа, обозначающего Цельсии
  delay(10000);                               // Задержка 10с
}
