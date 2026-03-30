---
description: Plantillas de archivos `.gitignore` y `.dockerignore` recomendadas
applyTo: "**"
---

instructions: |
  ### Plantilla: `.gitignore`
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

  ### Plantilla: `.dockerignore`
  ```dockerignore
  # Version control
  .git
  .gitignore

  # Build artifacts (Don't copy host builds to container)
  build/
  managed_components/
  dependencies.lock

  # Configuration & Secrets (Don't bake into image, use volumes/secrets)
  sdkconfig
  sdkconfig.old
  secrets/
  .env

  # IDE & Instruction files
  .vscode/
  .cursorrules
  .github/
  README.md
  ```
