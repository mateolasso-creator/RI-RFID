---
description: Reglas y plantilla para Dockerfile (ESP32 build image)
applyTo: "Dockerfile"
---

instructions: |
  # 🐳 Reglas para Dockerfile

  - **Base Image:** Usar `espressif/idf` (versión fija, ej. `release-v5.3`).
  - **Usuario:** Crear y usar usuario no-root (`USER appuser`).
  - **Build Context:** Asumir que `.dockerignore` filtra archivos basura.
  - **Comando:** Usar `ENTRYPOINT ["idf.py"]` y `CMD ["build"]`.

  ## Plantilla mínima
  ```Dockerfile
  FROM espressif/idf:release-v5.3
  
  # Create non-root user
  RUN groupadd -r appuser && useradd -r -g appuser -m -s /bin/bash appuser
  
  WORKDIR /project
  
  # Copy project (secrets are filtered by .dockerignore)
  COPY --chown=appuser:appuser . /project
  
  USER appuser
  
  ENTRYPOINT ["idf.py"]
  CMD ["build"]
  ```
