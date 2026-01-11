<script lang="ts">
  interface TextInputProps {
    value: string;
    oninput: (event: Event) => void;
  }

  let { value, oninput }: TextInputProps = $props();
</script>

<input class="input" type="text" {value} {oninput} />

<style>
  .input {
    background-color: rgb(50, 50, 50);
    color: white;
    width: 6rem;
    border: 0;
  }
</style>
