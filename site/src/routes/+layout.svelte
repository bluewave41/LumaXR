<script lang="ts">
	import favicon from '$lib/assets/favicon.svg';
	import Footer from '$lib/components/Footer.svelte';
	import Navigation from '$lib/components/Navigation.svelte';
	import '../globals.css';

	let { children } = $props();
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
</svelte:head>

<div class="layout">
	<Navigation />
	<main>
		{@render children()}
	</main>
	<Footer />
</div>

<style>
	main {
		flex: 1;
	}
	.layout {
		display: flex;
		flex-direction: column;
		min-height: 100vh;
	}
</style>
