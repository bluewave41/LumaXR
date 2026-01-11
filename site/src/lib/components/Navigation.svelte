<script lang="ts">
	import { page } from '$app/state';

	let active = $derived(page.url.pathname.slice(1).toUpperCase());
</script>

<div class="container science-gothic">
	<div class="image">
		<img src="images/logo-simple.png" class="logo" alt="logo" />
		<h2 class="science-gothic">LumaXR</h2>
	</div>
	<div class="row">
		<a href="/" class={['link', { active: active === '' }]}>HOME</a>
		<a href="/about" class={['link', { active: active === 'ABOUT' }]}>ABOUT</a>
		<a href="/faq" class={['link', { active: active === 'FAQ' }]}>FAQ</a>
	</div>
</div>

<style>
	.container {
		padding: 0.5rem 2rem;
		display: flex;
		justify-content: space-between;
		align-items: center;
		background-color: #111;
	}
	.logo {
		width: 6rem;
		height: 6rem;
	}
	.image {
		display: flex;
		align-items: center;
		gap: 1rem;
	}
	.row {
		display: flex;
		gap: 1rem;
	}
	.link {
		color: white;
		text-decoration: none;
	}
	.active {
		color: var(--primary);
	}
</style>
