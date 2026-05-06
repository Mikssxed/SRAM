import type { Ref } from "vue";
import { computed, ref, watch } from "vue";
import { INITIAL_STALL_STATES, PLAYFUL_COPY } from "../data/mockOccupancy";
import type { AppStalls } from "../types/stall";
import type { DerivedStats } from "../utils/sensorCalc";

export function useOccupancyState(derived: Ref<DerivedStats>) {
  // UI-only state: not derived from sensor data
  const copyIdx = ref(0);
  const notify = ref(false);

  watch(
    () => derived.value.occupied,
    () => {
      copyIdx.value = Math.floor(Math.random() * PLAYFUL_COPY.free.length);
      notify.value = false;
    },
  );

  const states = computed<AppStalls>(() => ({
    male: {
      occupied: derived.value.occupied,
      duration: Math.round((Date.now() - derived.value.lastChangedAt) / 1000),
      copyIdx: copyIdx.value,
      notify: notify.value,
      lastChange: derived.value.lastChangedAt,
    },
    female: INITIAL_STALL_STATES.female,
  }));

  return { states };
}
