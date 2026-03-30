---
description: Reglas para docker-compose.yml en proyectos ESP32
applyTo: "docker-compose.yml"
---

instructions: |
  # ⚙️ Reglas para Docker Compose

  - **Secretos:** Usar bloques `secrets:` montados en `/run/secrets/` (ej. `wifi_creds`).
  - **Hardware:** Mapear dispositivos seriales si es necesario (`devices: ["/dev/ttyUSB0"]`).
  - **Volúmenes:** Montar el código fuente (`.:/project`) y cache (`ccache_data:/root/.ccache`).

  ## Plantilla sugerida
  ```yaml
  version: '3.8'
  
  services:
    builder:
      build: .
      image: esp32-builder:local
      volumes:
        - .:/project:cached
        - ccache_data:/root/.ccache
      devices:
        - "/dev/ttyUSB0:/dev/ttyUSB0"
      secrets:
        - wifi_creds
      environment:
        - IDF_PATH=/opt/esp/idf
      tty: true
  
  volumes:
    ccache_data:
  
  secrets:
    wifi_creds:
      file: ./secrets/wifi_creds
  ```
