import type { Ref } from "vue";
import { computed, ref, watch } from "vue";
import { PLAYFUL_COPY } from "../constants/copyConstants";
import type { AppStalls } from "../types/stall";
import type { DerivedStats } from "../utils/sensorCalc";

export function useOccupancyState(derived: Ref<DerivedStats>) {
  const copyIdx = ref(0);

  watch(
    () => derived.value.occupied,
    () => {
      copyIdx.value = Math.floor(Math.random() * PLAYFUL_COPY.free.length);
    },
  );

  const states = computed<AppStalls>(() => ({
    male: {
      occupied: derived.value.occupied,
      duration: Math.round((Date.now() - derived.value.lastChangedAt) / 1000),
      copyIdx: copyIdx.value,
      lastChange: derived.value.lastChangedAt,
    },
    female: {
      occupied: false,
      duration: 0,
      copyIdx: 0,
      lastChange: Date.now(),
    },
  }));

  return { states };
}
