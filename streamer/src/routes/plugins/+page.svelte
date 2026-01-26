<script>
  import Sidecar from "$lib/Sidecar.svelte";

  let plugins = $state([]);
  let env = "";

  const keys = ["create-monitor"];

  // check settings for which plugin are active
  // activate those
  // otherwise activate matching arch first
  // otherwise activate any

  (async () => {
    env = (await Sidecar.write("get-env")).env;
    const response = await Sidecar.write("get-plugins");
    plugins = response.plugins.filter(
      (plugin) => plugin.env === env[1] || plugin.env === "any",
    );

    plugins.map((plugin) => (plugin.activated = false));

    for (const key of keys) {
      // activated plugins from settings
      // arch match
      let plugin = plugins.find(
        (plugin) => plugin.key === key && plugin.env === env[1],
      );
      if (!plugin) {
        plugin = plugins.find(
          (plugin) => plugin.key === key && plugin.env === "any",
        );
      }
      plugin.activated = true;
      // any
    }
  })();

  const onPluginActive = (e) => {
    const target = e.currentTarget;
    const split = target.name.split("-");
    const env = split[0];
    const key = split.slice(1).join("-");

    for (const plugin of plugins) {
      if (plugin.key === key) {
        plugin.activated = false;
      }
    }

    // alert backend to update
    Sidecar.write("update-active-plugins", { name: target.name });
  };
</script>

<h1>Plugins</h1>
{#each plugins as plugin}
  {@const name = `${plugin.env}-${plugin.key}`}
  <div class="row">
    <input
      type="checkbox"
      {name}
      bind:checked={plugin.activated}
      onclick={onPluginActive}
    />
    <p>{name}</p>
  </div>
{/each}

<style>
  .row {
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }
</style>
