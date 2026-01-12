<script lang="ts">
  let activeTab = "CLIENTS";

  const tabs = ["SCREENS", "SETTINGS", "LOGS", "DEBUG", "PLUGINS"];
</script>

<div class="container">
  <div class="links">
    {#each tabs as tab}
      <a
        href={tab.toLowerCase()}
        class={["link", { active: activeTab === tab }]}
        onclick={() => (activeTab = tab)}>{tab}</a
      >
    {/each}
  </div>
</div>

<style>
  .container {
    background-color: rgb(21, 21, 21);
    width: 6rem;
    height: 100%;
    display: flex;
    flex-direction: column;
  }
  .links {
    display: flex;
    flex-direction: column;
    padding: 0.5rem;
    gap: 1rem;
  }
  .link {
    all: unset;
    &:hover {
      cursor: pointer;
    }
  }
  .active {
    color: aqua;
  }
</style>
