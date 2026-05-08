<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref, watch } from "vue";
import { PLAYFUL_COPY } from "../../constants/copyConstants";
import type { StallKind, StallState } from "../../types/stall";
import { formatDuration } from "../../utils/time";
import BusyHoursChart from "./BusyHoursChart.vue";
import ComingSoonOverlay from "./ComingSoonOverlay.vue";
import GenderGlyph from "./GenderGlyph.vue";
import NightModeOverlay from "./NightModeOverlay.vue";
import SensorOfflineOverlay from "./SensorOfflineOverlay.vue";
import StatusPill from "./StatusPill.vue";

const props = defineProps<{
  kind: StallKind;
  label: string;
  state: StallState;
  busyHours: number[];
  comingSoon?: boolean;
  sensorOffline?: boolean;
}>();

const emit = defineEmits<{ "toggle-notify": [] }>();

const hour = ref(new Date().getHours());
let hourTimer: ReturnType<typeof setInterval>;

onMounted(() => {
  hourTimer = setInterval(() => {
    hour.value = new Date().getHours();
  }, 1000);
});

onUnmounted(() => clearInterval(hourTimer));

const sleeping = computed(() => hour.value >= 18 || hour.value < 6);

const tickedDuration = ref(props.state.duration);
let durationTimer: ReturnType<typeof setInterval>;

function restartTimer() {
  clearInterval(durationTimer);
  tickedDuration.value = props.state.duration;

  durationTimer = setInterval(() => {
    tickedDuration.value++;
  }, 1000);
}

watch(() => props.state.lastChange, restartTimer, { immediate: false });
watch(() => props.state.occupied, restartTimer);

onMounted(restartTimer);
onUnmounted(() => clearInterval(durationTimer));

const occupied = computed(() => props.state.occupied);

const elapsedDisplay = computed(() => formatDuration(tickedDuration.value));

const copyPhrase = computed(() => {
  const pool = occupied.value ? PLAYFUL_COPY.occupied : PLAYFUL_COPY.free;
  return pool[props.state.copyIdx % pool.length];
});

const elapsedLabel = computed(() =>
  occupied.value ? "ELAPSED" : "SINCE LAST USE",
);

const restroomLabel = computed(() =>
  props.kind === "male" ? "RESTROOM · M" : "RESTROOM · W",
);

const accentClasses = computed(() =>
  occupied.value
    ? {
        text: "text-live-red",
        cardShadow: "shadow-card-red",
        glowGradient: "glow-gradient-red",
        buttonBorder: "border-live-red/40",
        buttonBg: "bg-live-red/8",
      }
    : {
        text: "text-live-green",
        cardShadow: "shadow-card-green",
        glowGradient: "glow-gradient-green",
        buttonBorder: "border-live-green/40",
        buttonBg: "bg-live-green/8",
      },
);

const overlayComponent = computed(() => {
  if (props.comingSoon) return ComingSoonOverlay;
  if (sleeping.value) return NightModeOverlay;
  if (props.sensorOffline) return SensorOfflineOverlay;
  return null;
});
</script>

<template>
  <div
    :class="[
      'relative bg-surface border border-edge rounded-3xl p-5 sm:p-8 flex flex-col gap-5 sm:gap-6 overflow-hidden transition-all duration-500',
      accentClasses.cardShadow,
    ]"
  >
    <component :is="overlayComponent" />

    <!-- Background glow -->
    <div
      class="absolute inset-0 pointer-events-none transition-all duration-600"
      :class="accentClasses.glowGradient"
    />

    <!-- Top row: label + status pill -->
    <div class="relative flex justify-between items-start gap-4 flex-wrap">
      <div>
        <div
          class="text-[11px] tracking-[2px] text-content-mute font-bold font-mono mb-2"
        >
          {{ restroomLabel }}
        </div>
        <div
          class="text-[28px] font-extrabold tracking-[-0.5px] text-content whitespace-nowrap"
        >
          {{ label }}
        </div>
      </div>
      <StatusPill :occupied="occupied" />
    </div>

    <!-- Hero block: icon + copy + elapsed timer -->
    <div
      class="relative bg-bg-elevated border border-edge rounded-[18px] p-4 sm:p-7 grid grid-cols-[auto_1fr] gap-4 sm:gap-7 items-center sm:min-h-50"
    >
      <div class="w-20 h-20 sm:w-32.5 sm:h-32.5">
        <GenderGlyph :kind="kind" :occupied="occupied" />
      </div>

      <div class="flex flex-col gap-2 sm:gap-3">
        <div
          :class="[
            'text-[20px] sm:text-[30px] font-extrabold tracking-[-0.5px] leading-[1.1]',
            accentClasses.text,
          ]"
        >
          {{ copyPhrase }}
        </div>
        <div
          class="text-[11px] sm:text-[13px] text-content-dim font-mono tracking-[1px]"
        >
          {{ elapsedLabel }}
        </div>
        <div
          class="text-[40px] sm:text-[56px] font-bold text-content font-mono tracking-[-1px] leading-none"
        >
          {{ elapsedDisplay }}
        </div>
      </div>
    </div>
    <!-- Busy hours chart -->
    <BusyHoursChart :data="busyHours" :occupied="occupied" />
  </div>
</template>
