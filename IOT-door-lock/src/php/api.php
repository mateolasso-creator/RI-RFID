<?php
/**
 * API RFID Door Lock - api.php (Versión Corregida para Docker)
 */

// ============================================
// CONFIGURACIÓN DE ERRORES Y CABECERAS
// ============================================
// Cambia display_errors a 0 cuando todo funcione perfectamente
ini_set('display_errors', 1); 
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

header('Content-Type: application/json; charset=utf-8');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');

// Zona horaria local del proyecto
define('APP_TIMEZONE', 'America/Lima');
define('DB_TIMEZONE_OFFSET', '-05:00');
date_default_timezone_set(APP_TIMEZONE);

// Configuración de base de datos para Docker
define('DB_HOST', 'db'); 
define('DB_USER', 'root');
define('DB_PASS', 'rootpassword');
define('DB_NAME', 'rfid_access');

// ============================================
// FUNCIONES DE UTILIDAD
// ============================================

function respond($status, $message = '', $data = []) {
    $response = [
        'status' => $status,
        'message' => $message,
        'timestamp' => date('Y-m-d H:i:s'),
        'data' => $data
    ];
    echo json_encode($response, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
    exit;
}

function logAccess($db, $uid, $status, $ip = '') {
    if (empty($ip)) {
        $ip = $_SERVER['REMOTE_ADDR'] ?? 'UNKNOWN';
    }
    
    // Dejamos que la BD maneje el timestamp automáticamente
    $stmt = $db->prepare("INSERT INTO logs (uid, access_status, ip_address) VALUES (?, ?, ?)");
    if ($stmt) {
        $stmt->bind_param('sss', $uid, $status, $ip);
        $stmt->execute();
        $stmt->close();
    }
}

// ============================================
// PROCESAMIENTO DE SOLICITUD
// ============================================

// Capturar UID y limpiar espacios
$uid = $_REQUEST['uid'] ?? '';
$uid = strtoupper(trim($uid));

if (empty($uid)) {
    respond('DENIED', 'UID vacío o no proporcionado');
}

// Validación de longitud (más flexible para evitar Error 500)
if (strlen($uid) < 4 || strlen($uid) > 32) {
    respond('DENIED', 'Longitud de UID inválida');
}

// ============================================
// CONEXIÓN Y CONSULTA
// ============================================

try {
    $db = new mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    $db->set_charset('utf8mb4');
    $db->query("SET time_zone = '" . DB_TIMEZONE_OFFSET . "'");

    // Consultar si el usuario existe y está activo
    $query = $db->prepare("SELECT id, name, status FROM users WHERE uid = ?");
    $query->bind_param('s', $uid);
    $query->execute();
    $result = $query->get_result();

    $ip = $_SERVER['REMOTE_ADDR'] ?? 'UNKNOWN';

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        
        if ($row['status'] === 'ACTIVE') {
            logAccess($db, $uid, 'GRANTED', $ip);
            respond('GRANTED', 'Acceso permitido', [
                'user_name' => $row['name'],
                'duration_ms' => 3000
            ]);
        } else {
            logAccess($db, $uid, 'DENIED', $ip);
            respond('DENIED', 'Usuario inactivo o bloqueado', [
                'user_name' => $row['name'],
                'duration_ms' => 0
            ]);
        }
    } else {
        logAccess($db, $uid, 'DENIED', $ip);
        respond('DENIED', 'Tarjeta no registrada', [
            'user_name' => 'Desconocido',
            'duration_ms' => 0
        ]);
    }

    $query->close();
    $db->close();

} catch (Exception $e) {
    http_response_code(500);
    respond('ERROR', 'Error interno del servidor', ['details' => $e->getMessage()]);
}
?>