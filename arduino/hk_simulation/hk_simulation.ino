#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <string.h>


// ─────────────────────────────────────────────────────────
// TYPES — mirrors your WOD_Telemetry_t from TelemetryCollector.h
// ─────────────────────────────────────────────────────────
typedef unsigned long time_unix;

typedef struct {
    int      vbat;             // battery voltage [mV]
    int      volt_5V;          // 5V bus voltage [mV]
    int      volt_3V3;         // 3.3V bus voltage [mV]
    int      charging_power;   // charging power [mW]
    int      consumed_power;   // consumed power [mW]
    int      current_3V3;      // 3.3V bus current [mA]
    int      current_5V;       // 5V bus current [mA]
    time_unix sat_time;        // satellite unix time [sec]
    unsigned int free_memory;  // free memory [bytes]
    unsigned int corrupt_bytes;// corrupted bytes [bytes]
    unsigned short number_of_resets; // reset counter
} WOD_Telemetry_t;

// ─────────────────────────────────────────────────────────
// GLOBALS — mirrors tlm_save_periods / tlm_last_save_time
// ─────────────────────────────────────────────────────────
#define NUM_SUBSYSTEMS 5

typedef enum {
    eps_tlm,
    trxvu_tlm,
    ant_tlm,
    solar_panel_tlm,
    wod_tlm
} subsystem_tlm;

// Save period per subsystem (in seconds) — loaded from FRAM in real code
time_unix tlm_save_periods[NUM_SUBSYSTEMS]    = {4, 6, 8, 10, 5};
time_unix tlm_last_save_time[NUM_SUBSYSTEMS]  = {0, 0, 0, 0,  0};

// ─────────────────────────────────────────────────────────
// STUB UTILITIES
// ─────────────────────────────────────────────────────────

// Replaces Time_getUnixEpoch() — returns seconds since boot
time_unix getUnixTime() {
    return (time_unix)(millis() / 1000UL);
}

// Replaces CheckExecutionTime()
int CheckExecutionTime(time_unix last, time_unix period) {
    return (getUnixTime() - last) >= period;
}

// Replaces c_fileWrite() — prints to Serial instead of writing to file
void c_fileWrite(const char *filename, void *data) {
    Serial.print("  [FILE WRITE] -> ");
    Serial.println(filename);
}

// ─────────────────────────────────────────────────────────
// STUB SUBSYSTEM SAVE FUNCTIONS
// All hardware calls replaced with random values
// ─────────────────────────────────────────────────────────

void TelemetrySaveEPS() {
    Serial.println("[EPS] Saving EPS telemetry...");
    // Stub: random raw/eng values for MB and CDB boards
    int raw_mb  = random(2900, 3100);
    int eng_mb  = random(3100, 3400);
    int raw_cdb = random(2800, 3000);
    int eng_cdb = random(3400, 3700);
    Serial.print("  raw_mb=");  Serial.print(raw_mb);
    Serial.print("  eng_mb=");  Serial.print(eng_mb);
    Serial.print("  raw_cdb="); Serial.print(raw_cdb);
    Serial.print("  eng_cdb="); Serial.println(eng_cdb);
    c_fileWrite("EPS_RAW_MB_TLM",  NULL);
    c_fileWrite("EPS_ENG_MB_TLM",  NULL);
    c_fileWrite("EPS_RAW_CDB_TLM", NULL);
    c_fileWrite("EPS_ENG_CDB_TLM", NULL);
}

void TelemetrySaveTRXVU() {
    Serial.println("[TRXVU] Saving TRXVU telemetry...");
    // Stub: random TX/RX signal values
    int tx_power = random(-30, 0);    // dBm
    int rx_rssi  = random(-100, -50); // dBm
    Serial.print("  TX_power="); Serial.print(tx_power);
    Serial.print(" dBm  RX_RSSI="); Serial.print(rx_rssi);
    Serial.println(" dBm");
    c_fileWrite("TX_TLM",   NULL);
    c_fileWrite("TX_REVC",  NULL);
    c_fileWrite("RX_TLM",   NULL);
    c_fileWrite("RX_REVC",  NULL);
}

void TelemetrySaveANT() {
    Serial.println("[ANT] Saving Antenna telemetry...");
    // Stub: antenna deployment status (0=not deployed, 1=deployed)
    int sideA = random(0, 2);
    int sideB = random(0, 2);
    Serial.print("  SideA="); Serial.print(sideA);
    Serial.print("  SideB="); Serial.println(sideB);
    c_fileWrite("ANTENNA_TLM", NULL);
}

