export type StallKind = "male" | "female";

export interface StallState {
  occupied: boolean;
  duration: number; // seconds since last state change
  copyIdx: number; // index into playful copy phrase array
  notify: boolean; // user opted in for "notify when free"
  lastChange: number; // unix timestamp ms — triggers timer reset in StallCard
}

export interface AppStalls {
  male: StallState;
  female: StallState;
}

// TODO: Replace with the actual WebSocket message shape sent by the ESP32 backend
export interface OccupancyUpdate {
  stallId: StallKind;
  occupied: boolean;
  ts: number;
}
