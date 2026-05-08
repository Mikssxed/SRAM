<template>
  <StallOverlay class="bg-bg-base/72">
    <template #background>
      <div class="hazard-stripes absolute inset-0 opacity-[0.08]" />
    </template>

    <template #corner>
      <ComingSoonRibbon />
    </template>

    <template #icon>
      <div
        class="w-18 h-18 rounded-[20px] grid place-items-center text-3xl bg-amber/10 border border-amber/33 shadow-glow-amber-lg"
      >
        🔧
      </div>
    </template>

    <template #label>
      <span class="text-[10px] tracking-[2.5px] text-amber font-bold font-mono"
        >⚡ SENSOR PENDING ⚡</span
      >
    </template>

    <template #heading>Women's room<br />coming soon</template>

    <template #description>
      We're wiring up the sensor next. Live status will light up here — stay
      tuned.
    </template>

    <template #footer>
      <div
        class="inline-flex items-center gap-2.5 px-4 py-2.5 mt-1 rounded-full border border-dashed border-amber/40 text-amber text-[12px] font-bold tracking-[1.5px] font-mono"
      >
        <span
          class="w-2 h-2 rounded-full bg-amber shadow-glow-amber animate-pulse-dot"
        />
        ETA · Soon
      </div>
    </template>
  </StallOverlay>
</template>

<script setup lang="ts">
import ComingSoonRibbon from "./ComingSoonRibbon.vue";
import StallOverlay from "./StallOverlay.vue";
</script>
