<script lang="ts">
  import Sidecar from "$lib/Sidecar.svelte";
  import Button from "../../components/Button.svelte";

  const startStream = () => {
    Sidecar.write("debug-start-stream");
  };
</script>

<h1>Debug</h1>

<Button label="Force start stream" onclick={startStream} />
