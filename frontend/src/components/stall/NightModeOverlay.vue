<template>
  <StallOverlay class="night-overlay-bg">
    <template #background>
      <div class="absolute inset-0 stars" />
    </template>

    <template #corner>
      <BatterySavingBadge />
    </template>

    <template #icon>
      <SleepingMoonIcon />
    </template>

    <template #label>
      <span class="text-[10px] tracking-[2.5px] font-bold font-mono text-teal"
        >⌀ NIGHT MODE ⌀</span
      >
    </template>

    <template #heading>Sensors are<br />catching some Zs</template>

    <template #description>
      We power down at night to save battery. Live status resumes at dawn —
      sleep tight.
    </template>

    <template #footer>
      <WakeUpChip wakes-at="06:00" status="STANDBY" />
    </template>
  </StallOverlay>
</template>

<script setup lang="ts">
import SleepingMoonIcon from "../icons/SleepingMoonIcon.vue";
import BatterySavingBadge from "./BatterySavingBadge.vue";
import StallOverlay from "./StallOverlay.vue";
import WakeUpChip from "./WakeUpChip.vue";
</script>
