import type { OccupancyEntry } from "../types/sensor";

export interface SittingRecord {
  rank: number;
  durationMs: number;
  startedAt: number;
}

export interface DerivedStats {
  occupied: boolean;
  lastChangedAt: number; // unix ms — when the current state began
  visitsToday: number;
  avgDurationSec: number; // average of completed visits only
  peakHour: number; // 0–23
  busyHours: number[]; // 24-element: visit starts per hour
  topSittings: SittingRecord[]; // top 10 longest completed sittings, sorted by duration desc
}

export function deriveAll(entries: OccupancyEntry[]): DerivedStats {
  const empty = (): DerivedStats => ({
    occupied: false,
    lastChangedAt: Date.now(),
    visitsToday: 0,
    avgDurationSec: 0,
    peakHour: 0,
    busyHours: new Array<number>(24).fill(0),
    topSittings: [],
  });

  if (entries.length === 0) return empty();

  // One sort — O(n log n)
  const sorted = [...entries].sort((a, b) => a.timestamp - b.timestamp);

  // One filter pass — drop entries whose state lasted ≤ 10 s (sensor noise)
  const MIN_DURATION_MS = 10_000;
  const denoised = sorted.filter((_, i) => {
    const next = sorted[i + 1];
    return !next || next.timestamp - sorted[i].timestamp > MIN_DURATION_MS;
  });

  const busyHours = new Array<number>(24).fill(0);
  const completedSittings: { durationMs: number; startedAt: number }[] = [];
  let completedCount = 0;
  let totalDurationMs = 0;
  let currentStart: number | null = null;
  let currentOccupied = false;
  let lastChangedAt = denoised[0]?.timestamp ?? sorted[0].timestamp;

  // One pass — O(n)
  for (const entry of denoised) {
    if (entry.occupied && !currentOccupied) {
      // free → occupied: start a visit
      currentStart = entry.timestamp;
      currentOccupied = true;
      lastChangedAt = entry.timestamp;
    } else if (!entry.occupied && currentOccupied && currentStart !== null) {
      // occupied → free: close the visit
      const durationMs = entry.timestamp - currentStart;
      completedCount++;
      totalDurationMs += durationMs;
      completedSittings.push({ durationMs, startedAt: currentStart });
      busyHours[new Date(currentStart).getHours()]++;
      currentStart = null;
      currentOccupied = false;
      lastChangedAt = entry.timestamp;
    }
  }

  // Count ongoing visit (stall still occupied at end of entries)
  if (currentOccupied && currentStart !== null) {
    busyHours[new Date(currentStart).getHours()]++;
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

  const topSittings: SittingRecord[] = completedSittings
    .sort((a, b) => b.durationMs - a.durationMs)
    .slice(0, 10)
    .map((s, i) => ({ ...s, rank: i + 1 }));

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
    topSittings,
  };
}
