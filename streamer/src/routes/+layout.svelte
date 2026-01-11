<script>
  import Header from "../components/Header.svelte";
  import Navigation from "../components/Navigation.svelte";
  import Status from "../components/Status.svelte";

  let { children } = $props();
</script>

<Header />
<div class="content">
  <Navigation />
  <div class="main">
    {@render children()}
  </div>
  <Status />
</div>

<style>
  .content {
    display: flex;
    flex: 1;
    align-items: stretch;
  }
  .main {
    margin: 1rem;
    flex: 1;
  }
</style>
