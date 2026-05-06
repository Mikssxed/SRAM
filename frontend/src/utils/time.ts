export function fmtMMSS(totalSeconds: number): string {
  const m = Math.floor(totalSeconds / 60);
  const ss = totalSeconds % 60;
  return `${String(m).padStart(2, "0")}:${String(ss).padStart(2, "0")}`;
}
