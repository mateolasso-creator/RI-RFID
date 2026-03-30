---
description: Reglas de estilo y buenas prácticas para archivos C y H (ESP-IDF)
applyTo: "*.c,*.h"
---

instructions: |
  # ⚡ Reglas para Firmware (C/ESP-IDF)

  ## Estilo y Convenciones

  - **Naming:**
      - Funciones/Variables: `snake_case` (ej. `sensor_read_data`).
      - Tipos: `snake_case_t` (ej. `wifi_config_t`).
      - Macros: `UPPER_CASE` (ej. `MAX_RETRIES`).
      - Prefijos: Usar prefijos de módulo (ej. `mqtt_init`).
  - **Headers:** Usar `#pragma once` en archivos `.h`.
  - **Estructura:** Archivos `.c` deben incluir su propio `.h` primero.

  ## Buenas Prácticas ESP-IDF & FreeRTOS

  - **Logging:** - ⛔ PROHIBIDO: `printf`.
      - ✅ OBLIGATORIO: `ESP_LOGI(TAG, "...")`, `ESP_LOGE(TAG, "...")`.
      - Definir `static const char *TAG = "ModuloName";` al inicio.
  - **Gestión de Errores:**
      - Usar `ESP_ERROR_CHECK()` SOLO en inicio/setup.
      - En runtime, verificar `if (err != ESP_OK)`. NUNCA ignorar errores.
  - **Memoria:**
      - Gestión manual rigurosa (`malloc`/`free`).
      - Preferir asignación estática (Static Allocation) en FreeRTOS.
      - Verificar punteros `NULL`.
  - **Concurrencia:**
      - Usar `vTaskDelay` en lugar de bucles vacíos.
      - Proteger recursos con `SemaphoreHandle_t`.

  ## Otros
  - Evitar "magic numbers"; usar `#define` o `const` bien nombrados.
  - Documentar funciones públicas con Doxygen.
