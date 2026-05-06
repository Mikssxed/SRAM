import { useQuery } from "@tanstack/vue-query";
import axios from "axios";
import { ref, watch } from "vue";
import type { SensorReading } from "../types/sensor";

const API_URL = "/api/readings";
const POLL_INTERVAL_MS = 10_000;

function startOfToday(): number {
  const d = new Date();
  d.setHours(0, 0, 0, 0);
  return d.getTime();
}

// Realistic mock readings for a typical office day — used when the API is unreachable
function generateMockReadings(): SensorReading[] {
  const base = startOfToday();
  const now = Date.now();
  // [start_minutes_from_midnight, duration_minutes]
  const visits: [number, number][] = [
    [490, 5],
    [522, 3],
    [557, 7],
    [601, 4],
    [638, 6],
    [672, 3],
    [722, 8],
    [767, 5],
    [802, 4],
    [843, 6],
    [877, 3],
    [912, 5],
    [947, 7],
    [982, 4],
  ];
  const readings: SensorReading[] = [];
  for (const [startMin, durMin] of visits) {
    const startMs = base + startMin * 60_000;
    const endMs = startMs + durMin * 60_000;
    if (startMs > now) break;
    readings.push({ time: new Date(startMs).toISOString(), cm: 42 });
    if (endMs <= now)
      readings.push({ time: new Date(endMs).toISOString(), cm: 110 });
  }
  return readings;
}

export function useSensorData() {
  // Start with mock data immediately so the UI is never blank
  const readings = ref<SensorReading[]>(generateMockReadings());
  const usingMock = ref(true);
  // Cursor: moves forward to the latest seen timestamp after each successful poll
  const lastTs = ref<number>(Date.now() - 24 * 60 * 60 * 1000);

  const { data, isLoading, isError } = useQuery({
    queryKey: ["sensor-readings"],
    queryFn: () =>
      axios
        .get<SensorReading[]>(API_URL, { params: { since: lastTs.value } })
        .then((r) => r.data),
    refetchInterval: POLL_INTERVAL_MS,
    // Always treat cached result as stale so each interval triggers a real request
    staleTime: 0,
    // Don't let TanStack retry on network failure — mock data is already shown
    retry: false,
  });

  watch(data, (newRows) => {
    if (!Array.isArray(newRows) || !newRows.length) return;
    const todayStart = startOfToday();
    if (usingMock.value) {
      // First real API response — replace mock data entirely
      readings.value = newRows.filter(
        (r) => new Date(r.time).getTime() >= todayStart,
      );
      usingMock.value = false;
    } else {
      // Subsequent polls — accumulate only new rows
      readings.value = [...readings.value, ...newRows].filter(
        (r) => new Date(r.time).getTime() >= todayStart,
      );
    }
    lastTs.value = Math.max(...newRows.map((r) => new Date(r.time).getTime()));
  });

  return { readings, isLoading, isError };
}
