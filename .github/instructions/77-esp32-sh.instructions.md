---
description: Reglas para scripts shell relacionados con ESP-IDF
applyTo: "*.sh"
---

instructions: |
  # 💻 Reglas para Scripts Bash

  - `#!/usr/bin/env bash` y `set -euo pipefail`.
  - Verificar entorno ESP-IDF (`idf.py --version`) antes de ejecutar.

  ## Buenas prácticas
  - Evitar hardcodear rutas de usuario; usar variables de entorno.
  - Añadir `--help` y validación de argumentos básicos.
  - No almacenar secretos en scripts; leer desde `/run/secrets/` o variables.
