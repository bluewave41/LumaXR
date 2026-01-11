<script lang="ts">
  import Sidecar from "$lib/Sidecar.svelte";
  import TextInput from "../../components/TextInput.svelte";

  interface Coordinates {
    pos: number[];
    rot: number[];
    scale: number[];
  }

  let clientIp = "";

  const axes = ["X", "Y", "Z"];

  let streams = $state([]);

  let stream = $state();

  let position: Coordinates | null = $state(null);

  let ws: WebSocket | null;

  (async () => {
    const response = await Sidecar.write("get-streams");
    const { settings } = await Sidecar.write("read-settings");
    streams = response.streams;
    clientIp = settings.clientIp;

    ws = new WebSocket(`ws://${clientIp}:8572/position`);
    ws.onopen = () => {
      console.log("Opened!");
    };
    ws.onerror = () => {
      console.log("error!");
    };
    ws.onmessage = (msg) => {
      const data = JSON.parse(msg.data);
      position = data;
    };
  })();

  const onInput = (
    field: keyof Coordinates,
    index: number,
    event: InputEvent
  ) => {
    if (position === null) {
      return;
    }
    const target = event.currentTarget as HTMLInputElement;
    console.log("edited", field, index, target.value);
    position[field][index] = parseFloat(target.value);
  };

  const onSelectStream = (index: number) => {
    stream = streams.at(index);
  };

  const onRequestPositions = (id: number) => {
    if (ws) {
      ws.send(JSON.stringify({ id }));
    }
  };

  const onSendPositions = () => {
    if (ws) {
      ws.send(JSON.stringify({ id: 1, position }));
    }
  };
</script>

<h1>Screens</h1>
{#if streams.length === 0}
  <p>No clients connected.</p>
{/if}

{#if streams.length > 0}
  <div class="tabs">
    {#each streams as stream, i}
      <button class="tab" onclick={() => onSelectStream(i - 1)}>{i + 1}</button>
    {/each}
  </div>
{/if}

{#if stream}
  <div class="container">
    <p class="center">{stream.width}</p>
    <div class="row">
      <p>{stream.height}</p>
      <div class="box">
        {#if position}
          <div class="grid">
            <p>Position</p>
            {#each axes as axis, i}
              <div class="row">
                <p>{axis}</p>
                <TextInput
                  value={Number(position.pos[i].toFixed(7)).toString()}
                  oninput={(value) => onInput("pos", i, value)}
                />
              </div>
            {/each}
          </div>
          <div class="grid">
            <p>Rotation</p>
            {#each axes as axis, i}
              <div class="row">
                <p>{axis}</p>
                <TextInput
                  value={Number(position.rot[i].toFixed(7)).toString()}
                  oninput={(value) => onInput("rot", i, value)}
                />
              </div>
            {/each}
          </div>
          <div class="grid">
            <p>Scale</p>
            {#each axes as axis, i}
              <div class="row">
                <p>{axis}</p>
                <TextInput
                  value={Number(position.scale[i].toFixed(7)).toString()}
                  oninput={(value) => onInput("scale", i, value)}
                />
              </div>
            {/each}
          </div>
        {/if}
      </div>
    </div>
  </div>

  <p>Port: {stream.port}</p>
  <button onclick={() => onRequestPositions(streams.indexOf(stream) + 1)}
    >Request positions</button
  >
  <button onclick={onSendPositions}>Send positions</button>
{/if}

<style>
  .box {
    display: flex;
    flex-direction: column;
    justify-content: center;
  }
  .container {
    display: flex;
    flex-direction: column;
    gap: 0.5rem;
  }
  .row {
    display: flex;
    flex: 1;
    gap: 0.5rem;
  }
  .grid {
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    padding-left: 0.5rem;
  }
  .center {
    text-align: center;
  }
  .box {
    border: 1px solid aqua;
    height: 8rem;
    flex: 1;
  }
  .tabs {
    display: flex;
    background-color: rgb(29, 29, 29);
    border: 2px solid aqua;
  }
  .tab {
    all: unset;
    width: 6rem;
    text-align: center;

    &:hover {
      background-color: rgb(50, 50, 50);
      cursor: pointer;
    }
  }
</style>
