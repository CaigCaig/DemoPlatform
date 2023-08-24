#ifndef __MOD_CONFIG_H
#define __MOD_CONFIG_H

//#define FW_VERSION              "сс.gen.2.1.6.a"

// Ключ для контроля сектора настроек
#define SETTINGS_KEY            123456789

// Базовый адрес сектора настроек
#define SETTINGS_BASE_ADDRESS   0x08010000

// Адрес контрольного слова для структуры настроек во флеш памяти
#define SETTINGS_KEY_ADDRESS    0x0801FFFC

// TCP - JSON протокол
#define TCP_PROTOCOL        0

// MQTT
//#define MQTT_PROTOCOL     1

#endif  // __MOD_CONFIG_H

