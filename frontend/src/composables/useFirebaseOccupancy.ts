import { initializeApp } from "firebase/app";
import {
  type DataSnapshot,
  get,
  getDatabase,
  onChildAdded,
  onValue,
  orderByChild,
  query,
  ref,
  startAt,
} from "firebase/database";
import { onMounted, onUnmounted, ref as vueRef } from "vue";
import type { OccupancyEntry } from "../types/sensor";

const app = initializeApp({
  apiKey: import.meta.env.VITE_FIREBASE_API_KEY as string,
  databaseURL: import.meta.env.VITE_FIREBASE_DATABASE_URL as string,
  projectId: import.meta.env.VITE_FIREBASE_PROJECT_ID as string,
  appId: import.meta.env.VITE_FIREBASE_APP_ID as string,
});

const db = getDatabase(app);
const historyRef = ref(db, "restrooms/men1/history");
const currentRef = ref(db, "restrooms/men1/current");

export function useFirebaseOccupancy() {
  const entries = vueRef<OccupancyEntry[]>([]);
  const currentOccupied = vueRef(false);
  const currentLastChangedAt = vueRef(Date.now());
  const isLoading = vueRef(true);
  const isError = vueRef(false);
  const lastTs = vueRef<number | undefined>(undefined);

  let unsubscribeHistory: (() => void) | null = null;
  let unsubscribeCurrent: (() => void) | null = null;

  async function initialLoad() {
    try {
      const [historySnap, currentSnap] = await Promise.all([
        get(query(historyRef, orderByChild("timestamp"))),
        get(currentRef),
      ]);

      const sorted: OccupancyEntry[] = [];
      historySnap.forEach((child: DataSnapshot) => {
        sorted.push(child.val() as OccupancyEntry);
      });
      entries.value = sorted;

      const last = sorted.at(-1);
      const occupied: boolean = currentSnap.val()?.occupied ?? false;
      currentOccupied.value = occupied;
      currentLastChangedAt.value = last
        ? last.occupied === occupied
          ? last.timestamp
          : Date.now()
        : Date.now();

      lastTs.value = last?.timestamp;

      isError.value = false;
    } catch {
      isError.value = true;
    } finally {
      isLoading.value = false;
    }
  }

  onMounted(async () => {
    await initialLoad();

    const since = lastTs.value !== undefined ? lastTs.value + 1 : Date.now();
    unsubscribeHistory = onChildAdded(
      query(historyRef, orderByChild("timestamp"), startAt(since)),
      (snap: DataSnapshot) => {
        const entry = snap.val() as OccupancyEntry;
        entries.value.push(entry);
        lastTs.value = entry.timestamp;
        isError.value = false;
      },
      () => {
        isError.value = true;
      },
    );

    unsubscribeCurrent = onValue(
      currentRef,
      (snap: DataSnapshot) => {
        const occupied: boolean = snap.val()?.occupied ?? false;
        if (occupied !== currentOccupied.value) {
          currentOccupied.value = occupied;
          currentLastChangedAt.value = Date.now();
        }
        isError.value = false;
      },
      () => {
        isError.value = true;
      },
    );
  });

  onUnmounted(() => {
    unsubscribeHistory?.();
    unsubscribeCurrent?.();
  });

  return { entries, currentOccupied, currentLastChangedAt, isLoading, isError };
}
