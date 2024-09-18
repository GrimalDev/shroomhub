package main

import (
	"encoding/json"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"github.com/jmoiron/sqlx"
	"github.com/joho/godotenv"
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	"golang.org/x/net/websocket"
	"net/http"
	"os"
)

var db *sqlx.DB

type SensorData struct {
	Co2          int32   `json:"co2" db:"co2"`
	Humidity     float32 `json:"humi" db:"humidity"`
	Temperature  float32 `json:"temp" db:"temperature"`
	Luminosity   float32 `json:"lum" db:"luminosity"`
	CreationDate string  `json:"creation_date" db:"creation_date"`
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

	db, err = sqlx.Open("mysql", dbUser+":"+dbPass+"@tcp("+dbHost+":3306)/shroomhub")
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

func wsHandler(c echo.Context) error {
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

func sensorDataHandler(c echo.Context) error {
	var sensorDataArray []SensorData

	err := db.Select(&sensorDataArray, "SELECT co2, humidity, temperature, creation_date FROM (SELECT *, ROW_NUMBER() OVER (PARTITION BY DATE_FORMAT(creation_date, '%Y-%m-%d %H:00:00') ORDER BY creation_date DESC) as row_num FROM sensor_data WHERE creation_date >= DATE_SUB(NOW(), INTERVAL 24 HOUR)) as subquery WHERE row_num = 1 ORDER BY creation_date ASC")
	if err != nil {
		fmt.Println(err)
		return c.JSON(http.StatusInternalServerError, map[string]string{"error": "Error fetching data from the database"})
	}

	// for i, j := 0, len(sensorDataArray)-1; i < j; i, j = i+1, j-1 {
	// 	sensorDataArray[i], sensorDataArray[j] = sensorDataArray[j], sensorDataArray[i]
	// }

	return c.JSON(http.StatusOK, sensorDataArray)
}

func main() {
	initEnv()
	initDB()

	e := echo.New()
	// Add CORS middleware
	e.Use(middleware.CORSWithConfig(middleware.CORSConfig{
		AllowOrigins: []string{"*"},
		AllowMethods: []string{http.MethodGet, http.MethodPost, http.MethodPut, http.MethodDelete},
	}))

	e.GET("/", func(c echo.Context) error {
		return c.String(http.StatusOK, "Hello, World!")
	})
	e.GET("/ws", wsHandler)
	e.GET("/sensordata", sensorDataHandler)
	e.Logger.Fatal(e.Start(":4040"))
}
