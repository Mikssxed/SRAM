import type { AppStalls } from "../types/stall";

// Female stall mock — kept until sensor hardware is installed
export const BUSY_HOURS_FEMALE: number[] = [
  1, 0, 0, 0, 0, 1, 2, 5, 8, 6, 4, 9, 13, 10, 6, 5, 7, 10, 12, 7, 5, 4, 2, 1,
];

// Female stall initial state placeholder
export const INITIAL_STALL_STATES: AppStalls = {
  male: {
    occupied: false,
    duration: 0,
    copyIdx: 0,
    notify: false,
    lastChange: Date.now(),
  },
  female: {
    occupied: false,
    duration: 47,
    copyIdx: 1,
    notify: false,
    lastChange: Date.now(),
  },
};

export const PLAYFUL_COPY = {
  free: [
    "Ready to roll",
    "Throne awaits",
    "All clear, captain",
    "Open for business",
  ],
  occupied: [
    "In use 🚽",
    "Mission in progress",
    "Currently occupied",
    "Someone's on the throne",
  ],
};
