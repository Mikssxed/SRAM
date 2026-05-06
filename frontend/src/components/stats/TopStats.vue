<script setup lang="ts">
import { computed } from "vue";
import type { AppStalls } from "../../types/stall";
import type { DerivedStats } from "../../utils/sensorCalc";
import { fmtMMSS } from "../../utils/time";

const props = defineProps<{
  states: AppStalls;
  derived: DerivedStats;
}>();

interface StatItem {
  label: string;
  value: string;
  colorClass: string;
}

const stats = computed<StatItem[]>(() => [
  {
    label: "AVAILABLE NOW",
    value: props.states.male.occupied ? "0/1" : "1/1",
    colorClass: props.states.male.occupied
      ? "text-live-red"
      : "text-live-green",
  },
  {
    label: "VISITS · TODAY",
    value: String(props.derived.visitsToday),
    colorClass: "text-content",
  },
  {
    label: "AVG DURATION",
    value: props.derived.visitsToday
      ? fmtMMSS(props.derived.avgDurationSec)
      : "--:--",
    colorClass: "text-content",
  },
  {
    label: "PEAK HOUR",
    value: props.derived.visitsToday ? `${props.derived.peakHour}:00` : "--:--",
    colorClass: "text-amber",
  },
]);
</script>

<template>
  <div class="grid grid-cols-2 sm:grid-cols-4 gap-3">
    <div
      v-for="stat in stats"
      :key="stat.label"
      class="bg-surface border border-edge rounded-2xl px-5.5 py-4.5"
    >
      <div
        class="text-[10px] tracking-[1.5px] text-content-mute font-bold font-mono mb-2"
      >
        {{ stat.label }}
      </div>
      <div
        :class="[
          'text-[26px] font-extrabold tracking-[-0.5px] font-mono',
          stat.colorClass,
        ]"
      >
        {{ stat.value }}
      </div>
    </div>
  </div>
</template>
