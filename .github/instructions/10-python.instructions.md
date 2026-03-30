---
applyTo: '*.py'
---
# 🐍 Reglas para archivos Python

### Estilo y estructura
- Cumplir con **PEP8** y **PEP257**.  
- Usar **Python 3.11+**.  
- Tipado estático con `typing` y validación con `mypy`.  
- Estructura recomendada:
  ```
  src/
    __init__.py
    main.py
    config.py
    utils/
    services/
    tests/
  ```
- Nombres:
  - funciones → `snake_case`
  - clases → `PascalCase`
  - constantes → `UPPER_CASE`
- Evitar funciones >50 líneas o archivos >200 líneas.

### Buenas prácticas
- Reemplazar `print()` por `logging`.  
- Manejar errores con `try/except` y logs estructurados.  
- No hardcodear rutas ni credenciales.  
- Escribir docstrings en formato Google o NumPy en inglés.  
- Dividir módulos grandes en subpaquetes.  
- Incluir pruebas unitarias en `tests/`.  
- Preguntarme para refactorizar cuando Copilot detecte redundancia.  
