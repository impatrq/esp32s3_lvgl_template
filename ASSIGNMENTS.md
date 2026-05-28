# Asignaciones LVGL — ESP32-S3

Guía de trabajo para las cinco asignaciones progresivas de desarrollo de UI con LVGL sobre la placa **Waveshare ESP32-S3-Touch-LCD-1.47**.

Cada asignación introduce conceptos nuevos de LVGL, usa periféricos reales del hardware y construye hacia una aplicación completa de monitorización de sensores en tiempo real.

---

## Índice

1. [Flujo de trabajo del repositorio](#1-flujo-de-trabajo-del-repositorio)
2. [Convención de ramas](#2-convención-de-ramas)
3. [Commits convencionales](#3-commits-convencionales)
4. [Cómo arrancar una asignación](#4-cómo-arrancar-una-asignación)
5. [Asignación 1 — Splash interactivo + widgets con datos reales](#asignación-1--splash-interactivo--widgets-con-datos-reales-del-hardware)
6. [Asignación 2 — Navegación por gestos, botón físico y brillo adaptativo](#asignación-2--navegación-por-gestos-botón-físico-y-brillo-adaptativo)
7. [Asignación 3 — Sensor en tiempo real, gráfico con scroll y alertas inline](#asignación-3--sensor-en-tiempo-real-gráfico-con-scroll-y-alertas-inline)
8. [Asignación 4 — WiFi, listas dinámicas y pantalla de conexión animada](#asignación-4--wifi-listas-dinámicas-y-pantalla-de-conexión-animada)
9. [Asignación 5 — Dashboard de producción](#asignación-5--dashboard-de-producción-sd-nvs-tema-personalizado-y-ux-completa)

---

## 1. Flujo de trabajo del repositorio

### Regla principal: `main` siempre limpio

La rama `main` contiene únicamente el template base. **Nunca se hace commit directamente en `main`.**

Cada asignación vive en su propia rama y permanece ahí — no se fusiona de vuelta a `main`. El historial de `main` refleja solo la evolución del template, no el trabajo de las asignaciones.

```
main ────────────────────────────────────────────────── (template limpio)
  │
  ├── assignment/01-splash-events ─────────────────────
  │
  ├── assignment/02-gestures-button ───────────────────
  │
  ├── assignment/03-freertos-charts ───────────────────
  │
  ├── assignment/04-wifi-lists ────────────────────────
  │
  └── assignment/05-dashboard ───────────────────────── (app completa)
```

Cada asignación parte del template limpio de `main`. Los conceptos son progresivos pero el código no se hereda — en cada asignación se reimplementa lo necesario, lo que refuerza la retención.

---

## 2. Convención de ramas

Todas las ramas de asignación siguen el patrón:

```
assignment/<número>-<descripción-corta>
```

| Rama | Asignación |
|------|-----------|
| `assignment/01-splash-events` | Asignación 1 |
| `assignment/02-gestures-button` | Asignación 2 |
| `assignment/03-freertos-charts` | Asignación 3 |
| `assignment/04-wifi-lists` | Asignación 4 |
| `assignment/05-dashboard` | Asignación 5 |

**Reglas:**
- Siempre en minúsculas
- Palabras separadas por guión `-`
- Sin tildes ni caracteres especiales en el nombre de la rama

---

## 3. Commits convencionales

Este repositorio sigue la especificación [Conventional Commits](https://www.conventionalcommits.org/).

### Formato

```
<tipo>(<ámbito>): <descripción corta en imperativo>

[cuerpo opcional — explica el POR QUÉ, no el QUÉ]
```

### Tipos

| Tipo | Cuándo usarlo |
|------|--------------|
| `feat` | Nueva funcionalidad: pantalla nueva, widget nuevo, periférico integrado |
| `fix` | Corrección de un bug |
| `style` | Cambios de estilo LVGL — colores, fuentes, layout — sin cambiar lógica |
| `refactor` | Reestructuración sin cambiar comportamiento observable |
| `perf` | Optimización de memoria, velocidad o consumo energético |
| `chore` | Cambios de configuración: `CMakeLists.txt`, `idf_component.yml`, `sdkconfig` |
| `docs` | Documentación únicamente |

### Ámbitos recomendados

| Ámbito | Qué cubre |
|--------|----------|
| `ui` | Código LVGL: widgets, estilos, animaciones genéricas |
| `screen` | Un controlador de pantalla específico (`scr_*.c`) |
| `bsp` | Integración con un periférico del BSP |
| `task` | Tarea FreeRTOS |
| `wifi` | Lógica de conexión y eventos WiFi |
| `nvs` | Lectura y escritura en NVS |
| `sd` | SD card y filesystem |

### Ejemplos reales

```
feat(screen): add splash screen with dot animation and fade-in
feat(bsp): read battery voltage from ADC on home screen
feat(ui): add lv_arc widget for battery level display
feat(task): create sensor task sending temperature via update queue
fix(screen): prevent direct LVGL call from sensor task context
style(ui): apply dark color scheme to meter and chart widgets
refactor(screen): extract threshold check to separate helper function
perf(task): reduce sensor task stack size from 4096 to 2048 bytes
chore: add temperature_sensor component to idf_component.yml
chore: enable CONFIG_TEMP_SENSOR_ENABLE in sdkconfig
```

### Reglas de estilo

- Descripción en **inglés**, en **imperativo** (`add`, `fix`, `move` — no `added`, `fixes`, `moving`)
- Sin punto al final de la primera línea
- Primera línea ≤ 72 caracteres
- Si necesitas explicar el porqué, usa el cuerpo separado por una línea en blanco

```bash
# Commit simple
git commit -m "feat(screen): add touch-anywhere handler to splash screen"

# Commit con cuerpo explicativo
git commit -m "$(cat <<'EOF'
fix(task): encode temperature as int32 centesimas before enqueue

LVGL update queue transfers void pointers. Passing a float directly
causes undefined behavior on Xtensa. Cast to int32 (value * 100)
before enqueuing and back to float in the handler.
EOF
)"
```

---

## 4. Cómo arrancar una asignación

### Paso 1 — Verificar que `main` está actualizado

```bash
git checkout main
git pull
```

### Paso 2 — Crear la rama de la asignación

```bash
# Ejemplo para la asignación 1
git checkout -b assignment/01-splash-events
```

### Paso 3 — Abrir el Dev Container

Abre la carpeta en VS Code y acepta **"Reopen in Container"** cuando aparezca el aviso, o usa:

```
Ctrl+Shift+P → Dev Containers: Reopen in Container
```

### Paso 4 — Verificar que el template base compila

```bash
idf.py build
```

Si compila sin errores, el entorno está listo para trabajar.

### Paso 5 — Hacer commits pequeños y frecuentes

No esperes a terminar todo para hacer un commit. Cada vez que una pieza funciona de forma estable, commitea:

```bash
git add components/lvgl_ui/screens/scr_splash.h
git add components/lvgl_ui/screens/scr_splash.c
git commit -m "feat(screen): implement splash screen skeleton with fade-in timer"
```

### Paso 6 — Subir la rama al remoto

```bash
# Primera vez
git push -u origin assignment/01-splash-events

# Pushes siguientes
git push
```

---

## Asignación 1 — Splash interactivo + widgets con datos reales del hardware

### Objetivo

Aprender el sistema de eventos de LVGL desde el primer día y leer periféricos reales del ESP32-S3 sin usar tareas FreeRTOS.

### Descripción

La app arranca con un **splash** que hace fade-in de la retroiluminación (LEDC vía BSP) durante 800 ms. El splash muestra el nombre de la aplicación y una animación de puntos suspensivos (`Iniciando`, `Iniciando.`, `Iniciando..`, `Iniciando...`) en loop. El usuario debe **tocar cualquier punto de la pantalla** para continuar — no hay botón visible.

La **pantalla principal** muestra tres widgets con datos reales, actualizados cada segundo mediante `lv_timer`:

| Widget | Dato | Fuente |
|--------|------|--------|
| `lv_arc` circular | Nivel de batería en % | ADC vía `bsp_battery` |
| `lv_label` | Temperatura interna del chip en °C | Driver `temperature_sensor` del ESP-IDF |
| `lv_label` | Tiempo activo en `HH:MM:SS` | `esp_timer_get_time()` |

### Conceptos LVGL

| Concepto | Aplicación |
|----------|-----------|
| `lv_obj_add_event_cb` | Capturar toque sobre el splash |
| `LV_EVENT_CLICKED`, `LV_EVENT_PRESSED` | Tipos de evento táctil |
| `LV_EVENT_SCREEN_LOAD_START` / `LV_EVENT_SCREEN_LOADED` | Sincronizar animaciones con la carga de pantalla |
| `lv_anim_t` | Fade-in de retroiluminación + animación de puntos |
| `lv_arc` con modo de ángulo personalizado | Widget de batería |
| `lv_timer_create` | Actualización periódica sin FreeRTOS |

### Periféricos

| Periférico | Uso |
|-----------|-----|
| ADC — `bsp_battery` | Nivel de batería en tiempo real |
| `temperature_sensor` interno (ESP32-S3) | Temperatura del chip |
| LEDC — `bsp_display_set_brightness` | Fade-in al arrancar |

### Pantallas

| Pantalla | Contenido |
|---------|----------|
| `scr_splash` | Nombre de app + animación de puntos + handler de toque global |
| `scr_home` | Arc de batería + label de temperatura + label de uptime |

### FreeRTOS

No requerido. Toda la lógica vive en timers LVGL y callbacks de evento.

### Commits sugeridos

```
feat(bsp): initialize temperature_sensor peripheral
feat(bsp): add backlight fade-in helper using lv_anim and LEDC
feat(screen): implement splash screen with dot animation
feat(screen): navigate from splash to home on touch anywhere
feat(screen): add battery arc widget reading ADC on home screen
feat(screen): add chip temperature label with 1s refresh timer
feat(screen): add uptime counter label with HH:MM:SS format
style(ui): change arc color based on battery level threshold
```

### Entregable

- Toque en cualquier punto del splash navega a home
- Los tres widgets muestran valores reales y se actualizan cada segundo
- La retroiluminación hace fade-in visible al arrancar
- Cero llamadas a `lv_*` fuera de un callback LVGL o timer LVGL

---

## Asignación 2 — Navegación por gestos, botón físico y brillo adaptativo

### Objetivo

Dominar gestos táctiles e input desde botón GPIO, y controlar periféricos del sistema directamente desde la UI.

### Descripción

Tres pantallas con contenido completo navegables mediante **swipe horizontal**. El botón físico de la placa (componente `espressif/button`) realiza una acción contextual diferente según la pantalla activa. La pantalla de Ajustes incluye un `lv_slider` que controla el brillo de la retroiluminación en tiempo real vía LEDC.

**Pantalla Home:** Resumen de los tres valores del hardware (batería, temperatura, uptime) con un indicador de estado general — ícono y color que cambian automáticamente según si la temperatura está en rango normal, en alerta o en crítico.

**Pantalla Sistema:** Datos del SoC en tiempo real — frecuencia de CPU, heap libre (`esp_get_free_heap_size()`), mínimo histórico de heap libre, versión de IDF, modelo de chip (`esp_chip_info()`), tamaño de flash.

**Pantalla Ajustes:** Slider de brillo con label de porcentaje, switch para habilitar/deshabilitar el timeout de pantalla, label con fecha y hora de compilación (`__DATE__`, `__TIME__`).

**Acción del botón físico por pantalla:**

| Pantalla | Acción |
|---------|--------|
| Home | Fuerza refresco inmediato de los tres valores |
| Sistema | Resetea el mínimo histórico de heap |
| Ajustes | Restaura brillo y switches a valores por defecto |

### Conceptos LVGL

| Concepto | Aplicación |
|----------|-----------|
| `LV_EVENT_GESTURE` + `lv_indev_get_gesture_dir()` | Swipe para navegar entre pantallas |
| `lv_group` + `lv_indev_set_group` | Mapear botón GPIO como dispositivo de entrada LVGL |
| `lv_slider` + `LV_EVENT_VALUE_CHANGED` | Control de brillo conectado a LEDC en tiempo real |
| `lv_switch` | Toggle de timeout de pantalla |
| `lv_obj_set_style_*` en runtime | Cambio de color del indicador de estado según valor |

### Periféricos

| Periférico | Uso |
|-----------|-----|
| Botón GPIO (`espressif/button`) | Acción contextual según pantalla activa |
| LEDC — `bsp_display_set_brightness` | Brillo controlado por el slider en tiempo real |

### Pantallas

| Pantalla | Barra de nav inferior |
|---------|----------------------|
| `scr_home` | Siempre visible con ícono activo de la sección actual |
| `scr_system` | |
| `scr_settings` | |

La barra de navegación inferior es un widget persistente — no es una pantalla separada. Se crea una vez y se reutiliza.

### FreeRTOS

No requerido. El callback del botón físico encola la acción via `ui_update_cmd()` para ejecutarla en el contexto del tick LVGL.

### Commits sugeridos

```
feat(bsp): register physical button with espressif/button component
feat(ui): add persistent bottom navigation bar
feat(screen): implement swipe gesture handler for screen navigation
feat(screen): add system info screen with esp_chip_info data
feat(screen): add heap minimum tracker with reset on button press
feat(screen): add brightness slider connected to LEDC in settings
feat(screen): add screen timeout toggle to settings
feat(task): route physical button press through ui_update_cmd
style(ui): add color-coded status indicator to home screen
fix(screen): guard gesture handler to ignore swipe during active scroll
```

### Entregable

- Swipe izquierda/derecha navega entre las tres pantallas
- La barra de navegación inferior refleja la pantalla activa
- El slider de Ajustes mueve el brillo físicamente en tiempo real
- El botón físico ejecuta su acción según la pantalla activa
- El indicador de estado cambia de color automáticamente

---

## Asignación 3 — Sensor en tiempo real, gráfico con scroll y alertas inline

### Objetivo

Integrar FreeRTOS correctamente con LVGL mediante la cola de comandos y visualizar series temporales con alertas que no interrumpen la navegación.

### Descripción

Una tarea FreeRTOS lee el sensor interno de temperatura cada 500 ms y envía el valor a la UI mediante `ui_update_cmd()`.

**Pantalla Medidor:** `lv_meter` con aguja animada con easing al nuevo valor. El arco de fondo cambia de color (verde / amarillo / rojo) según umbrales configurables. La alerta es **inline** — el color cambia sobre la pantalla activa sin modal ni navegación forzada.

**Pantalla Gráfico:** `lv_chart` de línea con los últimos 120 puntos (1 minuto de datos) en scroll automático. Labels de mínimo, máximo y promedio calculados sobre el buffer. Tocar sobre el gráfico mueve un cursor que muestra el valor exacto del punto seleccionado.

**Pantalla Historial:** `lv_table` con los últimos 10 eventos de alerta (timestamp de uptime + temperatura que disparó el evento). Las filas se añaden dinámicamente; al superar 10, la más antigua se descarta.

**Pantalla Ajustes:** `lv_slider` para configurar el umbral de alerta. El cambio es inmediato — el medidor refleja el nuevo umbral en el siguiente tick sin reiniciar la pantalla.

### Conceptos LVGL

| Concepto | Aplicación |
|----------|-----------|
| `lv_meter` con escala, agujas y rangos de color | Pantalla Medidor |
| `lv_anim_t` con `lv_anim_path_ease_in_out` | Movimiento suave de la aguja |
| `lv_chart` con `LV_CHART_UPDATE_MODE_SHIFT` | Scroll automático del gráfico |
| Cursor de `lv_chart` con evento táctil | Inspección de valores por toque |
| `lv_table` con filas dinámicas | Historial de alertas |
| `lv_obj_set_style_arc_color` en runtime | Alerta inline por cambio de color |

### Periféricos

| Periférico | Uso |
|-----------|-----|
| `temperature_sensor` interno (ESP32-S3) | Fuente de datos real a 500 ms |

### Pantallas

| Pantalla | Contenido principal |
|---------|-------------------|
| `scr_meter` | `lv_meter` con aguja animada y alerta de color |
| `scr_chart` | `lv_chart` con scroll, estadísticas y cursor táctil |
| `scr_history` | `lv_table` de eventos de alerta con scroll |
| `scr_settings` | Slider para configurar umbral de alerta |

### FreeRTOS

Una tarea productora a 500 ms. **Prohibido** llamar funciones `lv_*` desde la tarea — toda interacción con LVGL pasa por `ui_update_cmd()`. La violación de esta regla es el error más común de esta asignación.

### Commits sugeridos

```
chore: add temperature_sensor component dependency
feat(task): create sensor task reading chip temperature at 500ms
feat(ui): add UI_UPDATE_TEMPERATURA command to update queue
feat(screen): implement meter screen with lv_meter and color ranges
feat(screen): animate meter needle with ease-in-out on value change
feat(screen): add inline color alert based on configurable threshold
feat(screen): implement chart screen with auto-scroll and statistics
feat(screen): add touch cursor to chart for point inspection
feat(screen): implement alert history table with dynamic row insertion
feat(screen): add threshold configuration slider to settings
fix(task): cast temperature float to int32 centesimas before enqueue
fix(screen): clamp chart cursor index to valid buffer range
```

### Entregable

- La aguja del medidor anima suavemente con easing en cada actualización
- El gráfico hace scroll automático sin saltos ni artefactos visuales
- El historial acumula eventos de alerta y descarta los más antiguos
- Cero llamadas a `lv_*` desde la tarea de sensores — verificable con un assert
- El umbral configurable afecta el color del medidor en el siguiente tick

---

## Asignación 4 — WiFi, listas dinámicas y pantalla de conexión animada

### Objetivo

Manejar estados asíncronos complejos reflejados en la UI en tiempo real, y construir layouts con `lv_tabview`, listas dinámicas y paneles animados.

### Descripción

Al arrancar, antes del dashboard, aparece una **pantalla de conexión** con un `lv_spinner` animado y un label que refleja el estado actual del event loop WiFi: `SIN CONEXIÓN → CONECTANDO → IP OBTENIDA` (o `ERROR`). Si la conexión falla, aparece un `lv_msgbox` con botón de reintento. La pantalla espera indefinidamente — no tiene timeout.

El **dashboard principal** usa `lv_tabview` con tres pestañas navegables por swipe y por tap en la barra de pestañas:

**Pestaña Red:** RSSI actualizado cada 2 s como `lv_bar` con etiqueta de dBm. Dirección IP, gateway y canal WiFi. Un `lv_list` muestra los últimos 15 eventos del ciclo de vida WiFi (reconexión, pérdida de señal, IP obtenida) con timestamp de uptime. Tocar la barra de RSSI abre un **panel lateral** que entra desde el borde derecho con animación de posición X, mostrando detalles extendidos: BSSID, autenticación, potencia de señal histórica.

**Pestaña Sensores:** El medidor y el gráfico de la asignación anterior, integrados sin duplicar lógica.

**Pestaña Sistema:** Heap libre como gráfico histórico (últimos 60 puntos), temperatura del chip y uptime.

### Conceptos LVGL

| Concepto | Aplicación |
|----------|-----------|
| `lv_tabview` con scroll táctil | Estructura principal del dashboard |
| `lv_spinner` | Indicador animado durante la conexión |
| `lv_list` con ítems dinámicos | Log de eventos WiFi |
| `lv_anim` sobre coordenada X (`lv_obj_set_x`) | Panel lateral con entrada/salida animada |
| `lv_msgbox` | Error de conexión con acción de reintento |
| Máquina de estados (`enum` + `switch`) | Control de la pantalla de conexión |

### Periféricos

| Periférico | Uso |
|-----------|-----|
| WiFi (`bsp_wifi`) | Conexión y lectura periódica de RSSI |
| `temperature_sensor` | Datos en la pestaña Sensores |

### Pantallas

| Pantalla | Contenido |
|---------|----------|
| `scr_connecting` | Spinner + label de estado + msgbox de error |
| `scr_dashboard` | Tabview con tres pestañas |
| Panel lateral (overlay sobre `scr_dashboard`) | Detalles extendidos de la conexión WiFi |

### FreeRTOS

- Tarea de sensores heredada de la asignación anterior
- Eventos WiFi registrados con `esp_event_handler_register` — cada evento encola un comando a la UI

### Commits sugeridos

```
feat(wifi): register WiFi event handlers and connect on boot
feat(ui): add UI_UPDATE_WIFI_STATE and UI_UPDATE_RSSI commands
feat(screen): add connection screen with lv_spinner and state machine
feat(screen): show lv_msgbox on WiFi failure with retry button
feat(screen): implement dashboard with lv_tabview and three tabs
feat(screen): add RSSI bar with 2s refresh in network tab
feat(screen): populate WiFi event log dynamically in lv_list
feat(screen): add slide-in lateral panel with X-axis animation
feat(screen): dismiss lateral panel on tab change event
fix(screen): prevent panel animation conflict on rapid tab switches
```

### Entregable

- La UI refleja cada transición del ciclo de vida WiFi sin bloqueos
- El panel lateral entra y sale con animación fluida desde el borde derecho
- El tabview es navegable por swipe y por tap en las pestañas
- El log de eventos WiFi se actualiza en orden con timestamp

---

## Asignación 5 — Dashboard de producción: SD, NVS, tema personalizado y UX completa

### Objetivo

Construir una aplicación embebida completa y lista para demostrar: persistencia de configuración, logging en SD, tema visual consistente y UX sin fricciones.

### Descripción

Todos los ajustes (umbrales, brillo, intervalo de muestreo, SSID/contraseña WiFi) se persisten en **NVS** y se cargan automáticamente al arrancar. La **SD card** graba un CSV rotativo con timestamp Unix, temperatura, RSSI y heap libre. Un botón en la pantalla de Ajustes exporta los últimos 500 registros y muestra el path del archivo en un `lv_textarea` de solo lectura.

La app tiene un **tema oscuro personalizado** (`lv_theme_t`) con paleta de colores coherente aplicada a todos los widgets. El timeout de inactividad apaga la retroiluminación (no la app) y la restaura con fade-in en el siguiente toque. Un `lv_dropdown` en Ajustes permite cambiar el intervalo de muestreo (500 ms / 1 s / 5 s) — la tarea de sensores ajusta su delay en runtime sin reiniciarse.

Las alertas críticas se muestran como `lv_msgbox` modal que bloquea la interacción hasta que el usuario las confirma explícitamente.

### Conceptos LVGL

| Concepto | Aplicación |
|----------|-----------|
| `lv_theme_t` personalizado con callbacks de estilo | Paleta de colores global consistente |
| `lv_textarea` de solo lectura | Visor del path del CSV exportado |
| `lv_dropdown` | Selección del intervalo de muestreo |
| `LV_OBJ_FLAG_EVENT_BUBBLE` | Capturar toque en cualquier widget para restaurar brillo |
| `lv_mem_monitor()` | Diagnóstico de memoria LVGL en la pestaña Sistema |

### Periféricos

| Periférico | Uso |
|-----------|-----|
| NVS | Persistencia de todos los ajustes entre reinicios |
| SD card (`bsp_sdcard`) | Logging CSV rotativo continuo |
| WiFi (`bsp_wifi`) | RSSI y conectividad |
| `temperature_sensor` | Datos de sensores |
| ADC — `bsp_battery` | Nivel de batería en el dashboard |
| LEDC | Brillo adaptativo con fade-in tras inactividad |

### Pantallas

| Pantalla | Contenido |
|---------|----------|
| `scr_splash` | Fade-in de retroiluminación + touch to start |
| `scr_connecting` | Estado de conexión WiFi (reutilizada de asignación 4) |
| `scr_dashboard` | Tabview: Resumen / Sensores / Red / Sistema |
| `scr_settings` | Sub-secciones: Monitorización y Conexión |
| Modal de alerta | `lv_msgbox` bloqueante hasta confirmación |

### FreeRTOS

| Tarea | Responsabilidad |
|-------|----------------|
| Tarea de sensores | Lectura a intervalo configurable en runtime |
| Tarea de SD | Escritura del CSV con mutex para acceso exclusivo |
| Event loop WiFi | Eventos de red encolados a la UI |

### Commits sugeridos

```
chore: initialize NVS flash partition on boot
feat(nvs): save and restore all settings from NVS
feat(bsp): mount SD card and create rotating CSV log file
feat(task): write sensor row to CSV every sample interval
feat(task): protect CSV file access with FreeRTOS mutex
feat(screen): add export button to settings showing CSV file path
feat(ui): implement custom dark lv_theme_t with brand color palette
feat(screen): add sample interval dropdown to settings
feat(task): adjust sensor task delay from UI_UPDATE_SET_INTERVAL command
feat(screen): implement critical alert modal with lv_msgbox
feat(bsp): fade-in backlight on any touch after inactivity timeout
perf(ui): reuse table rows instead of rebuilding list on each update
fix(task): flush CSV buffer before SD unmount on shutdown
```

### Entregable

- Los ajustes persisten correctamente tras un corte de energía
- El CSV se escribe de forma continua sin bloquear la UI ni perder frames
- El tema oscuro aplica a todos los widgets de forma visual consistente
- El modal bloquea la interacción hasta la confirmación del usuario
- El intervalo de muestreo cambia en runtime sin reiniciar la tarea de sensores
- La retroiluminación se apaga y restaura con fade, sin perder el estado de la app

---

*Consulta el [README principal](README.md) para instrucciones de instalación de Docker, compilación, flasheo y arquitectura del proyecto.*
