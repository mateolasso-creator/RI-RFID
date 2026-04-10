# 📋 API RFID Door Lock - Documentación Completa

## 🎯 Resumen de Mejoras Implementadas

### API.PHP ✅ MEJORADO
Se ha transformado el archivo `api.php` de respuestas en **texto plano** a respuestas en **formato JSON estructurado**. Esto proporciona:

- **Mayor compatibilidad** con clientes HTTP (especialmente microcontroladores)
- **Información adicional** en cada respuesta (timestamp, mensajes descriptivos, datos extra)
- **Mejor seguridad** (validación de entrada más rigurosa)
- **Características avanzadas** (registro de IP, duración configurable del relé)

---

## 📨 Protocolo de Comunicación

### Request (desde ESP32)

```http
GET http://192.168.0.143/api.php?uid=A1B2C3D4 HTTP/1.1
Content-Type: application/json
```

| Parámetro | Tipo | Descrición | Ejemplo |
|-----------|------|-----------|---------|
| `uid` | string | UID de la tarjeta RFID (hexadecimal, 8-20 caracteres) | `A1B2C3D4` |

---

### Response (desde API)

#### ✅ Acceso Granted
```json
{
  "status": "GRANTED",
  "message": "Acceso permitido",
  "timestamp": "2026-04-02 15:02:37",
  "data": {
    "user_id": 1,
    "user_name": "Juan García",
    "duration_ms": 3000
  }
}
```

#### ❌ Acceso Denied
```json
{
  "status": "DENIED",
  "message": "UID no encontrado o inactivo",
  "timestamp": "2026-04-02 15:03:09",
  "data": {
    "duration_ms": 0
  }
}
```

#### ⚠️ Error en Servidor
```json
{
  "status": "ERROR",
  "message": "No se pudo conectar a la base de datos",
  "timestamp": "2026-04-02 15:03:45",
  "data": {
    "error": "..."
  }
}
```

---

## 🗄️ Base de Datos

### Tabla: `users`

```sql
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) UNIQUE NOT NULL,           -- UID de tarjeta RFID
    name VARCHAR(100) NOT NULL,                -- Nombre del usuario
    status ENUM('ACTIVE', 'INACTIVE', 'BLOCKED'),  -- Estado del acceso
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Estados:**
- `ACTIVE` - Acceso permitido
- `INACTIVE` - Deshabilitado (rechaza acceso)
- `BLOCKED` - Bloqueado por seguridad

---

### Tabla: `logs`

```sql
CREATE TABLE logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) NOT NULL,                  -- UID de la tarjeta
    access_status ENUM('GRANTED', 'DENIED', 'ERROR'),
    ip_address VARCHAR(45),                    -- IP del ESP32
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

## 📊 Flujo de Validación

```
┌─ ESP32 lee UID
│
├─ Envía: GET /api.php?uid=XXXXXXXX
│
└─ API Valida:
   ├─ ¿UID vacío? → DENIED
   ├─ ¿Formato inválido? → DENIED
   ├─ ¿Conexión BD fallida? → ERROR
   ├─ ¿UID no existe? → DENIED (registra intento)
   ├─ ¿Status = INACTIVE? → DENIED (registra intento)
   ├─ ¿Status = BLOCKED? → DENIED (registra intento)
   └─ ¿Status = ACTIVE? → GRANTED (registra acceso)
       └─ Responde duración del relé: 3000ms (configurable)
```

---

## 🔐 Características de Seguridad

1. **Prepared Statements** - Previene SQL Injection
2. **Validación de UID** - Solo acepta hexadecimal válido (8-20 caracteres)
3. **Control de Estado** - Solo ACTIVE puede acceder
4. **Registro de Intentos** - Todos los accesos se registran con IP
5. **Respuestas JSON** - No revela estructura de BD en errores

---

## 🧪 Pruebas Rápidas

### 1️⃣ UID Válido y Registrado (DEBE SER GRANTED)
```bash
curl "http://localhost:8080/api.php?uid=A1B2C3D4"
# Respuesta: {"status": "GRANTED", ...}
```

### 2️⃣ UID Válido pero No Registrado (DEBE SER DENIED)
```bash
curl "http://localhost:8080/api.php?uid=KKKKKKKK"
# Respuesta: {"status": "DENIED", "message": "UID no encontrado..."}
```

