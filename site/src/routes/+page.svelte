<script>
	import Button from '$lib/components/Button.svelte';
	import Customize from '$lib/icons/customize.svelte';
	import Screen from '$lib/icons/screen.svelte';
	import Wrench from '$lib/icons/wrench.svelte';
</script>

<div class="container">
	<div class="hero">
		<div class="content">
			<h1 class="roboto">Your desktop in VR. Your way.</h1>
			<p>
				LumaXR is an unopinionated VR streaming tool for Linux. Everything is user customizable with
				no restrictions.
			</p>
			<div>
				<Button text="Download Streamer" logo="linux.png" />
			</div>
		</div>
	</div>
	<div class="slogan"></div>

	<div class="tiles">
		<div class="tile">
			<Screen size="3rem" />
			<h2>Multiple Displays</h2>
			<p>
				One screen or ten, entirely up to you. LumaXR doesn't impose any restrictions. If you think
				your system can handle it you should be able to decide what you can do.
			</p>
		</div>
		<div class="tile">
			<Wrench size="3rem" />
			<h2>Fully Customizable</h2>
			<p>
				LumaXR was built to be customized. Size, distance, resolution, refresh rate, those are only
				some of the options you're free to edit.
			</p>
		</div>
		<div class="tile">
			<Customize size="3rem" />
			<h2>Fine-grained controls</h2>
			<p>
				Create your dream setup with fine grained numeric controls. Customize size, scale and
				rotations down to the pixel to place screens exactly where you need them.
			</p>
		</div>
	</div>
</div>

<style>
	.container {
		display: flex;
		flex-direction: column;
		gap: 1rem;
	}
	.hero {
		position: relative;
		height: 32rem;
		background-image: url('/images/background.png');
	}
	.content {
		display: flex;
		flex-direction: column;
		gap: 0.5rem;
		position: absolute;
		top: 60%;
		left: 5%;
	}
	.slogan {
		display: grid;
		place-items: center;
	}
	.tiles {
		display: flex;
		justify-content: center;
		gap: 2rem;
		margin: 0 1rem;
	}

	.tile {
		flex: 1;
		display: flex;
		flex-direction: column;
		justify-content: center;
		align-items: center;
		padding: 1rem;
		text-align: center;
		background-color: #111;
		border-radius: 0.5rem;
		border: 2px solid var(--tertiary);
	}
</style>
