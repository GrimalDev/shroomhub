package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"github.com/joho/godotenv"
	"github.com/labstack/echo/v4"
	"golang.org/x/net/websocket"
	"net/http"
	"os"
)

var db *sql.DB

type SensorData struct {
	Co2         int32   `json:"co2"`
	Humidity    float32 `json:"humi"`
	Temperature float32 `json:"temp"`
	Luminosity  float32 `json:"lum"`
}

var lastSensorData SensorData

func initEnv() {
	err := godotenv.Load()
	if err != nil {
		panic(err)
	}
	fmt.Println("Environment initialized !")
}

func initDB() {
	var err error

	dbUser := os.Getenv("DB_USER")
	dbPass := os.Getenv("DB_PASS")
	dbHost := os.Getenv("DB_HOST")

	db, err = sql.Open("mysql", dbUser+":"+dbPass+"@tcp("+dbHost+":3306)/shroomhub")
	if err != nil {
		panic(err)
	}

	err = db.Ping()
	if err != nil {
		panic(err)
	}

	if err != nil {
		panic(err)
	}

	fmt.Println("Connected to MySQL!")
}

func appendSensorData(data SensorData) error {

	// fmt.Print(data)

	if int(lastSensorData.Co2) == int(data.Co2) && int(lastSensorData.Humidity) == int(data.Humidity) && int(lastSensorData.Temperature) == int(data.Temperature) {
		// fmt.Print("\n")
		return nil
	}

	// fmt.Print(" - Inserting data\n")

	_, err := db.Exec("INSERT INTO sensor_data (co2, humidity, temperature) VALUES (?, ?, ?)", data.Co2, data.Humidity, data.Temperature)
	if err != nil {
		return err
	}

	lastSensorData = data

	return nil
}

func sensorHandler(c echo.Context) error {
	websocket.Handler(func(ws *websocket.Conn) {
		defer ws.Close()
		for {
			msg := ""
			err := websocket.Message.Receive(ws, &msg)
			if err != nil {
				c.Logger().Error(err)
				break
			}

			sensorData := SensorData{}
			err = json.Unmarshal([]byte(msg), &sensorData)
			if err != nil {
				c.Logger().Error(err)
			}

			err = appendSensorData(sensorData)
			if err != nil {
				c.Logger().Error(err)
			}
		}
	}).ServeHTTP(c.Response(), c.Request())

	return nil
}

func main() {
	initEnv()
	initDB()

	e := echo.New()
	e.GET("/", func(c echo.Context) error {
		return c.String(http.StatusOK, "Hello, World!")
	})
	e.GET("/ws", sensorHandler)
	e.Logger.Fatal(e.Start(":4040"))
}
