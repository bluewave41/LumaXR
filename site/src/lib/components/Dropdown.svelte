<script lang="ts">
	import ArrowDown from '$lib/icons/arrow-down.svelte';
	import ArrowUp from '$lib/icons/arrow-up.svelte';

	interface Props {
		question: string;
		answer: String;
	}
	let { question, answer }: Props = $props();

	let open = $state(false);
</script>

<button class="dropdown" onclick={() => (open = !open)}>
	<div class="row">
		{#if !open}
			<ArrowDown size="1rem" />
		{:else if open}
			<ArrowUp size="1rem" />
		{/if}
		<p>{question}</p>
	</div>

	{#if open}
		<hr />
		<p>{answer}</p>
	{/if}
</button>

<style>
	.dropdown {
		display: flex;
		flex-direction: column;
		gap: 1rem;
		width: 100%;
		background-color: #111;
		padding: 1rem;
		border: 2px solid var(--tertiary);
		color: white;
		text-align: left;

		&:hover {
			cursor: pointer;
		}
	}
	.row {
		display: flex;
		align-items: center;
		gap: 1rem;
	}
</style>
