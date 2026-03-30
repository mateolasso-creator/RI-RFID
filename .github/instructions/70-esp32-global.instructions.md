---
description: ESP32 global principles, project structure and intent
applyTo: "**"
---

intent: |
  Generate robust, secure, and efficient C code for ESP32 (ESP-IDF/FreeRTOS).
  Manage the build environment using secure Docker containers.
  Enforce standard project structure and strict version control ignore rules.

context: |
  You are an expert in Embedded Systems (ESP32), C11, FreeRTOS, and DevSecOps.
  The workflow involves writing firmware in C, testing with Unity, and building
  artifacts inside reproducible Docker containers.

instructions: |
  ## 🌐 Principios Globales
  - **Lenguaje:** C estricto (Standard C11). No C++ salvo indicación explícita.
  - **Calidad:** Código limpio, SOLID, KISS. Sin "magic numbers".
  - **Seguridad:** Validación de inputs, gestión segura de memoria y secretos.
  - **Documentación:** Comentarios explicativos (el "por qué") y formato Doxygen.

  ## 📂 Estructura del Proyecto (Mandatoria)
  Copilot debe asumir y respetar esta estructura de carpetas estándar ESP-IDF:
  ```text
  / (root)
  ├── .gitignore             # Reglas estrictas de exclusión
  ├── .dockerignore          # Contexto limpio para Docker
  ├── Dockerfile             # Entorno de compilación
  ├── docker-compose.yml     # Orquestación y Secrets
  ├── CMakeLists.txt         # Build system raíz
  ├── secrets/               # 🔒 Certificados y claves (EXCLUIDO de git)
  ├── main/                  # Código principal de la aplicación
  │   ├── CMakeLists.txt
  │   ├── main.c             # Entry point (app_main)
  │   └── include/           # Headers públicos de main
  ├── components/            # Módulos reutilizables desacoplados
  │   └── [nombre_componente]/
  │       ├── CMakeLists.txt
  │       ├── include/
  │       └── src/
  └── test/                  # Tests unitarios/integración globales
  ```
