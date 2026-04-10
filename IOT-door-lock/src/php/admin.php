<?php
session_start();

define('APP_TIMEZONE', 'America/Lima');
define('DB_TIMEZONE_OFFSET', '-05:00');
date_default_timezone_set(APP_TIMEZONE);

// Verificar autenticación
if (!isset($_SESSION['loggedin'])) {
    if (isset($_GET['ajax']) && $_GET['ajax'] === 'logs') {
        http_response_code(401);
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode(['error' => 'Unauthorized']);
        exit;
    }

    header('Location: /index.php');
    exit;
}

$db = new mysqli('db', 'root', 'rootpassword', 'rfid_access');
if ($db->connect_error) {
    die("Error de conexión MySQL: " . $db->connect_error);
}
$db->query("SET time_zone = '" . DB_TIMEZONE_OFFSET . "'");

// Endpoint AJAX para refrescar historial sin recargar la página
if (isset($_GET['ajax']) && $_GET['ajax'] === 'logs') {
    header('Content-Type: application/json; charset=utf-8');

    $logsData = [];
    $logs = $db->query("SELECT id, uid, access_status, timestamp FROM logs ORDER BY timestamp DESC LIMIT 100");
    while ($row = $logs->fetch_assoc()) {
        $logsData[] = $row;
    }

    echo json_encode([
        'updated_at' => date('Y-m-d H:i:s'),
        'logs' => $logsData,
    ], JSON_UNESCAPED_UNICODE);
    exit;
}

// Gestión de exportación a CSV
if (isset($_GET['export'])) {
    header('Content-Type: text/csv; charset=utf-8');
    header('Content-Disposition: attachment; filename="registro_accesos_'.date('Y-m-d').'.csv"');
    
    $output = fopen('php://output', 'w');
    fputcsv($output, ['ID', 'UID del Badge', 'Estado', 'Fecha y Hora'], ';');
    
    $logs = $db->query("SELECT * FROM logs ORDER BY timestamp DESC");
    while ($row = $logs->fetch_assoc()) {
        fputcsv($output, [
            $row['id'],
            $row['uid'],
            $row['access_status'] === 'GRANTED' ? 'Permitido' : 'Denegado',
            $row['timestamp']
        ], ';');
    }
    fclose($output);
    exit;
}

// Autenticación del administrador
if (isset($_POST['login'])) {
    if ($_POST['username'] === 'admin' && $_POST['password'] === 'admin123') {
        $_SESSION['loggedin'] = true;
    } else {
        $error = "¡Credenciales incorrectas!";
    }
}

// Déconnexion
if (isset($_GET['logout'])) {
    session_destroy();
    header("Location: /index.php");
    exit;
}

// Agregar un nuevo badge
if (isset($_POST['add_badge'])) {
    $uid = $db->real_escape_string($_POST['new_uid']);
    $name = $db->real_escape_string($_POST['user_name']);
    $db->query("INSERT INTO users (uid, name) VALUES ('$uid', '$name')");
    $success = "¡Badge agregado exitosamente!";
}

// Eliminar un badge
if (isset($_POST['delete_badge'])) {
    $id = intval($_POST['delete_id']);
    $db->query("DELETE FROM users WHERE id = $id");
    $success = "¡Badge eliminado exitosamente!";
}

