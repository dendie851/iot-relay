const express = require('express');
const mysql = require('mysql2');
const mqtt = require('mqtt');

const app = express();
app.use(express.json());

// --- 1. KONFIGURASI DATABASE ---
const db = mysql.createPool({
    host: process.env.DB_HOST || 'mysql-db',
    user: process.env.DB_USER || 'root',
    password: process.env.DB_PASS || 'password_iot',
    database: process.env.DB_NAME || 'iot_db',
    waitForConnections: true,
    connectionLimit: 10
});

// --- 2. KONFIGURASI MQTT ---
const mqttClient = mqtt.connect('mqtt://mqtt-broker');

mqttClient.on('connect', () => {
    console.log('✅ Connected to MQTT Broker');
    // Subscribe ke semua topik relay sesuai pola diagram
    mqttClient.subscribe('-/+/relay+');
    console.log('📡 Subscribed to relay topics');
});

// --- LOGIC: UPDATE DB SAAT ADA PESAN MQTT (SYNC STATUS) ---
mqttClient.on('message', (topic, message) => {
    try {
        // Parsing pesan string ke JSON
        const data = JSON.parse(message.toString());

        // Pastikan field 'status' ada di dalam JSON
        if (data.status !== undefined) {
            const status = parseInt(data.status);
            console.log(`📥 MQTT Received [JSON]: [${topic}] -> status: ${status}`);

            const sql = "UPDATE devices_relay SET status = ? WHERE mqtt_topic = ?";
            db.query(sql, [status, topic], (err, result) => {
                if (err) console.error('❌ Database update error:', err);
            });
        }
    } catch (e) {
        // Jika pesan bukan JSON (misal angka 1 saja), script tidak akan crash
        console.warn(`⚠️ Pesan di [${topic}] bukan JSON valid: ${message.toString()}`);
    }
});

// --- 3. API ENDPOINTS ---

// A. API List Status Relay
app.get('/api/relays', (req, res) => {
    db.query("SELECT * FROM devices_relay", (err, results) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json(results);
    });
});

// B. API Control Relay (Publish JSON & Update DB)
app.post('/api/relay/control', (req, res) => {
    const { topic, status } = req.body;

    if (topic === undefined || status === undefined) {
        return res.status(400).json({ error: "Missing topic or status" });
    }

    // 1. BUAT PAYLOAD JSON
    const payload = JSON.stringify({
        status: parseInt(status),
        updated_at: new Date().toISOString()
    });

    // 2. Publish ke MQTT (Kirim JSON String)
    mqttClient.publish(topic, payload, { qos: 1, retain: true }, (err) => {
        if (err) return res.status(500).json({ error: "MQTT Publish Failed" });

        // 3. Update Database Lokal
        const sql = "UPDATE devices_relay SET status = ? WHERE mqtt_topic = ?";
        db.query(sql, [status, topic], (dbErr) => {
            if (dbErr) return res.status(500).json({ error: "DB Update Failed" });

            res.json({
                message: `Success: Data published as JSON`,
                topic: topic,
                payload: JSON.parse(payload) // Mengirim balik objek JSON ke response API
            });
        });
    });
});

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`🚀 Backend IoT API listening on port ${PORT}`);
});