<script>
	import Dropdown from '$lib/components/Dropdown.svelte';
</script>

<div class="container">
	<h1 class="science-gothic">FAQ</h1>
	<Dropdown
		question="What environments are supported?"
		answer="LumaXR current only supports GNOME 40+. Virtual displays are created using private
		Mutter API's as the open freedesktop API prompts users to accept a screen cast request whenever
		a stream is created or destroyed."
	/>
	<Dropdown
		question="When will X be supported?"
		answer="Most likely never but there's nothing stopping you from adding support! The open nature of the
		streamer means you can customize the behaviour entirely on your own."
	/>
	<Dropdown
		question="Can I connect over the internet instead of locally?"
		answer="Internet connections are not supported at this time."
	/>
</div>

<style>
	.container {
		display: flex;
		flex-direction: column;
		gap: 1rem;
		align-items: center;
		margin: 1rem;
	}
</style>
