<script lang="ts">
  interface Props {
    label: String;
    onclick: () => void;
  }

  let { label, onclick }: Props = $props();
</script>

<button {onclick}>{label}</button>
