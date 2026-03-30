---
description: Reglas para .gitignore en proyectos ESP32
applyTo: ".gitignore"
---

instructions: |
  # 🚫 Reglas para .gitignore

  - Asegurar que el archivo exista y contenga SIEMPRE:
      - `build/` (Artefactos de compilación)
      - `managed_components/` (Dependencias descargadas)
      - `sdkconfig` (Configuración local con secretos potenciales)
      - `secrets/` (Credenciales reales)
      - `.vscode/` y `*.log`

  ## Plantilla sugerida (usar tal cual salvo ajustar rutas privadas)
  ```gitignore
  # ESP-IDF
  /build/
  /managed_components/
  sdkconfig
  sdkconfig.old
  dependencies.lock

  # Python / System
  __pycache__/
  *.pyc
  .DS_Store
  *.log

  # IDE
  .vscode/
  .idea/

  # Security
  /secrets/*
  !/secrets/.keep
  *.pem
  *.key
  ```
