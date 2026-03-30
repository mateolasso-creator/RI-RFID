---
description: Reglas para .dockerignore en proyectos ESP32
applyTo: ".dockerignore"
---

instructions: |
  # 🐳 Reglas para .dockerignore

  - Minimizar el contexto de build para seguridad y velocidad. Excluir:
      - `.git`
      - `build/`
      - `managed_components/`
      - `secrets/` (Los secretos se montan, NO se copian en el build context)
      - `test/` (Salvo que sea un target de test específico)

  ## Plantilla sugerida
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