void TelemetrySaveSolarPanels() {
    Serial.println("[SOLAR] Saving Solar Panel telemetry...");
    // Stub: 9 solar panels with random temperatures
    Serial.print("  Temps [C]: ");
    for (int i = 0; i < 9; i++) {
        int temp = random(-20, 80); // realistic space range
        Serial.print(temp);
        if (i < 8) Serial.print(", ");
    }
    Serial.println();
    c_fileWrite("SOLAR_PANELS_TLM", NULL);
}

// ─────────────────────────────────────────────────────────
// GetCurrentWODTelemetry — mirrors your real function
// All hardware calls replaced with random values
// ─────────────────────────────────────────────────────────
void GetCurrentWODTelemetry(WOD_Telemetry_t *wod) {
    if (wod == NULL) return;
    memset(wod, 0, sizeof(*wod));

    // Stub: replaces f_getfreespace()
    wod->free_memory   = random(100000, 512000);
    wod->corrupt_bytes = random(0, 10);

    // Stub: replaces Time_getUnixEpoch()
    wod->sat_time = getUnixTime();

    // Stub: replaces IsisEPS_getEngHKDataCDB() and IsisEPS_getRAEngHKDataMB()
    wod->vbat           = random(3600, 4200);  // mV
    wod->volt_3V3       = random(3250, 3350);  // mV
    wod->volt_5V        = random(4900, 5100);  // mV
    wod->current_3V3    = random(50,   200);   // mA
    wod->current_5V     = random(50,   300);   // mA
    wod->charging_power = random(200,  800);   // mW
    wod->consumed_power = random(100,  500);   // mW

    // Stub: replaces FRAM_read() for reset counter
    wod->number_of_resets = 3; // fixed fake value
}

void TelemetrySaveWOD() {
    WOD_Telemetry_t wod = {0};
    GetCurrentWODTelemetry(&wod);
    Serial.println("[WOD] Saving WOD telemetry...");
    Serial.print("  vbat=");           Serial.print(wod.vbat);           Serial.println(" mV");
    Serial.print("  volt_3V3=");       Serial.print(wod.volt_3V3);       Serial.println(" mV");
    Serial.print("  volt_5V=");        Serial.print(wod.volt_5V);        Serial.println(" mV");
    Serial.print("  current_3V3=");    Serial.print(wod.current_3V3);    Serial.println(" mA");
    Serial.print("  current_5V=");     Serial.print(wod.current_5V);     Serial.println(" mA");
    Serial.print("  charging_power="); Serial.print(wod.charging_power); Serial.println(" mW");
    Serial.print("  consumed_power="); Serial.print(wod.consumed_power); Serial.println(" mW");
    Serial.print("  free_memory=");    Serial.print(wod.free_memory);    Serial.println(" bytes");
    Serial.print("  resets=");         Serial.println(wod.number_of_resets);
    c_fileWrite("WOD_TLM", &wod);
}

// ─────────────────────────────────────────────────────────
// TelemetryCollectorLogic — exact mirror of your doc 5
// ─────────────────────────────────────────────────────────
void TelemetryCollectorLogic() {
    if (CheckExecutionTime(tlm_last_save_time[eps_tlm], tlm_save_periods[eps_tlm])) {
        TelemetrySaveEPS();
        tlm_last_save_time[eps_tlm] = getUnixTime();
    }
    if (CheckExecutionTime(tlm_last_save_time[trxvu_tlm], tlm_save_periods[trxvu_tlm])) {
        TelemetrySaveTRXVU();
        tlm_last_save_time[trxvu_tlm] = getUnixTime();
    }
    if (CheckExecutionTime(tlm_last_save_time[ant_tlm], tlm_save_periods[ant_tlm])) {
        TelemetrySaveANT();
        tlm_last_save_time[ant_tlm] = getUnixTime();
    }
    if (CheckExecutionTime(tlm_last_save_time[solar_panel_tlm], tlm_save_periods[solar_panel_tlm])) {
        TelemetrySaveSolarPanels();
        tlm_last_save_time[solar_panel_tlm] = getUnixTime();
    }
    if (CheckExecutionTime(tlm_last_save_time[wod_tlm], tlm_save_periods[wod_tlm])) {
        TelemetrySaveWOD();
        tlm_last_save_time[wod_tlm] = getUnixTime();
    }
}

// ─────────────────────────────────────────────────────────
// FREERTOS QUEUE
// ─────────────────────────────────────────────────────────
QueueHandle_t xHKQueue;

