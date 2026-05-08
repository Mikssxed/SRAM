<script setup lang="ts">
import { computed } from "vue";
import AppHeader from "./components/layout/AppHeader.vue";
import StallCard from "./components/stall/StallCard.vue";
import TopStats from "./components/stats/TopStats.vue";
import { useFirebaseOccupancy } from "./composables/useFirebaseOccupancy";
import { useOccupancyState } from "./composables/useOccupancyState";
import { deriveAll } from "./utils/sensorCalc";

const { entries, currentOccupied, currentLastChangedAt, isError, sensorOffline, isOffline } =
  useFirebaseOccupancy();

const derived = computed(() => ({
  ...deriveAll(entries.value),
  occupied: currentOccupied.value,
  lastChangedAt: currentLastChangedAt.value,
}));
  

const { states } = useOccupancyState(derived);
</script>

<template>
  <main
    class="min-h-screen bg-linear-to-b from-bg-elevated to-bg-base text-content font-sans px-4 sm:px-8 pb-16 pt-7"
  >
    <div class="max-w-300 mx-auto flex flex-col gap-5 sm:gap-6">
      <AppHeader :is-error="isError" />

      <!-- Page title -->
      <div class="mt-1 sm:mt-2">
        <h1
          class="text-4xl sm:text-6xl font-black tracking-[-1.5px] sm:tracking-[-2px] m-0 leading-none"
        >
          Who's in the loo<span class="text-live-green">?</span>
        </h1>
        <p class="text-content-dim text-sm sm:text-base mt-2 sm:mt-3 max-w-130">
          Live occupancy for both restrooms. No more awkward door-rattles —
          check before you walk.
        </p>
      </div>

      <TopStats :states="states" :derived="derived" />

      <!-- Stall cards -->
      <div class="grid grid-cols-1 lg:grid-cols-2 gap-4 lg:gap-5">
        <StallCard
          kind="male"
          label="Men's Room"
          :state="states.male"
          :busy-hours="derived.busyHours"
          :sensor-offline="sensorOffline"
          :is-offline="isOffline"
        />
        <StallCard
          kind="female"
          label="Women's Room"
          :state="states.female"
          :coming-soon="true"
          :busy-hours="Array(24).fill(0)"
        />
      </div>
    </div>
  </main>
</template>
