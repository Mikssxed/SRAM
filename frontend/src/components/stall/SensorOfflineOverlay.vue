<template>
  <StallOverlay class="bg-bg-base/72">
    <template #background>
      <div class="signal-static absolute inset-0 opacity-[0.06]" />
    </template>

    <template #corner>
      <div class="absolute top-4 right-4 px-3 py-1.5 rounded-full bg-live-red/12 border border-live-red/33 text-live-red text-[10px] font-bold font-mono tracking-[2px] rotate-0">
        NO SIGNAL
      </div>
    </template>

    <template #icon>
      <OfflineSensorIcon />
    </template>

    <template #label>
      <span class="text-[10px] tracking-[2.5px] text-live-red font-bold font-mono">⚡ SENSOR OFFLINE ⚡</span>
    </template>

    <template #heading>Signal lost —<br />checking back soon</template>

    <template #description>
      The sensor isn't responding right now. We're watching for it to come back — live status will return automatically.
    </template>

    <template #footer>
      <div class="inline-flex items-center gap-2.5 px-4 py-2.5 mt-1 rounded-full border border-dashed border-live-red/40 text-live-red text-[12px] font-bold tracking-[1.5px] font-mono">
        <span class="w-2 h-2 rounded-full bg-live-red shadow-glow-red animate-pulse-dot" />
        RECONNECTING…
      </div>
    </template>
  </StallOverlay>
</template>

<script setup lang="ts">
import OfflineSensorIcon from "../icons/OfflineSensorIcon.vue";
import StallOverlay from "./StallOverlay.vue";
</script>
