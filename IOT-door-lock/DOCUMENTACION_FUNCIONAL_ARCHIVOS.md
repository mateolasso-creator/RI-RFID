# Documentacion funcional de archivos del proyecto IOT-door-lock

## Objetivo del documento
Este archivo explica la funcionalidad de cada archivo principal del proyecto, como interactuan entre si y cual es el comportamiento esperado del sistema completo.

## 1) Archivos de infraestructura

### docker-compose.yml
- Levanta dos servicios: web (PHP + Apache) y db (MySQL 8.0).
- Publica la aplicacion web en el puerto 8088 del host.
- Monta el codigo PHP desde src/php dentro del contenedor web.
- Inicializa la base con init.sql en el arranque de MySQL.

### Dockerfile
- Construye la imagen del servicio web basada en php:8.1-apache.
- Instala extensiones necesarias para MySQL (mysqli, pdo, pdo_mysql).
- Habilita mod_rewrite en Apache.
- Copia el codigo PHP al directorio web.

### init.sql
- Crea y prepara la base rfid_access.
- Crea tablas users y logs.
- Inserta datos semilla de usuarios.
- Incluye el UID autorizado principal con nombre Grupo3.

## 2) Archivos de backend PHP

### src/php/api.php
- Es el endpoint que recibe UID desde ESP32 por HTTP GET.
- Valida el UID, consulta users y responde JSON.
- Responde:
  - GRANTED si el UID existe y esta ACTIVE.
  - DENIED si el UID no existe o no esta activo.
  - ERROR si ocurre un fallo interno.
- Registra todos los intentos en logs.
- Ajusta zona horaria para mostrar hora local consistente.
- Para UID no registrado devuelve user_name = Desconocido.

### src/php/admin.php
- Panel de administracion protegido por sesion.
- Permite:
  - Agregar y eliminar badges.
  - Ver historial de accesos.
  - Exportar historial a CSV.
- Implementa actualizacion automatica del historial con AJAX (sin recargar pagina).
- Muestra la ultima hora de actualizacion del panel.

### src/php/index.php
- Pantalla de login de administrador.
- Crea sesion y redirige al panel admin.php cuando las credenciales son validas.

### src/php/.htaccess
- Define reglas de reescritura para enrutar solicitudes hacia index.php cuando aplica.
- Mantiene comportamiento consistente de URLs dentro de Apache.

## 3) Archivo de firmware ESP32

### src/arduino/code-iot-MEJORADO/code-iot-MEJORADO.ino
- Lee tarjetas RFID con MFRC522 por SPI.
- Envia el UID detectado al backend api.php.
- Parsea respuesta JSON (status, user_name, duration_ms).
- Comportamiento por estado:
  - GRANTED: activa rele por duration_ms.
  - DENIED: no abre puerta y hace parpadeo de error.
  - ERROR: indica error de servidor.
- Usa LED RGB integrado de la placa para indicar estado de puerta:
  - Verde: puerta abierta.
  - Rojo: puerta cerrada.
- Usa la URL del backend en puerto 8088.

## 4) Archivos de apoyo funcional

### API_DOCUMENTATION.md
- Describe contrato de la API (requests y responses JSON).
- Ayuda a depurar integracion entre ESP32 y backend.

### README.md
- Describe el proyecto, arquitectura y uso general.

## 5) Flujo tecnico de funcionamiento
1. ESP32 detecta un UID con el lector RFID.
2. ESP32 envia GET a /api.php?uid=... en el servidor.
3. API valida el UID y consulta tabla users.
4. API responde JSON con status y datos.
5. ESP32 decide abrir o denegar segun status.
6. API registra intento en tabla logs con hora local.
7. Panel admin actualiza historial automaticamente cada 3 segundos.

## Funcionamiento correcto esperado (resultado final)
- El servicio web debe estar activo en http://localhost:8088.
- El ESP32 debe apuntar al endpoint correcto (IP del host + :8088 + /api.php).
- UID autorizado 1117E7A9 debe mostrar:
  - Acceso concedido.
  - Nombre Grupo3.
  - Apertura de puerta por la duracion configurada.
- UID 09162D1F y cualquier UID no registrado debe mostrar:
  - Acceso denegado.
  - Nombre Desconocido.
  - Sin apertura de puerta.
- El historial en admin.php debe actualizarse en tiempo real sin recargar manualmente.
- Las horas del historial y la API deben verse en hora local.
