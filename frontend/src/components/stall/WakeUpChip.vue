<template>
  <div
    class="mt-1 px-4.5 py-3 rounded-2xl border border-edge bg-bg-elevated grid grid-cols-[auto_1px_auto] gap-3.5 items-center"
  >
    <div class="text-left">
      <div
        class="text-[9px] tracking-[1.8px] text-content-mute font-bold font-mono mb-1"
      >
        WAKES AT
      </div>
      <div
        class="text-[20px] font-bold text-content font-mono tracking-[0.5px]"
      >
        {{ wakesAt }}
      </div>
    </div>
    <div class="h-7 w-px bg-edge" />
    <div class="text-left">
      <div
        class="text-[9px] tracking-[1.8px] text-content-mute font-bold font-mono mb-1"
      >
        STATUS
      </div>
      <div class="text-[14px] font-bold font-mono text-teal">{{ status }}</div>
    </div>
  </div>
</template>

<script setup lang="ts">
defineProps<{ wakesAt: string; status: string }>();
</script>
