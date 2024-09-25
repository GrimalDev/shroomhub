# Shroomhub

Schroomhub is a contraction of mushroom and hub. The goal is to have a visual gui dashboardl. It connects to a server that initiates a websocket with a microcontroller.

---

> [!IMPORTANT]
>
> **Documentation in progress**
>
> [!IMPORTANT]

> [!NOTE]
>
> All test, build or startup scripts can be found in the .run folder:
>
> ```bash
> bash .run/<script>
> ```

# Deployment

All deployment or tests can be done using the [docker compose](https://github.com/GrimalDev/shroomhub/blob/main/docker-compose.yml) description file.
But if you still want to go ahead, you might want to take a look a those sections:

## Base microcontroller code

- Temperature
- Humidity
- Gaz sensor configured for co2 read
- Lux indicator
- Display
- Mist water atomizer

### How to build

You shold have some form of [PIO setup](https://docs.platformio.org/en/latest/core/installation/index.html) before going any further.

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
- [ ] Add Basic Arduino IDE support ?

> [!IMPORTANT]
>
> **Documentation in progress**
>
> [!IMPORTANT]
