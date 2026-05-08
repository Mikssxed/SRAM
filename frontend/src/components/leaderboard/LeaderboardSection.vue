<script setup lang="ts">
import { computed } from "vue";
import type { SittingRecord } from "../../utils/sensorCalc";
import LeaderboardRow from "./LeaderboardRow.vue";

const props = defineProps<{
  sittings: SittingRecord[];
}>();

const maxDurationMs = computed(() => props.sittings[0]?.durationMs ?? 0);
</script>

<template>
  <section class="bg-surface border border-edge rounded-3xl overflow-hidden">
    <!-- Header -->
    <div
      class="flex items-start justify-between gap-4 px-5 sm:px-6 pt-5 sm:pt-6 pb-4 sm:pb-5 border-b border-edge"
    >
      <div>
        <div class="flex items-center gap-2 mb-1">
          <h2
            class="text-base sm:text-lg font-extrabold tracking-[-0.5px] text-content leading-none"
          >
            Hall of Shame
          </h2>
        </div>
        <p
          class="text-[11px] tracking-[1.2px] text-content-mute font-bold font-mono uppercase"
        >
          Today's longest bathroom conquests
        </p>
      </div>
      <span
        class="shrink-0 text-[10px] tracking-[1.5px] font-bold font-mono text-amber bg-amber/10 border border-amber/20 rounded-full px-2.5 py-1"
      >
        TOP 10
      </span>
    </div>

    <!-- Empty state -->
    <div
      v-if="sittings.length === 0"
      class="flex flex-col items-center justify-center gap-3 px-6 py-10 text-center"
    >
      <span class="text-4xl">🚽</span>
      <p class="text-content-dim font-semibold text-sm max-w-64">
        The throne sits uncontested.
      </p>
      <p class="text-content-mute text-xs max-w-72">
        No completed sessions recorded today — either everyone's shockingly
        efficient, or the bathroom is having an identity crisis.
      </p>
    </div>

    <!-- Rows -->
    <div v-else>
      <LeaderboardRow
        v-for="sitting in sittings"
        :key="sitting.rank"
        :sitting="sitting"
        :max-duration-ms="maxDurationMs"
      />
    </div>
  </section>
</template>
