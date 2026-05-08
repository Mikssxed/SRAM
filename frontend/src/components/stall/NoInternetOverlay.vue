<template>
  <StallOverlay class="bg-bg-base/72">
    <template #background>
      <div class="signal-static absolute inset-0 opacity-[0.04]" />
    </template>

    <template #corner>
      <div class="absolute top-4 right-4 px-3 py-1.5 rounded-full bg-yellow/12 border border-yellow/33 text-yellow text-[10px] font-bold font-mono tracking-[2px]">
        NO INTERNET
      </div>
    </template>

    <template #icon>
      <NoInternetIcon />
    </template>

    <template #label>
      <span class="text-[10px] tracking-[2.5px] text-yellow font-bold font-mono">⚠ YOU ARE OFFLINE ⚠</span>
    </template>

    <template #heading>No internet<br />connection</template>

    <template #description>
      Live data is unavailable while you're offline. Status will resume automatically once your connection is restored.
    </template>

    <template #footer>
      <div class="inline-flex items-center gap-2.5 px-4 py-2.5 mt-1 rounded-full border border-dashed border-yellow/40 text-yellow text-[12px] font-bold tracking-[1.5px] font-mono">
        <span class="w-2 h-2 rounded-full bg-yellow shadow-[0_0_6px_0_rgba(234,179,8,0.7)] animate-pulse-dot" />
        WAITING FOR CONNECTION…
      </div>
    </template>
  </StallOverlay>
</template>

<script setup lang="ts">
import NoInternetIcon from "../icons/NoInternetIcon.vue";
import StallOverlay from "./StallOverlay.vue";
</script>
