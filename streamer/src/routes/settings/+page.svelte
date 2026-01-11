<script lang="ts">
  import Sidecar from "$lib/Sidecar.svelte";

  interface Settings {
    senderPipeline: string;
    receiverPipeline: string;
    virtualOnly: boolean;
    linuxMode: boolean;
  }

  let settings = $state<Settings>({
    senderPipeline: "",
    receiverPipeline: "",
    virtualOnly: true,
    linuxMode: false,
  });

  $inspect(settings);

  (async () => {
    const response = await Sidecar.write("read-settings");
    settings = response.settings;
  })();

  const onSave = () => {
    console.log("Saving settings");
    Sidecar.write("save-settings", { settings: JSON.stringify(settings) });
  };
</script>

<div class="container">
  <div class="pipelines">
    <div class="col">
      Sender
      <textarea rows={10} bind:value={settings.senderPipeline}></textarea>
    </div>
    <div class="col">
      Receiver<textarea rows={10} bind:value={settings.receiverPipeline}
      ></textarea>
    </div>
  </div>
  <div class="row">
    <label for="virtualOnly">Virtual only</label>
    <input
      type="checkbox"
      name="virtualOnly"
      bind:checked={settings.virtualOnly}
    />
  </div>
  <div class="row">
    <label for="linuxMode">Linux mode</label>
    <input type="checkbox" name="linuxMode" bind:checked={settings.linuxMode} />
  </div>

  <button onclick={onSave}>Save</button>
</div>

<style>
  .container {
    display: flex;
    flex-direction: column;
    gap: 1rem;
  }
  .pipelines {
    display: flex;
    gap: 1rem;
  }
  .row {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }
  .col {
    display: flex;
    flex: 1;
    flex-direction: column;
  }
  input[type="checkbox"] {
    width: 15px;
    height: 15px;
  }
</style>