### 3️⃣ UID Inválido (DEBE SER DENIED)
```bash
curl "http://localhost:8080/api.php?uid=INVALID"
# Respuesta: {"status": "DENIED", "message": "Formato de UID inválido"}
```

### 4️⃣ Sin UID (DEBE SER DENIED)
```bash
curl "http://localhost:8080/api.php"
# Respuesta: {"status": "DENIED", "message": "UID vacío o no proporcionado"}
```

---

## 📱 Código Arduino Esperado

El ESP32 debe:

1. **Enviar:** `GET /api.php?uid=XXXXXXXX`
2. **Recibir:** Respuesta JSON
3. **Parsear:** Campo `"status"`
4. **Actuar:**
   - Si `status == "GRANTED"` → Activar relé por `duration_ms`
   - Si `status == "DENIED"` → Parpadear relé 3 veces (error)
   - Si `status == "ERROR"` → Parpadear relé 5 veces (error mayor)

**Ejemplo de parsing en Arduino:**
```cpp
String jsonResponse = http.getString();
int statusIndex = jsonResponse.indexOf("\"status\"");
int firstQuote = jsonResponse.indexOf('\"', statusIndex + 9);
int secondQuote = jsonResponse.indexOf('\"', firstQuote + 1);
String status = jsonResponse.substring(firstQuote + 1, secondQuote);

if (status == "GRANTED") {
  activateRelay(3000);
} else if (status == "DENIED") {
  blink_relay(3);
}
```

---

## 🐛 Solución de Problemas

| Problema | Causa | Solución |
|----------|-------|---------|
| API retorna 500 | MySQL no está listo | Esperar 10-15 segundos después de `docker-compose up` |
| "Connection refused" | BD no inicializada | Verificar que el volumen no existe: `docker-compose down -v` |
| Status siempre "DENIED" | UID no registrado o inactivo | Verificar tabla `users`: `SELECT * FROM users;` |
| IP no registra en logs | Problema de network | Verificar si foreign key está configurada correctamente |
| El relé no se activa | Parsing JSON falló | Revisar formato de respuesta en Monitor Serial |

---

## 🔄 Flujo Completo de Ejemplo

### Paso 1: Usuario pasa tarjeta RFID
```
Monitor Serial (ESP32):
UID detectado: A1B2C3D4
```

### Paso 2: ESP32 envía solicitud HTTP
```
Enviando solicitud a: http://192.168.0.143/api.php?uid=A1B2C3D4
```

### Paso 3: API consulta BD y responde
```
Base de datos encuentra: id=1, name="Juan García", status="ACTIVE"
Respuesta JSON: {"status": "GRANTED", ...}
```

### Paso 4: ESP32 parsea y ejecuta
```
Respuesta JSON: {"status":"GRANTED",...}
Status parseado: GRANTED
✓ Acceso CONCEDIDO
Duración del relé: 3000 ms
Activando relé...
[RELÉ ABIERTO POR 3 SEGUNDOS]
Relé desactivado
```

### Paso 5: Registro en BD
```
Tabla logs:
  uid = "A1B2C3D4"
  access_status = "GRANTED"
  ip_address = "192.168.0.100"
  timestamp = "2026-04-02 15:02:37"
```

---

## 📝 Ejemplo: Agregar Nuevo Usuario

```sql
-- Insertar nuevo usuario
INSERT INTO users (uid, name, status) VALUES 
('NEWYXYYZZ', 'Nuevo Usuario', 'ACTIVE');

-- Verificar que se insertó
SELECT * FROM users WHERE uid='NEWYXYYZZ';

-- Ahora el ESP32 puede usar este UID
```

---

## 🚀 Versión de API

- **Versión:** 2.0 (JSON)
- **Compatibilidad:** ESP32, Arduino, Cualquier cliente HTTP
- **Formato:** JSON (RFC 7159)
- **Seguridad:** Prepared Statements + Validación de entrada
- **Base de Datos:** MySQL 8.0
- **Estado:** ✅ Producción

---

**Última actualización:** 2 de abril de 2026  
**Responsable:** Sistema RFID Door Lock  
**Ambiente:** Docker Compose
