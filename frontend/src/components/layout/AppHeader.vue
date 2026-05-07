<script setup lang="ts">
import LiveClock from "../ui/LiveClock.vue";

defineProps<{ isError?: boolean }>();
</script>

<template>
  <header class="flex justify-between items-center pb-2">
    <!-- Logo -->
    <div class="flex items-center gap-3">
      <div
        class="w-9.5 h-9.5 rounded-[10px] grid place-items-center text-xl bg-linear-to-br from-live-green to-teal shadow-header"
      >
        🚽
      </div>
      <div class="flex flex-col leading-none">
        <span class="text-lg font-extrabold text-content tracking-[-0.3px]">
          <span class="text-live-green">SRAM</span>
        </span>
      </div>
    </div>

    <!-- Right: location + clock -->
    <div class="flex items-center gap-3">
      <span
        class="hidden sm:inline text-xs text-content-dim font-mono tracking-[1px]"
        >FLOOR 1 · OFFICE</span
      >
      <LiveClock :is-error="isError" />
    </div>
  </header>
</template>
