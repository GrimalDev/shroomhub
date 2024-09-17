package main

import (
	"encoding/json"
	"fmt"
	"log"
	"time"

	"golang.org/x/net/websocket"
)

type SensorData struct {
	Humi       float64 `json:"humi"`
	Temp       float64 `json:"temp"`
	CO2        int     `json:"co2"`
	FoggerGoal int     `json:"foggerGoal"`
}

func main() {
	// WebSocket server URL
	url := "ws://localhost:4040/ws"

	// Create the sensor data
	data := SensorData{
		Humi:       77.19999695,
		Temp:       19.60000038,
		CO2:        167,
		FoggerGoal: 90,
	}

	// Send data every second
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for {
		// Connect to the WebSocket server
		ws, err := websocket.Dial(url, "", "http://localhost/")
		if err != nil {
			log.Println("Error connecting to WebSocket server:", err)
			time.Sleep(5 * time.Second) // Wait before retrying
			continue
		}

		log.Println("Connected to WebSocket server")

		for {
			select {
			case <-ticker.C:
				jsonData, err := json.Marshal(data)
				if err != nil {
					log.Println("Error marshaling JSON:", err)
					continue
				}

				err = websocket.JSON.Send(ws, data)
				if err != nil {
					log.Println("Error sending message:", err)
					break // Break the inner loop to trigger reconnection
				}

				fmt.Println("Sent:", string(jsonData))
			}
		}

		log.Println("Connection lost. Reconnecting...")
		ws.Close()
		time.Sleep(2 * time.Second) // Wait before reconnecting
	}
}
