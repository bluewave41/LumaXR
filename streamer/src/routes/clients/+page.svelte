<script lang="ts">
  import Sidecar from "$lib/Sidecar.svelte.ts";
  import { onMount } from "svelte";

  onMount(() => {
    window.addEventListener("beforeunload", () => {
      Sidecar.stop();
    });
  });
</script>

<h1>Clients</h1>
<main></main>
