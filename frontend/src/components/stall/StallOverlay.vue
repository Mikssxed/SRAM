<template>
  <div
    class="absolute inset-0 z-10 rounded-3xl overflow-hidden backdrop-blur-md flex items-center justify-center"
  >
    <slot name="background" />
    <slot name="corner" />

    <div
      class="relative flex flex-col items-center gap-4 text-center px-8 max-w-95"
    >
      <slot name="icon" />
      <slot name="label" />

      <h2
        class="text-[30px] font-extrabold text-content tracking-[-0.5px] leading-[1.1] m-0"
      >
        <slot name="heading" />
      </h2>

      <p class="text-[14px] text-content-dim leading-relaxed">
        <slot name="description" />
      </p>

      <slot name="footer" />
    </div>
  </div>
</template>
