---
applyTo: '**'
---
# 🧠 Instrucciones globales para GitHub Copilot

## 🎯 Propósito general
Este proyecto combina **Python**, **Docker** y **Bash** para crear un sistema automatizado, portable y seguro.  
Copilot debe priorizar **claridad, mantenibilidad, cobertura de pruebas y seguridad**.

### Directrices globales
- Siempre aplicar **principios SOLID**, **Clean Code** y **KISS (Keep It Simple, Stupid)**.
- Promover la **automatización del testing y CI/CD**, **linting automático**, y **formateo estándar** (`black`, `flake8`, `shellcheck`).  
- Evitar duplicación, redundancia y dependencias innecesarias.  
- Sugerir **tests unitarios** y **documentación** al generar nuevas funciones.  
- Los nombres deben ser expresivos y los comentarios deben explicar el *por qué*, no el *cómo*.  
- Usar inglés para nombres de variables, funciones y comentarios.  
- Mantener consistencia entre código, Docker y scripts.

# 🚫 Restricciones globales

### No permitido
- No incluir secretos ni credenciales.  
- No usar `print()` como logger.  
- No mezclar lógica de negocio con configuración.  
- No crear archivos temporales sin limpieza.  
- No usar dependencias obsoletas.  

# ✅ Ejemplo de estilo esperado
```python
from fastapi import FastAPI
from loguru import logger

app = FastAPI()

@app.get("/health")
def health_check():
    """Endpoint de salud del servicio API."""
    logger.info("Health check OK")
    return {"status": "ok"}
```

# 📦 Mantenimiento y actualización

- Reconstruir imágenes tras cambios de dependencias (`docker compose build --no-cache`).  
- Actualizar dependencias con `pip-tools` o `poetry update`.  
- Analizar vulnerabilidades con `pip-audit` o `safety`.  
- Mantener actualizado `CHANGELOG.md`.  
- Documentar cambios en `README.md` y `docs/`.
