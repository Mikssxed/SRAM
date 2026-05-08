<script setup lang="ts">
import { computed } from "vue";
import type { SittingRecord } from "../../utils/sensorCalc";
import { formatDurationMs, formatTimeOfDay } from "../../utils/time";

const props = defineProps<{
  sitting: SittingRecord;
  maxDurationMs: number;
}>();

const barWidth = computed(() =>
  props.maxDurationMs > 0
    ? `${Math.round((props.sitting.durationMs / props.maxDurationMs) * 100)}%`
    : "0%",
);

const rankColorClass = computed(() => {
  switch (props.sitting.rank) {
    case 1:
      return "text-amber";
    case 2:
      return "text-content-dim";
    case 3:
      return "text-live-red";
    default:
      return "text-content-mute";
  }
});

const barColorClass = computed(() => {
  switch (props.sitting.rank) {
    case 1:
      return "bg-amber";
    case 2:
      return "bg-content-dim";
    case 3:
      return "bg-live-red";
    default:
      return "bg-edge";
  }
});

const formattedDuration = computed(() =>
  formatDurationMs(props.sitting.durationMs),
);
const formattedTimeOfDay = computed(() =>
  formatTimeOfDay(props.sitting.startedAt),
);
</script>

<template>
  <div
    class="flex items-center gap-3 sm:gap-4 px-5 py-3.5 border-b border-edge last:border-b-0"
  >
    <!-- Rank -->
    <span
      :class="[
        'w-7 shrink-0 text-center font-mono text-sm font-extrabold tracking-tight',
        rankColorClass,
      ]"
    >
      #{{ sitting.rank }}
    </span>

    <!-- Duration + bar -->
    <div class="flex-1 flex flex-col gap-1.5 min-w-0">
      <span
        class="font-mono text-lg sm:text-xl font-extrabold text-content tracking-tight leading-none"
      >
        {{ formattedDuration }}
      </span>
      <div class="h-1 rounded-full bg-surface-alt overflow-hidden">
        <div
          :class="[
            'h-full rounded-full transition-all duration-500',
            barColorClass,
          ]"
          :style="{ width: barWidth }"
        />
      </div>
    </div>

    <!-- Time of day -->
    <span class="shrink-0 font-mono text-xs text-content-mute tabular-nums">
      at {{ formattedTimeOfDay }}
    </span>
  </div>
</template>
