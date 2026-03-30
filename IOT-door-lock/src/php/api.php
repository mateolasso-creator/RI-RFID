<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
header('Content-Type: text/plain');

// Conexión a la base de datos MySQL
$db = new mysqli('db', 'root', 'rootpassword', 'rfid_access');

// Verificar si la conexión falló
if ($db->connect_error) {
    die("Error al conectar con la base de datos: " . $db->connect_error);
}

// Obtener el UID enviado por el ESP32 (mediante GET)
$uid = $_GET['uid'] ?? '';

if (empty($uid)) {
    die("DENIED"); // UID vacío = acceso denegado
}

// Verificar si el UID existe en la tabla 'users'
$query = $db->prepare("SELECT id FROM users WHERE uid = ?");
$query->bind_param('s', $uid);
$query->execute();
$result = $query->get_result();

if ($result->num_rows > 0) {
    // UID encontrado: acceso permitido
    echo "GRANTED";

    // Registrar el acceso (acceso exitoso)
    $logQuery = $db->prepare("INSERT INTO logs (uid, access_status) VALUES (?, 'GRANTED')");
    $logQuery->bind_param('s', $uid);
    $logQuery->execute();
} else {
    // UID desconocido: acceso denegado
    echo "DENIED";

    // Registrar el acceso (acceso denegado)
    $logQuery = $db->prepare("INSERT INTO logs (uid, access_status) VALUES (?, 'DENIED')");
    $logQuery->bind_param('s', $uid);
    $logQuery->execute();
}

$db->close();
?>
