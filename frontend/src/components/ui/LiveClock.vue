<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";

const props = defineProps<{ isError?: boolean }>();

const now = ref(new Date());
let timer: ReturnType<typeof setInterval>;

onMounted(() => {
  timer = setInterval(() => {
    now.value = new Date();
  }, 1000);
});
onUnmounted(() => clearInterval(timer));

const hours = computed(() => String(now.value.getHours()).padStart(2, "0"));
const minutes = computed(() => String(now.value.getMinutes()).padStart(2, "0"));
const seconds = computed(() => String(now.value.getSeconds()).padStart(2, "0"));

const timeLabel = computed(() => `${hours.value}:${minutes.value}`);

const statusLabel = computed(() => (props.isError ? "OFFLINE" : "LIVE"));
const statusClass = computed(() =>
  props.isError
    ? "text-[11px] text-red-400 tracking-[1px]"
    : "text-[11px] text-content-dim tracking-[1px]",
);
const dotClass = computed(() =>
  props.isError
    ? "w-2 h-2 rounded-full bg-red-400"
    : "w-2 h-2 rounded-full bg-live-green shadow-glow-green animate-pulse-dot",
);
</script>

<template>
  <div
    class="flex items-center gap-2.5 px-3.5 py-2 rounded-full border border-edge bg-surface font-mono"
  >
    <span :class="dotClass" />
    <span :class="statusClass">{{ statusLabel }}</span>
    <span class="text-[13px] font-semibold text-content">
      {{ timeLabel }}<span class="text-content-mute">:{{ seconds }}</span>
    </span>
  </div>
</template>
