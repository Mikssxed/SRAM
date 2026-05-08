<template>
  <div class="w-18 h-18 rounded-[20px] grid place-items-center bg-yellow/10 border border-yellow/33 shadow-[0_0_16px_0_rgba(234,179,8,0.18)]">
    <svg viewBox="0 0 44 44" width="32" height="32" aria-hidden="true">
      <!-- Globe circle -->
      <circle cx="22" cy="22" r="16" class="stroke-yellow opacity-60" fill="none" stroke-width="2.5" />
      <!-- Horizontal line -->
      <line x1="6" y1="22" x2="38" y2="22" class="stroke-yellow opacity-40" stroke-width="2" stroke-linecap="round" />
      <!-- Vertical ellipse hint (left arc) -->
      <path d="M22 6 Q14 22 22 38" class="stroke-yellow opacity-40" fill="none" stroke-width="2" stroke-linecap="round" />
      <!-- Vertical ellipse hint (right arc) -->
      <path d="M22 6 Q30 22 22 38" class="stroke-yellow opacity-40" fill="none" stroke-width="2" stroke-linecap="round" />
      <!-- X slash -->
      <line x1="8" y1="8" x2="36" y2="36" class="stroke-yellow" stroke-width="3" stroke-linecap="round" />
    </svg>
  </div>
</template>