// Control manual de la puerta
if (isset($_POST['open_door'])) {
    file_put_contents('door_command.txt', 'OPEN');
    $success = "¡Comandó de apertura enviado!";
}
?>

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Administración de Control de Acceso RFID</title>
    <style>
        :root {
            --primary: #4361ee;
            --secondary: #3f37c9;
            --success: #4cc9f0;
            --danger: #f72585;
            --light: #f8f9fa;
            --dark: #212529;
            --gray: #6c757d;
        }

        body {
            margin: 0;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f5f7fa;
        }

        .container {
            margin-left: 240px;
            padding: 30px;
        }

        nav.sidebar {
            position: fixed;
            top: 0;
            left: 0;
            width: 220px;
            height: 100vh;
            background-color: var(--dark);
            color: white;
            padding: 20px;
        }

        nav.sidebar h2 {
            font-size: 20px;
            margin-bottom: 30px;
        }

        nav.sidebar ul {
            list-style: none;
            padding: 0;
        }

        nav.sidebar li {
            margin-bottom: 20px;
        }

        nav.sidebar a {
            color: white;
            text-decoration: none;
        }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 30px;
        }

        h1 {
            color: var(--primary);
        }

        h2 {
            color: var(--secondary);
            margin-top: 30px;
        }

        h3 {
            margin-top: 20px;
            color: var(--gray);
        }

        .btn {
            background: var(--primary);
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 6px;
            cursor: pointer;
            text-decoration: none;
        }

        .btn:hover {
            background: var(--secondary);
        }

        .btn-danger {
            background: var(--danger);
        }

        .btn-danger:hover {
            background: #d1144a;
        }

        .btn-success {
            background: var(--success);
        }

        .btn-success:hover {
            background: #2ab4d6;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }

        th, td {
            padding: 12px;
            border-bottom: 1px solid #ddd;
        }

        th {
            background-color: var(--primary);
            color: white;
        }

        tr:nth-child(even) {
            background-color: #f9f9f9;
        }

        .section {
            margin-bottom: 50px;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.05);
        }

        .alert {
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 6px;
        }

        .alert-success {
            background-color: #d4edda;
            color: #155724;
        }

        .alert-danger {
            background-color: #f8d7da;
            color: #721c24;
        }

        input[type="text"], input[type="password"] {
            padding: 10px;
            width: 300px;
            margin-bottom: 10px;
            border-radius: 6px;
            border: 1px solid #ccc;
        }

        .login-container {
            max-width: 400px;
            margin: 100px auto;
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
        }

        .logs-meta {
            color: #6c757d;
            font-size: 13px;
            margin-top: 8px;
        }
    </style>
</head>
<body>

<?php if (!isset($_SESSION['loggedin'])): ?>
    <div class="login-container">
        <h2>Iniciar Sesión - Administrador</h2>
        <?php if (isset($error)): ?>
            <div class="alert alert-danger"><?= $error ?></div>
        <?php endif; ?>
        <form method="POST">
            <input type="text" name="username" placeholder="Nombre de usuario" required><br>
            <input type="password" name="password" placeholder="Contraseña" required><br>
            <button type="submit" name="login" class="btn">Iniciar Sesión</button>
        </form>
    </div>
