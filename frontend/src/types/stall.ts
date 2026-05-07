export type StallKind = "male" | "female";

export interface StallState {
  occupied: boolean;
  duration: number; // seconds since last state change
  copyIdx: number; // index into playful copy phrase array
  lastChange: number; // unix timestamp ms — triggers timer reset in StallCard
}

export interface AppStalls {
  male: StallState;
  female: StallState;
}

export interface OccupancyUpdate {
  stallId: StallKind;
  occupied: boolean;
  ts: number;
}
