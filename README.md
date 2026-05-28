# ESP32-S3 LVGL Template

Plantilla para el desarrollo de interfaces gráficas con LVGL en la placa **Waveshare ESP32-S3-Touch-LCD-1.47** ([ficha técnica](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.47)).

Diseñada para que el flujo de trabajo sea simple:

1. Diseña la UI en **SquareLine Studio** y exporta al directorio del componente.
2. Implementa la lógica de cada pantalla en `components/lvgl_ui/screens/`.
3. Compila y flashea.

`main.c` se limita a inicializar el hardware y arrancar la capa de UI — no necesita tocarse entre proyectos.

---

## Índice

1. [Hardware](#hardware)
2. [Versiones de componentes](#versiones-de-componentes)
3. [Entorno de desarrollo (Docker / Dev Container)](#entorno-de-desarrollo-docker--dev-container)
4. [Estructura del proyecto](#estructura-del-proyecto)
5. [Pines del BSP](#pines-del-bsp)
6. [Arquitectura de pantallas LVGL](#arquitectura-de-pantallas-lvgl)
7. [Cómo añadir una nueva pantalla](#cómo-añadir-una-nueva-pantalla)
8. [Cómo añadir un comando de actualización inter-tarea](#cómo-añadir-un-comando-de-actualización-inter-tarea)
9. [SquareLine Studio — Configuración y exportación](#squareline-studio--configuración-y-exportación)
10. [Compilar el proyecto](#compilar-el-proyecto)
11. [Paleta de comandos ESP-IDF en VS Code](#paleta-de-comandos-esp-idf-en-vs-code)
12. [Análisis del binario](#análisis-del-binario)
13. [Diagnóstico de crashes (backtrace y addr2line)](#diagnóstico-de-crashes-backtrace-y-addr2line)
14. [Flashear el firmware (fuera del contenedor)](#flashear-el-firmware-fuera-del-contenedor)
15. [Periféricos opcionales](#periféricos-opcionales)

---

## Hardware

| Campo | Valor |
|-------|-------|
| Placa | Waveshare ESP32-S3-Touch-LCD-1.47 |
| SoC | ESP32-S3 (dual-core Xtensa LX7, 240 MHz) |
| Pantalla | 1.47" JD9853, 172 × 320 px, RGB565 |
| Táctil | AXS5106 (I2C) |
| Flash | 2 MB |
| PSRAM | 8 MB (OPI) |
| SD Card | SDMMC (4 bits) |
| Batería | ADC (canal de monitorización de voltaje) |

---

## Versiones de componentes

| Componente | Versión | Notas |
|-----------|---------|-------|
| ESP-IDF | **5.2.0** | Imagen Docker `espressif/idf:v5.2` |
| LVGL (`lvgl/lvgl`) | **8.4.0** | Gestionado por IDF Component Manager |
| esp_lvgl_port (`espressif/esp_lvgl_port`) | **2.7.2** | Puerto LVGL para ESP-IDF |
| esp_lcd_touch (`espressif/esp_lcd_touch`) | **1.2.1** | Driver base de pantalla táctil |
| button (`espressif/button`) | **4.1.6** | Gestión de botones GPIO |
| Driver LCD | JD9853 | Componente local (`components/esp_lcd_jd9853/`) |
| Driver táctil | AXS5106 | Componente local (`components/esp_lcd_touch_axs5106/`) |

> **Nota:** Para añadir o actualizar dependencias, edita `main/idf_component.yml` y ejecuta `idf.py update-dependencies` dentro del contenedor.

---

## Entorno de desarrollo (Docker / Dev Container)

El proyecto incluye un **Dev Container** listo para usar con VS Code. Todo el toolchain de ESP-IDF está preinstalado en la imagen Docker.

### Requisitos previos

- [Docker Desktop](https://www.docker.com/products/docker-desktop/) (o Docker Engine en Linux)
- [Visual Studio Code](https://code.visualstudio.com/) con la extensión **Dev Containers** (`ms-vscode-remote.remote-containers`)

### Clonar el repositorio

```bash
git clone https://github.com/carlassaraf/esp32s3_lvgl_template.git
cd esp32s3_lvgl_template
```

### Instalación de Docker

#### Linux (Ubuntu / Debian)

```bash
# Eliminar versiones antiguas si las hubiera
sudo apt-get remove -y docker docker-engine docker.io containerd runc

# Instalar dependencias previas
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg

# Añadir la clave GPG oficial de Docker
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg \
  | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

# Añadir el repositorio oficial
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
  | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# Instalar Docker Engine
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io \
  docker-buildx-plugin docker-compose-plugin

# Añadir tu usuario al grupo docker (evita usar sudo en cada comando)
sudo usermod -aG docker $USER
newgrp docker

# Verificar que la instalación funciona
docker run hello-world
```

> **Otras distribuciones:** Consulta la [documentación oficial](https://docs.docker.com/engine/install/) para Fedora, RHEL, Arch y otras.

#### Windows

1. Descarga **Docker Desktop para Windows** desde [docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop/).
2. Ejecuta el instalador (`Docker Desktop Installer.exe`) y sigue el asistente.
3. Cuando se solicite, habilita la integración con **WSL 2** (recomendado) o con **Hyper-V**.
4. Reinicia el equipo si el instalador lo pide.
5. Abre Docker Desktop y espera a que el ícono de la barra de tareas indique que el motor está en ejecución.
6. Verifica la instalación desde PowerShell o CMD:

```powershell
docker run hello-world
```

> **WSL 2 (recomendado):** Docker Desktop con WSL 2 ofrece mejor rendimiento. Si aún no tienes WSL 2, ejecuta en PowerShell como administrador:
> ```powershell
> wsl --install
> ```
> Reinicia el equipo antes de instalar Docker Desktop.

### Arrancar el entorno

1. Abre la carpeta del proyecto en VS Code.
2. Cuando aparezca el aviso **"Reopen in Container"**, acéptalo. Si no aparece: `Ctrl+Shift+P` → *Dev Containers: Reopen in Container*.
3. VS Code construirá la imagen y abrirá el proyecto dentro del contenedor.
4. Abre un terminal integrado — el entorno de ESP-IDF ya está activado automáticamente.

### Detalles del contenedor

| Campo | Valor |
|-------|-------|
| Imagen base | `espressif/idf:v5.2` |
| Locale | `C.UTF-8` |
| Paquetes extra | `udev` (necesario para acceso a puertos serie en Linux host) |
| Privilegios | `--privileged` (necesario para acceso a USB/serie desde el contenedor) |
| IDF path | `/opt/esp/idf` |

> **Acceso al puerto serie desde el contenedor:** En Linux basta con `--privileged`. En macOS y Windows el puerto USB no pasa directamente al contenedor Docker; flashea el firmware **desde el sistema anfitrión** con `esptool` (ver sección [Flashear el firmware](#flashear-el-firmware-fuera-del-contenedor)).

---

## Estructura del proyecto

```
esp32s3_lvgl_template/
├── main/
│   ├── main.c                  ← Punto de entrada: init hardware + lvgl_ui_start()
│   ├── CMakeLists.txt
│   └── idf_component.yml       ← Dependencias gestionadas (LVGL, lvgl_port, etc.)
│
├── components/
│   ├── esp_bsp/                ← Board Support Package (display, táctil, I2C, WiFi…)
│   │   ├── bsp_display.h/c     ← LCD SPI + retroiluminación LEDC
│   │   ├── bsp_touch.h/c       ← Controlador táctil AXS5106
│   │   ├── bsp_lvgl.h/c        ← Inicialización del puerto LVGL
│   │   ├── bsp_i2c.h/c         ← Bus I2C maestro
│   │   ├── bsp_wifi.h/c        ← WiFi STA (opcional)
│   │   ├── bsp_sdcard.h/c      ← Tarjeta SD SDMMC (opcional)
│   │   └── bsp_battery.h/c     ← Monitorización de batería ADC (opcional)
│   │
│   ├── lvgl_ui/                ← Capa de UI (ver sección Arquitectura)
│   │   ├── CMakeLists.txt      ← NUNCA sobreescrito por SquareLine
│   │   ├── lvgl_ui.h/c         ← Punto de entrada: lvgl_ui_start()
│   │   ├── ui/                 ← ← EXPORT DE SQUARELINE (no tocar)
│   │   └── screens/            ← Lógica de pantallas (aquí trabajas)
│   │       ├── screens.h/c     ← Registro + motor de ciclo de vida
│   │       ├── screens_update.h/c  ← Cola FreeRTOS inter-tarea
│   │       ├── screens_timeout.h/c ← Timeout de inactividad / apagado
│   │       ├── scr_main.h/c    ← Controlador de pantalla principal (ejemplo)
│   │       └── scr_<nombre>.h/c ← Un fichero por pantalla adicional
│   │
│   ├── esp_lcd_jd9853/         ← Driver LCD JD9853
│   └── esp_lcd_touch_axs5106/  ← Driver táctil AXS5106
│
├── .devcontainer/              ← Configuración Dev Container
├── partitions.csv              ← Tabla de particiones flash
├── sdkconfig                   ← Configuración del proyecto ESP-IDF
└── dependencies.lock           ← Versiones resueltas de dependencias
```

---

## Pines del BSP

Todos los pines están definidos como constantes `BSP_PIN_*` en los headers del BSP. Para cambiar un pin, basta con editar el header correspondiente — el código no contiene números de GPIO sueltos.

### Pantalla LCD (SPI — JD9853)

| GPIO | Constante BSP | Función |
|------|--------------|---------|
| 39 | `BSP_PIN_LCD_MOSI` | SPI MOSI (datos hacia pantalla) |
| 38 | `BSP_PIN_LCD_SCLK` | SPI CLK |
| NC | `BSP_PIN_LCD_MISO` | SPI MISO (no conectado) |
| 21 | `BSP_PIN_LCD_CS` | SPI Chip Select |
| 45 | `BSP_PIN_LCD_DC` | Data / Command |
| 40 | `BSP_PIN_LCD_RST` | Reset del panel |
| 46 | `BSP_PIN_LCD_BL` | Retroiluminación (PWM LEDC) |

### Táctil (I2C — AXS5106)

| GPIO | Constante BSP | Función |
|------|--------------|---------|
| 41 | `BSP_PIN_I2C_SCL` | I2C Clock |
| 42 | `BSP_PIN_I2C_SDA` | I2C Data |
| 47 | `BSP_PIN_TP_INT` | Interrupción táctil |
| 48 | `BSP_PIN_TP_RST` | Reset táctil |

### Tarjeta SD (SDMMC 4-bit)

| GPIO | Constante BSP | Función |
|------|--------------|---------|
| 15 | `BSP_PIN_SD_CMD` | SD CMD |
| 16 | `BSP_PIN_SD_CLK` | SD CLK |
| 17 | `BSP_PIN_SD_D0` | SD Data 0 |
| 18 | `BSP_PIN_SD_D1` | SD Data 1 |
| 13 | `BSP_PIN_SD_D2` | SD Data 2 |
| 14 | `BSP_PIN_SD_D3` | SD Data 3 |

### Batería (ADC)

| Parámetro | Valor | Constante BSP |
|-----------|-------|--------------|
| Unidad ADC | ADC_UNIT_2 | `BSP_BATTERY_ADC_UNIT` |
| Canal | ADC_CHANNEL_1 | `BSP_BATTERY_ADC_CHANNEL` |
| Atenuación | 12 dB (~0–3.9 V) | `BSP_BATTERY_ADC_ATTEN` |

### Rotación de pantalla

La rotación se configura con una sola constante en `components/esp_bsp/bsp_display.h`:

```c
#define BSP_DISPLAY_ROTATION  (90)   /* 0 | 90 | 180 | 270 */
```

La resolución efectiva (`BSP_LCD_H_RES` / `BSP_LCD_V_RES`) y la configuración del driver táctil se ajustan automáticamente al cambiar este valor.

---

## Arquitectura de pantallas LVGL

```
┌─────────────────────────────────────────────────────────┐
│  main.c                                                 │
│  app_main() → bsp_*_init() → lvgl_ui_start()           │
└────────────────────────┬────────────────────────────────┘
                         │ llama una vez
┌────────────────────────▼────────────────────────────────┐
│  components/lvgl_ui/lvgl_ui.c                           │
│  lvgl_ui_start():                                       │
│    1. ui_init()          ← Squareline crea los objetos  │
│    2. ui_screens_prepare()  ← registra callbacks        │
│    3. lv_timer_create(20ms) ← arranca el motor de ticks │
└────────────────────────┬────────────────────────────────┘
                         │ cada 20 ms (timer LVGL)
┌────────────────────────▼────────────────────────────────┐
│  components/lvgl_ui/screens/                            │
│                                                         │
│  ui_screens_step()    → detecta cambio de pantalla      │
│                          llama uninit() / init()        │
│                          llama step() en la activa      │
│                                                         │
│  ui_update_screens()  → vacía la cola FreeRTOS          │
│                          ejecuta comandos inter-tarea    │
│                                                         │
│  disp_timeout_step()  → gestiona timeout inactividad    │
└─────────────────────────────────────────────────────────┘
```

### Regla de oro

> **El directorio `components/lvgl_ui/ui/` contiene el export de SquareLine Studio y NUNCA debe modificarse manualmente.**
>
> Toda interacción con LVGL debe realizarse desde los controladores de pantalla (`screens/scr_*.c`) o enviando comandos a la cola de actualización (`ui_update_cmd()`). Ninguna tarea de FreeRTOS debe llamar funciones de LVGL directamente.

### Motor de ciclo de vida (screens.c)

Cada pantalla tiene cuatro callbacks opcionales:

| Callback | Cuándo se llama |
|----------|----------------|
| `prepare()` | Una sola vez al arrancar — para registrar callbacks persistentes en widgets |
| `init()` | Cada vez que la pantalla se vuelve activa — para refrescar datos y arrancar animaciones |
| `uninit()` | Al navegar a otra pantalla — para detener timers locales |
| `step()` | Cada ~20 ms mientras la pantalla está activa — para lógica incremental |

### Actualizaciones inter-tarea (screens_update.c)

Las tareas FreeRTOS que necesitan actualizar la UI **no deben** tomar el mutex LVGL ni llamar funciones de LVGL directamente. En su lugar, encolan un comando mediante `ui_update_cmd()`. El timer LVGL de 20 ms vacía la cola de forma segura dentro de su propio contexto.

```c
// Desde cualquier tarea FreeRTOS (o ISR con FromISR):
#include "screens/screens_update.h"

ui_update_cmd(UI_UPDATE_TEMPERATURA, (void*)(uintptr_t)valor_entero);
```

Ver la sección [Cómo añadir un comando de actualización inter-tarea](#cómo-añadir-un-comando-de-actualización-inter-tarea) para el proceso completo.

### Timeout de pantalla (screens_timeout.c)

Los timeouts se configuran en `lvgl_ui.c` al arrancar:

```c
disp_timeout_init(
    30000,         /* ms de inactividad antes de volver a SCREEN_MAIN */
    TIMEOUT_NEVER  /* ms de inactividad antes de apagar la pantalla   */
);
```

Usa `TIMEOUT_NEVER` (0) para deshabilitar cualquiera de los dos. El valor del timeout de apagado puede modificarse en tiempo de ejecución con `disp_timeout_set_off_ms()` (p. ej., desde una pantalla de ajustes).

La detección de actividad se basa en `lv_disp_get_inactive_time()` de LVGL — no requiere un handler de táctil adicional.

---

## Cómo añadir una nueva pantalla

### Convención de nombres — SquareLine Studio ↔ código C

El macro `SCREEN_REGISTER` conecta los tres nombres que intervienen en el registro:

```c
#define SCREEN_REGISTER(name, scr, fn_name) \
    { name, &scr,                           \
      scr_##fn_name##_prepare,              \
      scr_##fn_name##_init,                 \
      scr_##fn_name##_uninit,               \
      scr_##fn_name##_step }
```

| Argumento del macro | Qué es | Ejemplo |
|---------------------|--------|---------|
| `name` | Cadena legible, sólo usada en logs | `"Settings"` |
| `scr` | Variable `lv_obj_t *` generada por SquareLine — **sin** `&` (el macro lo añade) | `ui_scrSettings` |
| `fn_name` | Prefijo de las 4 funciones del controlador | `settings` |

**Regla SquareLine → nombre de variable C:**
SquareLine antepone siempre `ui_` al nombre que le pongas a la pantalla en el editor. Ejemplos:

| Nombre en SquareLine | Variable generada |
|----------------------|-------------------|
| `scrMain` | `ui_scrMain` |
| `scrSettings` | `ui_scrSettings` |
| `Screen1` (nombre por defecto) | `ui_Screen1` |
| `scrError` | `ui_scrError` |

**Regla `fn_name` → nombres de función C:**
Con `fn_name = settings` el macro espera exactamente:

```c
void scr_settings_prepare(void);
void scr_settings_init(void);
void scr_settings_uninit(void);
void scr_settings_step(void);
```

---

### Paso 1 — Diseña en SquareLine Studio

Crea la nueva pantalla y dale el nombre `scrSettings`. SquareLine generará `lv_obj_t *ui_scrSettings` como variable global accesible vía `#include "ui.h"`.

### Paso 2 — Añade el valor al enum

En `components/lvgl_ui/screens/screens.h`, añade el nuevo valor **antes** de `SCREEN_MAX`:

```c
typedef enum {
    SCREEN_MAIN = 0,
    SCREEN_SETTINGS,   // ← nuevo
    SCREEN_MAX
} ui_screens_t;
```

### Paso 3 — Crea el controlador de pantalla

Crea `scr_settings.h` y `scr_settings.c` en `components/lvgl_ui/screens/`:

```c
/* scr_settings.h */
#ifndef __SCR_SETTINGS_H__
#define __SCR_SETTINGS_H__
void scr_settings_prepare(void);
void scr_settings_init(void);
void scr_settings_uninit(void);
void scr_settings_step(void);
#endif
```

```c
/* scr_settings.c */
#include "scr_settings.h"
#include "screens.h"
#include "ui.h"   /* acceso a ui_scrSettings, ui_lblTitulo, etc. */
#include "esp_log.h"

static const char *TAG = "scr_settings";

void scr_settings_prepare(void)
{
    /* Registra callbacks persistentes en widgets que sobreviven entre pantallas.
     * Ejemplo:
     *   lv_obj_add_event_cb(ui_btnVolver, on_volver_cb, LV_EVENT_CLICKED, NULL);
     */
}

void scr_settings_init(void)
{
    /* Refresca contenido dinámico al entrar en la pantalla.
     * Ejemplo:
     *   lv_slider_set_value(ui_sliderBrillo, brillo_actual, LV_ANIM_OFF);
     */
    ESP_LOGD(TAG, "init");
}

void scr_settings_uninit(void)
{
    /* Detén animaciones o timers locales al salir. */
    ESP_LOGD(TAG, "uninit");
}

void scr_settings_step(void)
{
    /* Lógica cada ~20 ms. Evita bloqueos. */
}
```

### Paso 4 — Regístrala con `SCREEN_REGISTER`

En `components/lvgl_ui/screens/screens.c`, añade el `#include` y la entrada en `screen_configs[]`:

```c
#include "scr_settings.h"

static const screen_config_t screen_configs[SCREEN_MAX] = {
    [SCREEN_MAIN]     = SCREEN_REGISTER("Main",     ui_scrMain,     main),
    [SCREEN_SETTINGS] = SCREEN_REGISTER("Settings", ui_scrSettings, settings),
};
```

Los tres argumentos de `SCREEN_REGISTER` deben cumplir la convención de nombres descrita arriba: el segundo es la variable global de SquareLine (sin `&`) y el tercero es el prefijo común de las cuatro funciones del controlador.

### Paso 5 — Navegar a la nueva pantalla

Desde cualquier controlador de pantalla (dentro del contexto del tick LVGL):

```c
ui_screens_navigate(SCREEN_SETTINGS);
```

El motor detectará el cambio, llamará a `scr_main_uninit()` y luego a `scr_settings_init()` de forma automática.

---

## Cómo añadir un comando de actualización inter-tarea

Los comandos de actualización son el único mecanismo para que tareas FreeRTOS externas modifiquen la UI de forma segura. Sigue estos pasos para añadir uno nuevo.

### Paso 1 — Declara el comando en el enum

En `components/lvgl_ui/screens/screens_update.h`, añade el nuevo valor **antes** de `UI_UPDATE_MAX`:

```c
typedef enum {
    UI_UPDATE_TEMPERATURA,   // ← nuevo: actualiza un label de temperatura
    UI_UPDATE_CONEXION,      // ← nuevo: actualiza icono de estado de red
    UI_UPDATE_MAX
} ui_update_cmd_t;
```

### Paso 2 — Implementa el handler en screens_update.c

En `components/lvgl_ui/screens/screens_update.c`, implementa una función estática para cada comando. Este handler se ejecuta **dentro del contexto del timer LVGL**, por lo que es seguro llamar a funciones de LVGL directamente:

```c
#include "ui.h"   /* acceso a los objetos de SquareLine */

static void handle_temperatura(void *data)
{
    /* data se pasa como (void*)(uintptr_t) desde el emisor.
     * Castea de vuelta al tipo original. */
    int32_t temp_centesimas = (int32_t)(uintptr_t)data;
    float temp = temp_centesimas / 100.0f;

    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f °C", temp);
    lv_label_set_text(ui_lblTemperatura, buf);
}

static void handle_conexion(void *data)
{
    bool conectado = (bool)(uintptr_t)data;
    lv_obj_t *icono = conectado ? ui_imgWifiOn : ui_imgWifiOff;
    /* mostrar/ocultar según estado */
    lv_obj_clear_flag(icono, LV_OBJ_FLAG_HIDDEN);
}
```

> **Tipos de datos en `data`:** Para valores escalares (enteros, booleanos, enumerados) usa el patrón `(void*)(uintptr_t)valor` — cabe en un puntero sin malloc. Para structs más grandes, aloja la memoria con `pvPortMalloc` antes de encolar y libérala al final del handler con `vPortFree(data)`.

### Paso 3 — Registra el handler en la tabla

Añade la función al array `s_handlers[]` en el mismo orden que el enum:

```c
static const handler_fn_t s_handlers[UI_UPDATE_MAX] = {
    [UI_UPDATE_TEMPERATURA] = handle_temperatura,
    [UI_UPDATE_CONEXION]    = handle_conexion,
};
```

### Paso 4 — Envía el comando desde cualquier tarea

```c
#include "screens/screens_update.h"

// Tarea de sensores — notifica nueva temperatura
void tarea_sensores(void *arg)
{
    for (;;) {
        float temp = leer_sensor();
        int32_t temp_centesimas = (int32_t)(temp * 100);

        // Seguro desde cualquier tarea FreeRTOS:
        ui_update_cmd(UI_UPDATE_TEMPERATURA, (void*)(uintptr_t)temp_centesimas);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

> **Profundidad de la cola:** La cola tiene capacidad para `UPDATE_QUEUE_DEPTH` (16) mensajes. Si se llena, `ui_update_cmd()` descarta el mensaje y emite un warning por log. Aumenta la constante en `screens_update.c` si necesitas más capacidad.

---

## SquareLine Studio — Configuración y exportación

### Configuración del proyecto

Crea o abre el proyecto de SquareLine y ajusta la configuración en **Project Settings**:

| Campo | Valor |
|-------|-------|
| Board group | ESP32 |
| Board | ESP32-S3 |
| LVGL version | **8.x** (usar la más cercana a 8.4) |
| Color depth | **16 bit** |
| Color swap | **Disabled** (`LV_COLOR_16_SWAP = 0`) |
| Screen width | **320** px (landscape) / **172** px (portrait) |
| Screen height | **172** px (landscape) / **320** px (portrait) |
| LVGL include | `lvgl.h` |

> **Orientación:** La pantalla nativa es 172 × 320 (portrait). Si usas `BSP_DISPLAY_ROTATION = 90` (landscape), diseña en SquareLine con 320 × 172.

> **Color swap:** El byte swap RGB565 lo gestiona el hardware (DMA). SquareLine debe generarse **sin** swap (`LV_COLOR_16_SWAP = 0`) para que coincida con la configuración del driver.

### Ruta de exportación

En **Export → Export UI Files**, configura la ruta de exportación como:

```
<ruta_al_proyecto>/components/lvgl_ui/ui/
```

Por ejemplo, si el proyecto de SquareLine está en una carpeta `squareline/` dentro del repo:

```
../components/lvgl_ui/ui/
```

SquareLine sobreescribirá todos los archivos dentro de `ui/` en cada exportación. El fichero `components/lvgl_ui/CMakeLists.txt` **no está dentro de esa carpeta** y nunca se verá afectado.

### Qué genera SquareLine

| Archivo / carpeta | Descripción |
|-------------------|-------------|
| `ui.c` / `ui.h` | `ui_init()` y declaraciones globales de objetos |
| `ui_helpers.c/h` | Helpers de animación y cambio de pantalla |
| `ui_events.h` | Declaraciones de callbacks de eventos |
| `screens/ui_scr*.c/h` | Un fichero por pantalla — función `ui_scr*_screen_init()` |
| `components/` | Widgets reutilizables compuestos |
| `fonts/` | Fuentes personalizadas como arrays C |
| `images/` | Assets de imagen como arrays C |

### Acceder a objetos de SquareLine desde los controladores

Los objetos LVGL generados se declaran como `extern lv_obj_t *` en los headers de pantalla. Incluyendo `ui.h` desde el controlador tienes acceso directo:

```c
#include "ui.h"

void scr_main_init(void) {
    lv_label_set_text(ui_lblEstado, "Conectado");
    lv_arc_set_value(ui_arcProgreso, 75);
}
```

---

## Compilar el proyecto

### Desde el terminal (dentro del contenedor)

```bash
# Configura el target — sólo la primera vez o al cambiar de chip
idf.py set-target esp32s3

# Compila
idf.py build
```

Los binarios quedan en `build/`:
- `build/bootloader/bootloader.bin`
- `build/esp32s3_lvgl_template.bin`
- `build/partition_table/partition-table.bin`

### Desde la paleta de comandos de VS Code

Abre la paleta con `Ctrl+Shift+P` (Windows / Linux) o `Cmd+Shift+P` (macOS) y escribe el nombre del comando:

| Comando | Equivalente terminal |
|---------|---------------------|
| `ESP-IDF: Set Espressif device target` | `idf.py set-target esp32s3` |
| `ESP-IDF: Build your project` | `idf.py build` |
| `ESP-IDF: SDK Configuration editor (Menuconfig)` | `idf.py menuconfig` |
| `ESP-IDF: Flash your project` | `idf.py flash` |
| `ESP-IDF: Monitor your project` | `idf.py monitor` |
| `ESP-IDF: Build, Flash and start a monitor on your project` | `idf.py build flash monitor` |
| `ESP-IDF: Open ESP-IDF Terminal` | Abre terminal con entorno IDF activado |
| `ESP-IDF: Size analysis of the binaries` | `idf.py size` (interfaz gráfica) |
| `ESP-IDF: Add vscode configuration folder` | Genera `.vscode/` con IntelliSense |
| `ESP-IDF: Doctor command` | Diagnóstico del entorno IDF |
| `ESP-IDF: Erase flash memory from device` | `idf.py erase-flash` |
| `ESP-IDF: Show ESP-IDF Examples` | Explorador de ejemplos del SDK |

> **Tip:** La acción **Build, Flash and start a monitor** compila, flashea y abre el monitor serie en un solo paso — es la más usada durante el desarrollo cuando el dispositivo está conectado directamente al equipo o al contenedor (Linux).

---

## Paleta de comandos ESP-IDF en VS Code

Además de los comandos de compilación, la extensión ESP-IDF ofrece herramientas de diagnóstico y análisis directamente desde VS Code:

### Comandos de uso frecuente

| Comando | Para qué sirve |
|---------|----------------|
| `ESP-IDF: Size analysis of the binaries` | Abre un panel visual con el desglose de tamaño por sección, componente y fichero |
| `ESP-IDF: Launch IDF Monitor for CoreDump Mode` | Monitor serie que captura y decodifica coredumps automáticamente |
| `ESP-IDF: GDB Stub: Receive stack dump` | Conecta GDB al stub de excepción del chip |
| `ESP-IDF: Partition Table Editor UI` | Editor visual de `partitions.csv` |
| `ESP-IDF: NVS Partition Table Editor` | Crea/edita particiones NVS en CSV |
| `ESP-IDF: Run idf.py reconfigure` | Regenera los ficheros CMake sin recompilar todo |
| `ESP-IDF: Update ESP-IDF Tools` | Actualiza el toolchain a la última versión |

---

## Análisis del binario

### Desde el terminal

```bash
# Resumen de memoria: flash total, IRAM, DRAM usadas
idf.py size

# Desglose por componente ESP-IDF
idf.py size-components

# Desglose por fichero objeto (.o)
idf.py size-files
```

Ejemplo de salida de `idf.py size`:

```
Total sizes:
 DRAM .data size:    8652 bytes
 DRAM .bss  size:   28904 bytes
Used static DRAM:   37556 bytes ( 145132 available, 20.5% used)
Used static IRAM:   84572 bytes (  46500 available, 64.5% used)
      Flash code:  802345 bytes
    Flash rodata:  224132 bytes
Total image size: ~1098269 bytes (1.05 MB)
```

### Desde VS Code

Ejecuta `ESP-IDF: Size analysis of the binaries` en la paleta de comandos. Se abre un panel interactivo con barras de progreso por sección de memoria y la posibilidad de navegar hasta el componente o fichero que más ocupa.

---

## Diagnóstico de crashes (backtrace y addr2line)

Cuando el firmware genera un panic (acceso a memoria inválido, stack overflow, watchdog, etc.), la consola serie muestra un backtrace similar a:

```
Guru Meditation Error: Core  0 panic'ed (LoadProhibited)
...
Backtrace: 0x42012abc:0x3fcab120 0x42008def:0x3fcab140 0x42001234:0x3fcab160
```

### Opción A — Monitor automático (recomendado)

`idf.py monitor` decodifica el backtrace automáticamente usando el fichero ELF. Cada dirección se traduce a función + fichero + número de línea en tiempo real:

```bash
idf.py monitor
```

O desde la paleta: `ESP-IDF: Monitor your project`.

### Opción B — addr2line manual

Si tienes el backtrace en texto y el ELF en `build/`, usa la herramienta `addr2line` del toolchain de Xtensa:

```bash
xtensa-esp32s3-elf-addr2line -pfiaC \
  -e build/esp32s3_lvgl_template.elf \
  0x42012abc 0x42008def 0x42001234
```

Flags útiles:

| Flag | Efecto |
|------|--------|
| `-p` | Salida legible (pretty-print) |
| `-f` | Muestra el nombre de la función |
| `-i` | Expande funciones inline |
| `-a` | Muestra la dirección antes del resultado |
| `-C` | Demangle nombres C++ |
| `-e <elf>` | Fichero ELF a usar |

Ejemplo de salida:

```
0x42012abc: scr_main_step at /workspace/components/lvgl_ui/screens/scr_main.c:42
0x42008def: ui_screens_step at /workspace/components/lvgl_ui/screens/screens.c:81
0x42001234: ui_step_timer_cb at /workspace/components/lvgl_ui/lvgl_ui.c:12
```

> **Dónde está `xtensa-esp32s3-elf-addr2line`:** Dentro del contenedor está disponible en el PATH tras activar el entorno IDF. También está en `~/.espressif/tools/xtensa-esp-elf/*/bin/` si instalaste las herramientas IDF localmente.

### Opción C — GDB interactivo

Para sesiones de depuración más avanzadas con JTAG o el GDB stub de OpenOCD:

```bash
idf.py gdb
```

O desde la paleta: `ESP-IDF: GDB Stub: Receive stack dump`.

---

## Flashear el firmware (fuera del contenedor)

Dado que el puerto USB no es accesible desde dentro del contenedor Docker en macOS y Windows, el proceso recomendado es compilar dentro del contenedor y flashear desde el sistema anfitrión.

### 1. Instalar esptool en un entorno virtual

#### Linux / macOS

```bash
# Crear entorno virtual
python3 -m venv .venv

# Activar
source .venv/bin/activate

# Instalar esptool
pip install esptool

# Verificar
esptool.py version
```

#### Windows (PowerShell)

```powershell
# Crear entorno virtual
python -m venv .venv

# Activar
.venv\Scripts\Activate.ps1

# Si PowerShell bloquea la ejecución de scripts:
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned

# Instalar esptool
pip install esptool

# Verificar
esptool.py version
```

#### Windows (CMD)

```cmd
python -m venv .venv
.venv\Scripts\activate.bat
pip install esptool
esptool.py version
```

> **Tip:** Para no tener que activar el entorno cada vez, puedes añadir `.venv/Scripts` (Windows) o `.venv/bin` (Linux/macOS) al PATH.

### 2. Identificar el puerto serie

| SO | Cómo identificarlo |
|----|-------------------|
| Linux | `ls /dev/ttyUSB*` o `ls /dev/ttyACM*` |
| macOS | `ls /dev/cu.usbmodem*` o `ls /dev/cu.usbserial*` |
| Windows | Administrador de dispositivos → Puertos (COM y LPT) |

### 3. Flashear

Desde la **raíz del proyecto** en el sistema anfitrión, con el entorno virtual activado:

```bash
esptool --chip esp32s3 -b 460800 --before default-reset --after hard-reset \
  write-flash --flash-mode dio --flash-size 2MB --flash-freq 80m \
  0x0     build/bootloader/bootloader.bin \
  0x10000 build/esp32s3_lvgl_template.bin \
  0x8000  build/partition_table/partition-table.bin
```

En Windows sustituye la barra invertida `\` por el carácter de continuación de CMD (`` ` `` en PowerShell o `^` en CMD), o escribe todo en una sola línea.

Si el puerto no se detecta automáticamente, añade `-p <puerto>`:

```bash
# Linux
esptool -p /dev/ttyUSB0 --chip esp32s3 ...

# macOS
esptool -p /dev/cu.usbmodem1401 --chip esp32s3 ...

# Windows
esptool -p COM3 --chip esp32s3 ...
```

> **Modo descarga:** Si el chip no entra en modo descarga automáticamente, mantén pulsado **BOOT** mientras pulsas y sueltas **RESET**, luego suelta **BOOT** antes de ejecutar el comando.

---

## Periféricos opcionales

Los siguientes periféricos están disponibles en el BSP pero **desactivados por defecto** en `main.c`. Descomenta las llamadas según necesites:

### WiFi

```c
#include "bsp_wifi.h"

// En app_main(), antes de lvgl_ui_start():
bsp_wifi_init("NOMBRE_RED", "CONTRASEÑA");
```

### Tarjeta SD

```c
#include "bsp_sdcard.h"

// En app_main():
bsp_sdcard_init();   // monta en /sdcard

// Opcional: registrar el driver FATFS en LVGL (para cargar imágenes desde SD)
// Llamar DENTRO del bloque lvgl_port_lock(), después de lvgl_ui_start():
bsp_lv_fs_fatfs_init();
```

### Monitorización de batería

```c
#include "bsp_battery.h"

// En app_main():
bsp_battery_init();

// En cualquier momento posterior:
float voltaje;
uint16_t raw;
bsp_battery_get_voltage(&voltaje, &raw);
```

---

## Licencia

Este proyecto es una plantilla de uso libre. Adapta, modifica y distribuye libremente.
