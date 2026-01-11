<script lang="ts">
	interface Props {
		text: string;
		logo?: string;
	}

	let { text, logo }: Props = $props();
</script>

<button class="button science-gothic" onclick={() => console.log('wow')}
	>{text}
	{#if logo}
		<img src={`images/${logo}`} />
	{/if}
</button>

<style>
	.button {
		border: 0;
		color: white;
		display: flex;
		align-items: center;
		background: linear-gradient(var(--primary), var(--secondary));
		padding: 0.5rem;
		border-radius: 0.5rem;

		&:hover {
			cursor: pointer;
		}
	}
</style>
