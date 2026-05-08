<template>
  <div class="w-18 h-18 rounded-[20px] grid place-items-center bg-live-red/10 border border-live-red/33 shadow-glow-red">
    <svg viewBox="0 0 44 44" width="32" height="32" aria-hidden="true">
      <!-- Signal arc 1 (outermost) -->
      <path
        d="M5.5 18.5a23 23 0 0 1 33 0"
        class="stroke-live-red opacity-30"
        fill="none"
        stroke-width="3"
        stroke-linecap="round"
      />
      <!-- Signal arc 2 -->
      <path
        d="M10.5 23.5a16 16 0 0 1 23 0"
        class="stroke-live-red opacity-55"
        fill="none"
        stroke-width="3"
        stroke-linecap="round"
      />
      <!-- Signal arc 3 (innermost) -->
      <path
        d="M15.5 28.5a9 9 0 0 1 13 0"
        class="stroke-live-red opacity-80"
        fill="none"
        stroke-width="3"
        stroke-linecap="round"
      />
      <!-- Center dot -->
      <circle cx="22" cy="36" r="2.5" class="fill-live-red" />
      <!-- X slash -->
      <line x1="6" y1="6" x2="38" y2="38" class="stroke-live-red" stroke-width="3" stroke-linecap="round" />
    </svg>
  </div>
</template>
