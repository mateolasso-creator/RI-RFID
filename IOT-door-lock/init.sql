-- 1. Preparación de la Base de Datos
CREATE DATABASE IF NOT EXISTS rfid_access;
USE rfid_access;

-- 2. Limpieza (Importante para evitar errores de tablas viejas)
SET FOREIGN_KEY_CHECKS = 0;
DROP TABLE IF EXISTS logs;
DROP TABLE IF EXISTS users;
SET FOREIGN_KEY_CHECKS = 1;

-- 3. Tabla de Usuarios
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    status ENUM('ACTIVE', 'INACTIVE', 'BLOCKED') DEFAULT 'ACTIVE',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_uid (uid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 4. Tabla de Logs (Sin llaves foráneas restrictivas)
CREATE TABLE logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) NOT NULL,
    access_status ENUM('GRANTED', 'DENIED', 'ERROR') NOT NULL,
    ip_address VARCHAR(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_uid (uid),
    INDEX idx_timestamp (timestamp)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 5. Datos Iniciales de Prueba
INSERT INTO users (uid, name, status) VALUES
('1117E7A9', 'Grupo3', 'ACTIVE'),
('E5F6A7B8', 'María López', 'ACTIVE'),
('C9D0E1F2', 'Carlos Rodríguez', 'INACTIVE');

-- Mensaje de confirmación en logs de Docker
SELECT '✅ Base de datos rfid_access inicializada con éxito' AS Info;