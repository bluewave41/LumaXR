<script lang="ts">
  import Sidecar from "$lib/Sidecar.svelte.ts";

  let logs = $state([]);

  const fetchLogs = async () => {
    const response = await Sidecar.write("read-logs");
    logs = response.logs;
  };

  (async () => {
    fetchLogs();
  })();
</script>

<h1>Logs</h1>
<textarea class="logs" readonly value={logs}></textarea>
<button onclick={fetchLogs}>Refresh</button>

<style>
  .logs {
    width: 100%;
    height: 70%;
  }
</style>
