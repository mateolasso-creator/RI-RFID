-- Crear base de datos si no existe
CREATE DATABASE IF NOT EXISTS rfid_access;

USE rfid_access;

-- Tabla de usuarios (badges)
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Tabla de logs de acceso
CREATE TABLE IF NOT EXISTS logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) NOT NULL,
    access_status ENUM('GRANTED', 'DENIED') NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insertar algunos datos de ejemplo
INSERT INTO users (uid, name) VALUES ('123456789', 'Usuario Ejemplo') ON DUPLICATE KEY UPDATE name=name;