<script lang="ts">
  import { goto } from "$app/navigation";
  import Sidecar from "$lib/Sidecar.svelte";

  (async () => {
    await Sidecar.start();
    goto("/clients");
  })();
</script>
