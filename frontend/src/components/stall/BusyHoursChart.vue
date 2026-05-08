<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";

interface Bar {
  heightPct: number;
  isNow: boolean;
}

const props = defineProps<{
  data: number[];
  occupied: boolean;
}>();

const currentHour = ref(new Date().getHours());

let hourInterval: ReturnType<typeof setInterval>;

onMounted(() => {
  hourInterval = setInterval(() => {
    currentHour.value = new Date().getHours();
  }, 60_000);
});

onUnmounted(() => {
  clearInterval(hourInterval);
});

const maxValue = computed(() => Math.max(...props.data));

const peakHour = computed(() => props.data.indexOf(maxValue.value));

const calculatePercentage = (value: number) =>
  maxValue.value === 0 ? 0 : Math.max(4, (value / maxValue.value) * 100);

const bars = computed<Bar[]>(() =>
  props.data.map((value, hour) => ({
    heightPct: calculatePercentage(value),
    isNow: hour === currentHour.value,
  })),
);

const barColorClass = computed(() =>
  props.occupied ? "bg-live-red" : "bg-live-green",
);

const barDimColorClass = computed(() =>
  props.occupied
    ? "bg-gradient-to-b from-live-red/53 to-live-red/20"
    : "bg-gradient-to-b from-live-green/53 to-live-green/20",
);
</script>

<template>
  <div>
    <!-- Header -->
    <div class="flex justify-between items-baseline mb-3">
      <span
        class="text-[10px] tracking-[1.8px] text-content-mute font-bold font-mono"
        >BUSY HOURS · TODAY</span
      >
      <span class="text-xs text-content-dim">
        peak <span class="text-content font-semibold">{{ peakHour }}:00</span>
      </span>
    </div>

    <!-- Bars -->
    <div class="grid grid-cols-[repeat(24,1fr)] gap-0.75 items-end h-16">
      <div
        v-for="(bar, i) in bars"
        :key="i"
        :class="[
          'rounded-[3px] transition-all duration-400',
          bar.isNow ? barColorClass : barDimColorClass,
          bar.isNow ? 'opacity-100' : 'opacity-70',
        ]"
        :style="{ height: `${bar.heightPct}%` }"
      />
    </div>

    <!-- Axis labels -->
    <div
      class="flex justify-between mt-2 text-[9px] tracking-[1px] text-content-mute font-mono"
    >
      <span>00</span>
      <span>06</span>
      <span>12</span>
      <span>18</span>
      <span>24</span>
    </div>
  </div>
</template>