<?php else: ?>

    <!-- BARRA LATERAL -->
    <nav class="sidebar">
        <h2>Menú Principal</h2>
        <ul>
            <li><a href="#badges">🎫 Badges</a></li>
            <li><a href="#logs">📋 Historial</a></li>
            <li><a href="#control">🔓 Control</a></li>
            <li><a href="?logout" class="btn btn-danger btn-sm">Cerrar Sesión</a></li>
        </ul>
    </nav>

    <!-- CONTENIDO PRINCIPAL -->
    <div class="container">
        <header>
            <h1>💳 Sistema de Control de Acceso RFID</h1>
        </header>

        <?php if (isset($success)): ?>
            <div class="alert alert-success"><?= $success ?></div>
        <?php endif; ?>

        <div class="section" id="badges">
            <h2>🎫 Gestión de Badges</h2>
            <form method="POST">
                <input type="text" name="new_uid" placeholder="UID del Badge" required>
                <input type="text" name="user_name" placeholder="Nombre del Usuario" required>
                <button type="submit" name="add_badge" class="btn btn-success">✅ Agregar Badge</button>
            </form>

            <h3>Badges Registrados</h3>
            <table>
                <thead>
                    <tr><th>ID</th><th>UID</th><th>Nombre del Usuario</th><th>Fecha de Registro</th><th>Acciones</th></tr>
                </thead>
                <tbody>
                    <?php
                    $badges = $db->query("SELECT * FROM users ORDER BY created_at DESC");
                    while ($row = $badges->fetch_assoc()):
                    ?>
                    <tr>
                        <td><?= $row['id'] ?></td>
                        <td><?= $row['uid'] ?></td>
                        <td><?= $row['name'] ?></td>
                        <td><?= $row['created_at'] ?></td>
                        <td>
                            <form method="POST" onsubmit="return confirm('¿Estás seguro de que deseas eliminar este badge?');">
                                <input type="hidden" name="delete_id" value="<?= $row['id'] ?>">
                                <button type="submit" name="delete_badge" class="btn btn-danger btn-sm">🗑️ Eliminar</button>
                            </form>
                        </td>
                    </tr>
                    <?php endwhile; ?>
                </tbody>
            </table>
        </div>

        <div class="section" id="logs">
            <h2>📋 Historial de Accesos</h2>
            <a href="?export" class="btn">📥 Descargar como CSV</a>
            <p class="logs-meta">Actualización automática cada 3 segundos. Última actualización: <span id="logs-updated-at">--</span></p>
            <table>
                <thead>
                    <tr><th>ID</th><th>UID</th><th>Estado</th><th>Fecha y Hora</th></tr>
                </thead>
                <tbody id="logs-body">
                    <?php
                    $logs = $db->query("SELECT * FROM logs ORDER BY timestamp DESC LIMIT 100");
                    while ($row = $logs->fetch_assoc()):
                    ?>
                    <tr>
                        <td><?= $row['id'] ?></td>
                        <td><?= $row['uid'] ?></td>
                        <td style="color: <?= $row['access_status'] === 'GRANTED' ? 'green' : 'red' ?>; font-weight: bold;">
                            <?= $row['access_status'] === 'GRANTED' ? '✅ Acceso Permitido' : '❌ Acceso Denegado' ?>
                        </td>
                        <td><?= $row['timestamp'] ?></td>
                    </tr>
                    <?php endwhile; ?>
                </tbody>
            </table>
        </div>

        <div class="section" id="control">
            <h2>🔐 Control Manual de la Puerta</h2>
            <p style="color: #666; margin-bottom: 15px;">Haz clic para enviar un comando de apertura a la puerta.</p>
            <form method="POST" onsubmit="return confirm('¿Deseas abrir la puerta ahora?');">
                <button type="submit" name="open_door" class="btn btn-success">🔓 Abrir Puerta</button>
            </form>
        </div>
    </div>
<?php endif; ?>

<script>
function escapeHtml(value) {
    return String(value)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#039;');
}

function renderLogRows(logs) {
    const body = document.getElementById('logs-body');
    if (!body) return;

    if (!logs || logs.length === 0) {
        body.innerHTML = '<tr><td colspan="4">Sin registros todavía</td></tr>';
        return;
    }

    body.innerHTML = logs.map((row) => {
        const isGranted = row.access_status === 'GRANTED';
        const statusText = isGranted ? '✅ Acceso Permitido' : '❌ Acceso Denegado';
        const color = isGranted ? 'green' : 'red';

        return '<tr>' +
            '<td>' + escapeHtml(row.id) + '</td>' +
            '<td>' + escapeHtml(row.uid) + '</td>' +
            '<td style="color: ' + color + '; font-weight: bold;">' + statusText + '</td>' +
            '<td>' + escapeHtml(row.timestamp) + '</td>' +
        '</tr>';
    }).join('');
}

async function refreshLogs() {
    try {
        const response = await fetch('/admin.php?ajax=logs', {
            method: 'GET',
            headers: {
                'Accept': 'application/json'
            },
            cache: 'no-store'
        });

        if (!response.ok) {
            return;
        }

        const payload = await response.json();
        renderLogRows(payload.logs || []);

        const updatedAtEl = document.getElementById('logs-updated-at');
        if (updatedAtEl && payload.updated_at) {
            updatedAtEl.textContent = payload.updated_at;
        }
    } catch (error) {
        // Silently ignore temporary network/UI errors during polling.
    }
}

setInterval(refreshLogs, 3000);
refreshLogs();
</script>
</body>
</html>
