CREATE TABLE IF NOT EXISTS devices_relay (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_name VARCHAR(50) NOT NULL,
    mqtt_topic VARCHAR(100) NOT NULL,
    status TINYINT(1) DEFAULT 0,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Masukkan data awal (Seed Data) sesuai diagram Anda
INSERT INTO devices_relay (device_name, mqtt_topic, status) VALUES 
('Relay Dev 1', '-/dev1/relay1', 0),
('Relay Dev 2', '-/dev1/relay2', 0),
('Relay Dev 3', '-/dev1/relay3', 0),
('Relay Dev 4', '-/dev1/relay4', 0);