#include <esp_now.h>
#include <WiFi.h>

// La estructura de datos debe ser exactamente igual en el Nodo y en el Gateway
typedef struct {
  char alerta[64];
} DatosAlerta;

DatosAlerta datosRecibidos;

// Función que se ejecuta automáticamente cuando llega un mensaje ESP-NOW
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  // Copiamos la información en nuestra estructura
  memcpy(&datosRecibidos, incomingData, sizeof(datosRecibidos));
  
  Serial.print("📥 MENSAJE RECIBIDO: ");
  Serial.println(datosRecibidos.alerta);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // ESP-NOW requiere que el WiFi esté en modo Estación (aunque no se conecte a un router)
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error inicializando ESP-NOW");
    return;
  }

  // Registrar la función que "escucha"
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("\n=========================================");
  Serial.println(" 📡 GATEWAY ESP-NOW LISTO Y ESCUCHANDO ");
  Serial.println("=========================================\n");
}

void loop() {
  // El Gateway no tiene que hacer nada en el loop, la función OnDataRecv trabaja en segundo plano
}
