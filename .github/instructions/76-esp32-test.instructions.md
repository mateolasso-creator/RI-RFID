---
description: Reglas para tests (Unity) en proyectos ESP32
applyTo: "test/**"
---

instructions: |
  # 🧪 Reglas para Testing (Unity)

  - Usar framework **Unity**.
  - `TEST_CASE("descripcion", "[tag]")`.
  - Validar `TEST_ASSERT_EQUAL_ESP_ERR(ESP_OK, func())`.

  ## Recomendaciones
  - Mantener tests rápidos y deterministas.
  - Aislar dependencias de hardware con mocks o stubs.
  - Integrar en pipeline de CI con contenedores que soporten `idf.py`.
