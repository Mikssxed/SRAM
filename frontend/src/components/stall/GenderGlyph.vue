<script setup lang="ts">
import { computed } from "vue";
import type { StallKind } from "../../types/stall";

const props = defineProps<{ kind: StallKind; occupied: boolean }>();

const strokeColor = computed(() => (props.occupied ? "#ff5a4e" : "#3ddc84"));
</script>

<template>
  <svg
    v-if="kind === 'male'"
    viewBox="0 0 200 200"
    width="100%"
    height="100%"
    aria-hidden="true"
  >
    <circle
      cx="80"
      cy="120"
      r="50"
      fill="none"
      :stroke="strokeColor"
      stroke-width="14"
    />
    <line
      x1="115"
      y1="85"
      x2="170"
      y2="30"
      :stroke="strokeColor"
      stroke-width="14"
      stroke-linecap="round"
    />
    <polyline
      points="135,30 170,30 170,65"
      fill="none"
      :stroke="strokeColor"
      stroke-width="14"
      stroke-linecap="round"
      stroke-linejoin="round"
    />
  </svg>

  <svg
    v-else
    viewBox="0 0 200 200"
    width="100%"
    height="100%"
    aria-hidden="true"
  >
    <circle
      cx="100"
      cy="80"
      r="50"
      fill="none"
      :stroke="strokeColor"
      stroke-width="14"
    />
    <line
      x1="100"
      y1="130"
      x2="100"
      y2="185"
      :stroke="strokeColor"
      stroke-width="14"
      stroke-linecap="round"
    />
    <line
      x1="75"
      y1="160"
      x2="125"
      y2="160"
      :stroke="strokeColor"
      stroke-width="14"
      stroke-linecap="round"
    />
  </svg>
</template>
