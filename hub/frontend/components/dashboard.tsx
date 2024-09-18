"use client";

import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuLabel,
  DropdownMenuSeparator,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";
import {
  Bell,
  ChevronDown,
  LayoutDashboard,
  Settings,
  User,
} from "lucide-react";

import { useState, useEffect } from "react";
// Function to fetch sensor data from the API

interface SensorData {
  creation_date: string;
  humidity: number;
  temperature: number;
  co2: number;
}

async function fetchSensorData(): Promise<SensorData[]> {
  try {
    const response = await fetch(
      "http://srv.grimaldev.local:4040/sensordata?start=2024-09-18&end=2024-09-18",
    );
    if (!response.ok) {
      throw new Error("Network response was not ok");
    }
    const data = await response.json();

    return data.map((record: any) => ({
      creation_date: new Date(record.creation_date).toLocaleTimeString([], {
        hour: "2-digit",
        minute: "2-digit",
        hour12: false,
        timeZone: "UTC",
      }),
      humidity: record.humi,
      temperature: record.temp,
      co2: record.co2,
    }));
  } catch (error) {
    console.error("Error fetching sensor data:", error);
    return [];
  }
}
export function Dashboard() {
  const [sensorData, setSensorData] = useState<SensorData[]>([]);

  useEffect(() => {
    async function loadSensorData() {
      const data = await fetchSensorData();
      setSensorData(data);
    }
    loadSensorData();
  }, []);

  return (
    <div className="flex h-screen bg-gray-100 dark:bg-gray-900">
      <aside className="w-64 bg-white dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700">
        <div className="p-4">
          <h2 className="text-2xl font-bold text-gray-800 dark:text-white">
            ShroomHub
          </h2>
        </div>
        <nav className="mt-4">
          <a
            href="#"
            className="flex items-center px-4 py-2 text-gray-700 bg-gray-100 dark:bg-gray-700 dark:text-gray-200"
          >
            <LayoutDashboard className="mr-3 h-5 w-5" />
            Dashboard
          </a>
          <a
            href="#"
            className="flex items-center px-4 py-2 text-gray-600 hover:bg-gray-100 dark:text-gray-400 dark:hover:bg-gray-700"
          >
            <Settings className="mr-3 h-5 w-5" />
            Settings
          </a>
        </nav>
      </aside>

      <main className="flex-1 overflow-y-auto">
        <header className="bg-white dark:bg-gray-800 shadow">
          <div className="max-w-7xl mx-auto py-4 px-4 sm:px-6 lg:px-8 flex justify-between items-center">
            <h1 className="text-2xl font-semibold text-gray-900 dark:text-white">
              Dashboard
            </h1>
            <div className="flex items-center">
              <Button variant="outline" size="icon" className="mr-2">
                <Bell className="h-4 w-4" />
              </Button>
            </div>
          </div>
        </header>

        <div className="max-w-7xl mx-auto py-6 sm:px-6 lg:px-8">
          <div className="px-4 py-6 sm:px-0">
            <Tabs defaultValue="all" className="w-full">
              <TabsList>
                <TabsTrigger value="all">All Metrics</TabsTrigger>
                <TabsTrigger value="humidity">Humidity</TabsTrigger>
                <TabsTrigger value="temperature">Temperature</TabsTrigger>
                <TabsTrigger value="co2">CO2</TabsTrigger>
              </TabsList>
              <TabsContent value="all">
                <div className="grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-3">
                  <MetricCard
                    title="Humidity"
                    data={sensorData}
                    dataKey="humidity"
                    unit="%"
                    color="#8884d8"
                  />
                  <MetricCard
                    title="Temperature"
                    data={sensorData}
                    dataKey="temperature"
                    unit="°C"
                    color="#82ca9d"
                  />
                  <MetricCard
                    title="CO2 Level"
                    data={sensorData}
                    dataKey="co2"
                    unit="ppm"
                    color="#ffc658"
                  />
                </div>
              </TabsContent>
              <TabsContent value="humidity">
                <MetricCard
                  title="Humidity"
                  data={sensorData}
                  dataKey="humidity"
                  unit="%"
                  color="#8884d8"
                  fullWidth
                />
              </TabsContent>
              <TabsContent value="temperature">
                <MetricCard
                  title="Temperature"
                  data={sensorData}
                  dataKey="temperature"
                  unit="°C"
                  color="#82ca9d"
                  fullWidth
                />
              </TabsContent>
              <TabsContent value="co2">
                <MetricCard
                  title="CO2 Level"
                  data={sensorData}
                  dataKey="co2"
                  unit="ppm"
                  color="#ffc658"
                  fullWidth
                />
              </TabsContent>
            </Tabs>
          </div>
        </div>
      </main>
    </div>
  );
}

function MetricCard({ title, data, dataKey, unit, color, fullWidth = false }) {
  return (
    <Card className={fullWidth ? "w-full" : "w-full md:w-auto"}>
      <CardHeader>
        <CardTitle>{title}</CardTitle>
      </CardHeader>
      <CardContent>
        {data.length === 0 ? (
          <div className="h-[200px] flex items-center justify-center">
            <p className="text-gray-500">No data available</p>
          </div>
        ) : (
          <>
            <div className={fullWidth ? "h-[400px]" : "h-[200px]"}>
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={data}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="creation_date" />
                  <YAxis />
                  <Tooltip />
                  <Line type="monotone" dataKey={dataKey} stroke={color} />
                </LineChart>
              </ResponsiveContainer>
            </div>
            <div className="mt-4 flex justify-between items-center">
              <span className="text-2xl font-bold">
                {data[data.length - 1][dataKey]}
                {unit}
              </span>
              <span className="text-sm text-gray-500">
                Last updated: {data[data.length - 1].creation_date}
              </span>
            </div>
          </>
        )}
      </CardContent>
    </Card>
  );
}