// ─────────────────────────────────────────────────────────
// OBC TASK — generates random subsystem values (simulates hardware)
//            and sends WOD struct to HK via queue
// ─────────────────────────────────────────────────────────
void OBC_Task(void *pvParameters) {
    WOD_Telemetry_t wod;
    for (;;) {
        Serial.println("\n========== OBC Task: generating telemetry ==========");

        // Simulate subsystem data — replaces real I2C hardware reads
        wod.vbat             = random(3600, 4200);   // mV
        wod.volt_3V3         = random(3250, 3350);   // mV
        wod.volt_5V          = random(4900, 5100);   // mV
        wod.current_3V3      = random(50,   200);    // mA
        wod.current_5V       = random(50,   300);    // mA
        wod.charging_power   = random(200,  800);    // mW
        wod.consumed_power   = random(100,  500);    // mW
        wod.free_memory      = random(100000, 512000); // bytes
        wod.corrupt_bytes    = random(0, 10);
        wod.number_of_resets = 3;                    // fixed (from FRAM stub)
        wod.sat_time         = (time_unix)(millis() / 1000UL);

        Serial.println("  [OBC] Subsystem values generated.");

        // Send WOD struct to HK task via queue
        xQueueSend(xHKQueue, &wod, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(3000)); // generate every 3 seconds
    }
}

// ─────────────────────────────────────────────────────────
// HK TASK — receives WOD from queue, runs collector logic,
//           and saves/displays telemetry
// ─────────────────────────────────────────────────────────
void HK_Task(void *pvParameters) {
    WOD_Telemetry_t received;
    for (;;) {
        if (xQueueReceive(xHKQueue, &received, portMAX_DELAY)) {

            Serial.println("---------- HK Task: collecting & saving ----------");

            // Run timing-based collector logic (mirrors TelemetryCollectorLogic)
            TelemetryCollectorLogic();

            // Display received WOD telemetry (replaces c_fileWrite)
            Serial.println("  [HK] WOD Telemetry saved:");
            Serial.print("  Vbat:           "); Serial.print(received.vbat);            Serial.println(" mV");
            Serial.print("  Volt_3V3:       "); Serial.print(received.volt_3V3);        Serial.println(" mV");
            Serial.print("  Volt_5V:        "); Serial.print(received.volt_5V);         Serial.println(" mV");
            Serial.print("  Current_3V3:    "); Serial.print(received.current_3V3);     Serial.println(" mA");
            Serial.print("  Current_5V:     "); Serial.print(received.current_5V);      Serial.println(" mA");
            Serial.print("  Charging Power: "); Serial.print(received.charging_power);  Serial.println(" mW");
            Serial.print("  Consumed Power: "); Serial.print(received.consumed_power);  Serial.println(" mW");
            Serial.print("  Free Memory:    "); Serial.print(received.free_memory);     Serial.println(" bytes");
            Serial.print("  Corrupt Bytes:  "); Serial.print(received.corrupt_bytes);   Serial.println(" bytes");
            Serial.print("  Resets:         "); Serial.println(received.number_of_resets);
            Serial.print("  Sat Time:       "); Serial.print(received.sat_time);        Serial.println(" sec");

            // Structured line for Python CSV logger
            Serial.print("WOD,");
            Serial.print(received.sat_time);         Serial.print(",");
            Serial.print(received.vbat);             Serial.print(",");
            Serial.print(received.volt_3V3);         Serial.print(",");
            Serial.print(received.volt_5V);          Serial.print(",");
            Serial.print(received.current_3V3);      Serial.print(",");
            Serial.print(received.current_5V);       Serial.print(",");
            Serial.print(received.charging_power);   Serial.print(",");
            Serial.print(received.consumed_power);   Serial.print(",");
            Serial.print(received.free_memory);      Serial.print(",");
            Serial.println(received.number_of_resets);

            Serial.println("--------------------------------------------------");
        }
    }
}

// ─────────────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0)); // seed random from floating pin

    xHKQueue = xQueueCreate(5, sizeof(WOD_Telemetry_t));

    xTaskCreate(OBC_Task, "OBC_Task", 512, NULL, 2, NULL); // higher priority — generates data
    xTaskCreate(HK_Task,  "HK_Task",  512, NULL, 1, NULL); // lower priority  — collects & saves

    vTaskStartScheduler();
}

void loop() {
    // Empty — FreeRTOS scheduler takes over
}
