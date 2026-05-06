export interface SensorReading {
  time: string; // ISO 8601 timestamp from DB
  cm: number; // raw distance from ultrasonic sensor
}
