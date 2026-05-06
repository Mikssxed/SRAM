import type { SensorReading } from "../types/sensor";

export const OCCUPANCY_THRESHOLD_CM = 80;

export function isOccupied(cm: number): boolean {
  return cm > 0 && cm < OCCUPANCY_THRESHOLD_CM;
}

export interface DerivedStats {
  occupied: boolean;
  lastChangedAt: number; // unix ms — when the current state began
  visitsToday: number;
  avgDurationSec: number; // average of completed visits only
  peakHour: number; // 0–23
  busyHours: number[]; // 24-element: visit starts per hour
}

export function deriveAll(readings: SensorReading[]): DerivedStats {
  const empty = (): DerivedStats => ({
    occupied: false,
    lastChangedAt: Date.now(),
    visitsToday: 0,
    avgDurationSec: 0,
    peakHour: 0,
    busyHours: new Array<number>(24).fill(0),
  });

  if (readings.length === 0) return empty();

  // One sort — O(n log n)
  const sorted = [...readings].sort(
    (a, b) => new Date(a.time).getTime() - new Date(b.time).getTime(),
  );

  const busyHours = new Array<number>(24).fill(0);
  let completedCount = 0;
  let totalDurationMs = 0;
  let currentStart: Date | null = null;
  let currentOccupied = false;
  // Tracks when the last free↔occupied transition occurred
  let lastChangedAt = new Date(sorted[0].time).getTime();

  // One pass — O(n)
  for (const r of sorted) {
    const occ = isOccupied(r.cm);

    if (occ && !currentOccupied) {
      // free → occupied: start a visit
      currentStart = new Date(r.time);
      currentOccupied = true;
      lastChangedAt = currentStart.getTime();
    } else if (!occ && currentOccupied && currentStart) {
      // occupied → free: close the visit
      const endMs = new Date(r.time).getTime();
      completedCount++;
      totalDurationMs += endMs - currentStart.getTime();
      busyHours[currentStart.getHours()]++;
      currentStart = null;
      currentOccupied = false;
      lastChangedAt = endMs;
    }
  }

  // Count ongoing visit (stall still occupied at the end of readings)
  if (currentOccupied && currentStart) {
    busyHours[currentStart.getHours()]++;
  }

  // Peak hour — O(24), negligible
  let peakHour = 0;
  let peakCount = 0;
  for (let h = 0; h < 24; h++) {
    if (busyHours[h] > peakCount) {
      peakCount = busyHours[h];
      peakHour = h;
    }
  }

  return {
    occupied: currentOccupied,
    lastChangedAt,
    visitsToday: completedCount + (currentOccupied ? 1 : 0),
    avgDurationSec:
      completedCount > 0
        ? Math.round(totalDurationMs / completedCount / 1000)
        : 0,
    peakHour,
    busyHours,
  };
}
