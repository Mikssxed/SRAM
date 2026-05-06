<script setup lang="ts">
import { computed } from "vue";

const props = defineProps<{ occupied: boolean }>();

const label = computed(() => (props.occupied ? "Occupied" : "Available"));

const pillClasses = computed(() =>
  props.occupied
    ? "bg-live-red/10 border-live-red/33 text-live-red"
    : "bg-live-green/10 border-live-green/33 text-live-green",
);

const dotClasses = computed(() =>
  props.occupied
    ? "bg-live-red shadow-glow-red animate-pulse-dot"
    : "bg-live-green shadow-glow-green",
);
</script>

<template>
  <div
    :class="[
      'inline-flex items-center gap-2.5 px-3.5 py-2 rounded-full border',
      'text-xs font-bold tracking-[1.5px] uppercase font-mono',
      pillClasses,
    ]"
  >
    <span :class="['w-2 h-2 rounded-full', dotClasses]" />
    {{ label }}
  </div>
</template>
