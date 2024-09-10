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

// Mock data - replace this with your actual data fetching logic
const mockData = [
  { time: "00:00", humidity: 85, temperature: 24, co2: 800 },
  { time: "04:00", humidity: 82, temperature: 23, co2: 850 },
  { time: "08:00", humidity: 88, temperature: 25, co2: 900 },
  { time: "12:00", humidity: 90, temperature: 26, co2: 950 },
  { time: "16:00", humidity: 87, temperature: 25, co2: 920 },
  { time: "20:00", humidity: 86, temperature: 24, co2: 880 },
];

export function Dashboard() {
  return (
    <div className="flex h-screen bg-gray-100 dark:bg-gray-900">
      {/* Sidebar */}
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

      {/* Main Content */}
      <main className="flex-1 overflow-y-auto">
        {/* Header */}
        <header className="bg-white dark:bg-gray-800 shadow">
          <div className="max-w-7xl mx-auto py-4 px-4 sm:px-6 lg:px-8 flex justify-between items-center">
            <h1 className="text-2xl font-semibold text-gray-900 dark:text-white">
              Dashboard
            </h1>
            //{" "}
            <div className="flex items-center">
              const metrics = fetch("http://localhost:4040/metrics"); //{" "}
              <Button variant="outline" size="icon" className="mr-2">
                // <Bell className="h-4 w-4" />
                //{" "}
              </Button>
              //{" "}
            </div>
          </div>
        </header>

        {/* Dashboard Content */}
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
                    data={mockData}
                    dataKey="humidity"
                    unit="%"
                    color="#8884d8"
                  />
                  <MetricCard
                    title="Temperature"
                    data={mockData}
                    dataKey="temperature"
                    unit="°C"
                    color="#82ca9d"
                  />
                  <MetricCard
                    title="CO2 Level"
                    data={mockData}
                    dataKey="co2"
                    unit="ppm"
                    color="#ffc658"
                  />
                </div>
              </TabsContent>
              <TabsContent value="humidity">
                <MetricCard
                  title="Humidity"
                  data={mockData}
                  dataKey="humidity"
                  unit="%"
                  color="#8884d8"
                  fullWidth
                />
              </TabsContent>
              <TabsContent value="temperature">
                <MetricCard
                  title="Temperature"
                  data={mockData}
                  dataKey="temperature"
                  unit="°C"
                  color="#82ca9d"
                  fullWidth
                />
              </TabsContent>
              <TabsContent value="co2">
                <MetricCard
                  title="CO2 Level"
                  data={mockData}
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
        <div className={fullWidth ? "h-[400px]" : "h-[200px]"}>
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={data}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis dataKey="time" />
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
            Last updated: {data[data.length - 1].time}
          </span>
        </div>
      </CardContent>
    </Card>
  );
}

