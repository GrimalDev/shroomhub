# Shroomhub

Schroomhub is a contraction of mushroom and hub. The goal is to have a visual gui dashboardl. It connects to a server that initiates a websocket with a microcontroller.

---

> [!NOTE]
>
> All test, build or startup scripts can be found in the .run folder:
>
> ```bash
> bash .run/<script>
> ```

## Base microcontroller code

- Temperature
- Humidity
- Gaz sensor configured for co2 read
- Lux indicator
- Display
- Mist water atomizer

### How to build

1. _(optional)_ Run the build script `.run/build`.
2. Connect your ESP (For now only ESP8266).
3. Run the uplaod script `.run/upload`.

## "IOT" Cloud

### Build the web server

1. `go mod tidy`

## Dashboard

### How to compile

1. `bun i` / `npm i` (Not tested with deno, yarn or pnpn. But should be drop-in replacements).

# Roadmap

- [x] "Parrallel" probe reading
- [ ] Support for more platforms
